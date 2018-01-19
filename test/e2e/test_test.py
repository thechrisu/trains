import unittest
from qemu_tcp_wrapper import train_interface

TIMEOUT = 10 #in seconds

class TestTestProgram(unittest.TestCase):
    def test_can_communicate_with_qemu_wrapper(self):
        ret = train_interface(TIMEOUT, '', 'test') # no data, send 'test'
        self.assertEqual(ret, 'TestReturn\n\r')

if __name__ == "__main__":
    unittest.main()
