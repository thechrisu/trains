import unittest
from qemu_tcp_wrapper import qemu_oneshot_test

TIMEOUT = 10

class TestSleeper(unittest.TestCase):
    def test_sleeper(self):
        ret = qemu_oneshot_test('sleeper', '', TIMEOUT, timer_interrupts_on = True)
        self.assertEqual(ret, 'Success.\n\r')
