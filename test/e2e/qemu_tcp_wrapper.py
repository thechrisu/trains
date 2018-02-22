import os
import signal
import socket
import socketserver
import sys
import time
from queue import Queue
from subprocess import Popen, PIPE, call

dir_path = os.path.dirname(os.path.realpath(__file__))

tr_sv_addr = ('127.0.0.1', 9990)
te_sv_addr = ('127.0.0.1', 9991)


class TwoWayHandler(socketserver.StreamRequestHandler):
    def __init__(self, val):
        self.val = val
        self.num_handles = 0

    def handle(self):
        self.request.send(self.val[2])
        self.val[0]((self.rfile.read(), self.val[1]))

    def __call__(self, req, addr, serv):
        socketserver.StreamRequestHandler.__init__(
            TwoWayHandler(self.val), req, addr, serv)


class ThreadingTCPServer(socketserver.ThreadingMixIn, socketserver.TCPServer):
    pass


def handle_twice(server):
    server.handle_request()
    server.handle_request()


def send_data_socket(s, data, queue, addr, prog):
    # tcp_lock.acquire()
    s.connect(addr)
    s.sendall(bytes('%s\r' % prog, 'ascii'))
    s.sendall(bytes('%s' % data, 'ascii'))
    # s.sendall(bytes(data + 'ENDOFTRANSMISSION', 'ascii'))
    tr_recv = ''
    while True:
        d = s.recv(10)
        if not d:
            break
        tr_recv += d.decode('ascii')
    queue.put(tr_recv)
    s.close()
    # tcp_lock.release()


def read_socket(sock, limit=None, may_send_cr=False):
    received = ''
    i = 0
    while limit is None or i < limit:
        d = sock.recv(1)
        if not d or (not may_send_cr and d.decode('ascii') == '\r'):
            break
        received += d.decode('ascii')
        if 'Enter a test program (q to exit): \n\r' in received:
            received = received.split('Enter a test program (q to exit): \n\r')[0]
            break
        if 'ENDPROG\n\r' in received:
            received = received.split('ENDPROG\n\r')[0]
            break
    return received if len(received) > 0 else None


old_stdout = sys.stdout
old_stdin = sys.stdin
old_stderr = sys.stderr


def kill_qemu(handle):
    handle.stdout.close()
    handle.stdin.close()
    handle.stderr.close()
    if handle.poll() is None:
        os.killpg(os.getpgid(handle.pid), signal.SIGTERM)
    sys.stdout = old_stdout
    sys.stdin = old_stdin
    sys.stderr = old_stderr
    handle.wait()


def call_qemu_tcp(optimized, timer_interrupts_on, iointerrupts_on):
    os.chdir(os.path.join(dir_path, '../..'))
    popen_arg = 'exec make qemutcprun FIFOS=true %s%s%s 2>&1' % ('o' if optimized else '', ' TIMER_INTERRUPTS=true' if timer_interrupts_on else '', ' IOINTERRUPTS=true' if iointerrupts_on else '')
    handle = Popen(popen_arg, shell=True, stdout=PIPE,
                   stdin=PIPE, stderr=PIPE, preexec_fn=os.setsid)  # , env=os.environ.copy())
    lines = ''
    while True:
        line = handle.stdout.readline().decode('utf-8')
        lines += line if line != '' else ''
        if 'QEMU waiting' in line:
            return handle
        elif 'failed' in line or 'Failed' in line:
            kill_qemu(handle)
            raise ConnectionError(lines)


def qemu_oneshot_test(prog, te_data, timeout, timer_interrupts_on=False,
                      iointerrupts_on=False, will_segfault=False, send_pause=None):
    qemu_handle = call_qemu_tcp(False, timer_interrupts_on, iointerrupts_on)
    q = Queue()
    # time.sleep(1)
    try:
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.settimeout(timeout)

        def handle_signal(sign, frame):
            return kill_qemu(qemu_handle)

        signal.signal(signal.SIGTERM, handle_signal)
        signal.signal(signal.SIGINT, handle_signal)
        s.connect(te_sv_addr)
        junk = read_socket(s, 1000)
        if junk is None:
            kill_qemu(qemu_handle)
            s.close()
            return None
        else:
            prog_name = '%s\r' % prog
            assert s.sendall(bytes(prog_name, 'ascii')) is None
            if not send_pause is None:
                for i in te_data:
                    time.sleep(0.05)
                    assert s.sendall(bytes(i, 'ascii')) is None
            else:
                assert s.sendall(bytes(te_data, 'ascii')) is None
            prog_output = read_socket(s, may_send_cr=True)
            if prog_output.startswith(prog + '\n\r'):
                prog_output = prog_output.split(prog + '\n\r')[1]
            assert s.sendall(bytes('q\r', 'ascii')) is None
            if not will_segfault and \
               (read_socket(s, 1000) is None or read_socket(s, 1000) is None):
                print(prog_output)
                kill_qemu(qemu_handle)
                s.close()
                raise ValueError
            kill_qemu(qemu_handle)
            s.close()
            return prog_output
    except(ConnectionAbortedError, ConnectionError) as e:
        raise e
    except:
        kill_qemu(qemu_handle)
        return None


if __name__ == "__main__":
    r = qemu_oneshot_test('test', '', 10)
    s = qemu_oneshot_test('k1', '', 10)
    print(r)
    print(s)
