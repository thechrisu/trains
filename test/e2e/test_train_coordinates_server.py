import unittest
from qemu_tcp_wrapper import qemu_oneshot_test

TIMEOUT = 10

class TestTrainCoordinatesServer(unittest.TestCase):
    def test_train_coordinates_server(self):
        ret = qemu_oneshot_test('train_coordinates_server', '', TIMEOUT, timer_interrupts_on = True)
        self.assertEqual(ret, 'Success.\n\r')
