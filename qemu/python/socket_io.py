from subprocess import Popen
import platform
import socketserver
from queue import Queue

tr_sv_addr = ('127.0.0.1', 9990)
te_sv_addr = ('127.0.0.1', 9991)


class TwoWayHandler(socketserver.StreamRequestHandler):
    def __init__(self, val):
        self.val = val

    def handle(self):
        self.val[0]((self.rfile.read(), self.val[1]))

    def __call__(self, req, addr, serv):
        socketserver.StreamRequestHandler.__init__(TwoWayHandler(self.val), req, addr, serv)


def call_qemu_tcp(optimised):
    if platform.system() == 'Microsoft' or platform.system() == 'Windows':
        Popen('cd .. && make qemutcpwinrun%s' % ('o' if optimised else ''))
    else:
        Popen('cd .. && make qemutcprun%s' % ('o' if optimised else ''))


def train_interface(timeout):
    q = Queue()
    tr_sv_o = socketserver.ThreadingTCPServer(tr_sv_addr, TwoWayHandler((q.put, False)))
    te_sv_o = socketserver.ThreadingTCPServer(te_sv_addr, TwoWayHandler((q.put, True)))
    tr_sv_o.timeout = timeout
    te_sv_o.timeout = timeout
    tr_sv_o.handle_request()
    te_sv_o.handle_request()
    val1 = q.get()
    val2 = q.get()
    terminal_data = val1[0] if val1[1] else val2[0]
    train_data = val2[0] if val1[1] else val1[0]
    return train_data, terminal_data
