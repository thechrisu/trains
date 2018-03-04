import unittest
from qemu_tcp_wrapper import qemu_oneshot_test

TIMEOUT = 10

class TestRouter(unittest.TestCase):
    def test_router_basic(self):
        ret = qemu_oneshot_test('router_basic', '', TIMEOUT, timer_interrupts_on = True)
        self.assertEqual(ret, 'Success.\n\r')

    def test_router_sensor_pair(self):
        ret = qemu_oneshot_test('router_sensor_pair', '', TIMEOUT, timer_interrupts_on = True)
        self.assertEqual(ret, 'Success.\n\r')

    def test_router_errors(self):
        ret = qemu_oneshot_test('router_errors', '', TIMEOUT, timer_interrupts_on = True)
        self.assertEqual(ret, 'Success.\n\r')

    def test_router_same_sensor(self):
        ret = qemu_oneshot_test('router_same_sensor', '', TIMEOUT, timer_interrupts_on = True)
        self.assertEqual(ret, 'Success.\n\r')
