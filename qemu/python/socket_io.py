from subprocess import Popen, PIPE
import platform
import socketserver
import socket
from queue import Queue
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


def call_qemu_tcp(optimized):
    os.chdir(os.path.join(dir_path, '../..'))
    if platform.system() == 'Microsoft' or platform.system() == 'Windows':
        popen_arg = 'make qemutcpwinrun%s' % ('o' if optimized else '')
    else:
        popen_arg = 'make qemutcprun%s' % ('o' if optimized else '')
    a = Popen(popen_arg, shell=True, stdout=PIPE, stdin=PIPE, stderr=PIPE)  # , env=os.environ.copy())
    i = 0
    lines = ''
    while True:
        line = a.stderr.readline().decode('utf-8')
        lines += line if line != '' else ''
        if 'QEMU waiting' in line:
            return
        elif 'failed' in line:
            raise ConnectionError(line + '\n\r' + a.stderr.read().decode('utf-8'))
        i += 1
        if i > 20:
            raise ConnectionAbortedError(lines + a.stderr.read().decode('utf-8'))


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
    print('a')
    # s.sendall(bytes(data + 'ENDOFTRANSMISSION', 'ascii'))
    tr_recv = ''
    while (True):
        d = s.recv(10)
        if not d: break
        tr_recv += d.decode('ascii')
    queue.put(tr_recv)
    s.close()
    # tcp_lock.release()


def train_interface(timeout, tr_data, te_data, optimized, prog):
    call_qemu_tcp(False)
    q = Queue()
    # time.sleep(1)

    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    # t1 = threading.Thread(target=lambda: send_data_socket(s1, '', q, tr_sv_addr))

    s.connect(te_sv_addr)
    # s.sendall(bytes(data + 'ENDOFTRANSMISSION', 'ascii'))
    tr_recv = ''
    success = False
    for i in range(1, 1000):
        d = s.recv(1)
        if not d:
            break
        if d.decode('ascii') == '\r':
            success = True
            break
        tr_recv += d.decode('ascii')
    print(tr_recv)
    if success:
        s.sendall(bytes('%s\r' % prog, 'ascii'))
        #s.sendall(bytes('%s' % te_data, 'ascii'))
        print(s.recv(12).decode('ascii'))
        #q.put(tr_recv)
        s.close()
    else:
        s.close()

    #t2 = threading.Thread(target=lambda: send_data_socket(s2, te_data, q, te_sv_addr, prog))
    #t2.start()
    # t1.start()
    # t1.join()
    #t2.join()
    return tr_recv #q.get()  # , q.get()
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
    print(train_interface(10, "testtr", "testte", False, 'test'))
