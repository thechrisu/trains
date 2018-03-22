import unittest
from qemu_tcp_wrapper import qemu_oneshot_test

TIMEOUT = 10

class TestRouteExecution(unittest.TestCase):
    def test_route_execution(self):
        ret = qemu_oneshot_test('route_execution', '', TIMEOUT, timer_interrupts_on = True)
        self.assertEqual(ret, 'Success.\n\r')
