import unittest
from qemu_tcp_wrapper import qemu_oneshot_test

TIMEOUT = 10

class TestPrediction(unittest.TestCase):
    def test_stopping_location_prediction(self):
        ret = qemu_oneshot_test('stop_loc_prediction', '', TIMEOUT)
        self.assertEqual(ret, 'Success.\n\r')

    def test_collision_prediction(self):
        ret = qemu_oneshot_test('collision_prediction', '', TIMEOUT, timer_interrupts_on=True)
        self.assertEqual(ret, 'Success.\n\r')

    def test_next_switch(self):
        ret = qemu_oneshot_test('test_next_switch_prediction', '', TIMEOUT, timer_interrupts_on=True)
        self.assertEqual(ret, 'Success.\n\r')

if __name__ == "__main__":
    unittest.main()
