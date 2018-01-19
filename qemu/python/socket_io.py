from subprocess import Popen, PIPE
import socketserver
import socket
import sys
from queue import Queue
import signal
import threading

import os

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
        socketserver.StreamRequestHandler.__init__(TwoWayHandler(self.val), req, addr, serv)


old_stdout = sys.stdout
old_stdin = sys.stdin
old_stderr = sys.stderr


def call_qemu_tcp(optimized):
    os.chdir(os.path.join(dir_path, '../..'))
    if len(sys.argv) > 1 and sys.argv[1] == 'win':
        popen_arg = 'exec make qemutcpwinrun%s' % ('o' if optimized else '')
    else:
        popen_arg = 'exec make qemutcprun%s' % ('o' if optimized else '')
    handle = Popen(popen_arg, shell=True, stdout=PIPE,
                   stdin=PIPE, stderr=PIPE, preexec_fn=os.setsid)  # , env=os.environ.copy())
    i = 0
    lines = ''
    while True:
        line = handle.stderr.readline().decode('utf-8')
        lines += line if line != '' else ''
        if 'QEMU waiting' in line:
            return handle
        elif 'failed' in line or 'Failed' in line:
            handle.kill()
            raise ConnectionError(lines + '\n\r' + handle.stderr.read().decode('utf-8'))
        i += 1
        if i > 20:
            handle.kill()
            raise ConnectionAbortedError(lines + handle.stderr.read().decode('utf-8'))


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


def get_until_cr(sock, limit=None, may_send_cr=False):
    received = ''
    i = 0
    while limit is None or i < limit:
        d = sock.recv(1)
        if not d or (not may_send_cr and d.decode('ascii') == '\r'):
            break
        received += d.decode('ascii')
        if 'ENDPROG' in received:
            received = received.split('ENDPROG')[0]
            break
    return received if len(received) > 0 else None


def kill_qemu_with_fire(handle):
    os.killpg(os.getpgid(handle.pid), signal.SIGTERM)


def train_interface(timeout, te_data, prog):
    qemu_handle = call_qemu_tcp(False)
    q = Queue()
    # time.sleep(1)
    try:
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.settimeout(timeout)
        handle_signal = lambda sign, frame: kill_qemu_with_fire(qemu_handle)
        signal.signal(signal.SIGTERM, handle_signal)
        signal.signal(signal.SIGINT, handle_signal)
        s.connect(te_sv_addr)
        junk = get_until_cr(s, 1000)
        if junk is None:
            kill_qemu_with_fire(qemu_handle)
            s.close()
            return None
        else:
            s.sendall(bytes('%s\r' % prog, 'ascii'))
            s.sendall(bytes(te_data, 'ascii'))
            prog_output = get_until_cr(s, may_send_cr=True)
            print(prog_output)
            print('aaaaaa')
            s.sendall(bytes('q\r', 'ascii'))
            get_until_cr(s, 1000)
            kill_qemu_with_fire(qemu_handle)
            s.close()
            print('really return 2')
            return prog_output
    except(ConnectionAbortedError, ConnectionError) as e:
        raise e
    except:
        kill_qemu_with_fire(qemu_handle)
        print('finally')
        return None
    '''tr_sv_o = ThreadingTCPServer(tr_sv_addr, TwoWayHandler((q.put, False, bytes(tr_data + 'ENDOFTRANSMISSION', 'ascii'))))
    te_sv_o = ThreadingTCPServer(te_sv_addr, TwoWayHandler((q.put, True, bytes(te_data + 'ENDOFTRANSMISSION', 'ascii'))))
    #tr_sv_o.socket.setblocking(False)
    #te_sv_o.socket.setblocking(False)
    tr_sv_o.timeout = timeout
    te_sv_o.timeout = timeout
    t1 = threading.Thread(target=lambda: handle_twice(tr_sv_o))
    t2 = threading.Thread(target=lambda: handle_twice(te_sv_o))
    t1.start()
    t2.start()
    #tr_sv_o.handle_request()
    #te_sv_o.handle_request()
    #time.sleep(0.4)
    call_qemu_tcp(optimized)
    t1.join(timeout)
    t2.join(timeout)
    #tr_sv_o.socket.settimeout(timeout)
    #te_sv_o.socket.settimeout(timeout)
    print('test')
    val1 = q.get()
    val2 = q.get()
    train_data = val2[0] if val1[1] else val1[0]
    terminal_data = val1[0] if val1[1] else val2[0]
    train_data = train_data.decode('ascii') if not train_data is None else None
    terminal_data = terminal_data.decode('ascii') if not terminal_data is None else None
    return train_data, terminal_data'''


if __name__ == "__main__":
    r = train_interface(10, '', 'test')
    print('ret: ' + r)
