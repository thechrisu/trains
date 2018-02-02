import unittest
from qemu_tcp_wrapper import qemu_oneshot_test

TIMEOUT = 10  # in seconds


class TestTestProgram(unittest.TestCase):
    def test_can_communicate_with_qemu_wrapper(self):
        ret = qemu_oneshot_test('test', '', TIMEOUT)  # no data, send 'test'
        self.assertEqual(ret, 'TestReturn\n\r')

    def test_unknown_test_program(self):
        self.assertEqual(qemu_oneshot_test('', '', TIMEOUT), 'Unknown test program ""')

if __name__ == "__main__":
    unittest.main()
