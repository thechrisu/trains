import unittest
from qemu_tcp_wrapper import qemu_oneshot_test

TIMEOUT = 10

class TestModel(unittest.TestCase):
    def test_constant_velocity_model(self):
        ret = qemu_oneshot_test('test_constant_velocity_model', '', TIMEOUT)
        self.assertEqual(ret, 'Success.\n\r')
