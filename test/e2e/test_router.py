import unittest
from qemu_tcp_wrapper import qemu_oneshot_test

TIMEOUT = 10

class TestRouter(unittest.TestCase):
    def test_router_basic(self):
        ret = qemu_oneshot_test('test_router_basic', '', TIMEOUT, timer_interrupts_on = True)
        self.assertEqual(ret, 'Success.\n\r')
