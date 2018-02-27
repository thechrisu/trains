import unittest
from qemu_tcp_wrapper import qemu_oneshot_test

TIMEOUT = 10

class TestModel(unittest.TestCase):
    def test_constant_velocity_model(self):
        ret = qemu_oneshot_test('test_constant_velocity_model', '', TIMEOUT)
        self.assertEqual(ret, 'Success.\n\r')

    def test_stopping_distance_model(self):
        ret = qemu_oneshot_test('test_stopping_distance_model', '', TIMEOUT)
        self.assertEqual(ret, 'Success.\n\r')

     def test_stopping_time_model(self):
        ret = qemu_oneshot_test('test_stopping_time_model', '', TIMEOUT)
        self.assertEqual(ret, 'Success.\n\r')

    def test_distance_between_sensors(self):
        ret = qemu_oneshot_test('test_distance_between_sensors', '', TIMEOUT)
        self.assertEqual(ret, 'Success.\n\r')
