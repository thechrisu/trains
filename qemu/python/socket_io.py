import os
import SocketServer

# windows: \\\\.\\Global\\ + path
# elsewhere: /dev/virtio-ports/ + path
HOST, PORT = 'localhost', 9999
SocketServer.TCPServer((HOST, PORT), handler)

def test_x():
    train_interface(received_train_byte, received_terminal_byte)
    send_train_bytes(x)
    send_terminal_bytes(x)


class TrainSimuRequestHandler(SocketServer.BaseRequestHandler):
    def __init__(self, real_handler):
        self.real_handler = real_handler
        
    def handle(self):
        self.real_handler(self.rfile.read())

def train_interface(received_train_bytes, received_terminal_bytes):
    tr_hd =TrainSimuRequestHandler(received_train_bytes)
    te_hd = TrainSimuRequestHandler(received_terminal_bytes)
    tr_sv = ThreadingTCPServer(('localhost', 9990), tr_hd)
    te_sv = ThreadingTCPServer(('localhost', 9991), te_hd)
    close_all = lambda x: tr_sv.server_close(); te_sv.server_close()
    return close_all

def received_train_byte():
    pass

def received_terminal_byte():
    pass

def send_terminal_byte():
    pass

def send_train_byte():
    pass

if __name__ == "__main__":
    SocketServer.ThreadingTCPServer()
    
    train_port = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
    train_port.connect('train.sock')
    terminal_port = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
    terminal_port.connect('terminal.sock')
