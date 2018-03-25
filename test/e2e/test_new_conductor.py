
import unittest
from qemu_tcp_wrapper import qemu_oneshot_test

TIMEOUT = 10

class TestConductor(unittest.TestCase):
    def test_conductor(self):
        ret = qemu_oneshot_test('coordinate_courier', '', TIMEOUT, timer_interrupts_on = True)
        self.assertEqual(ret, 'Success.\n\r')

if __name__ == "__main__":
    unittest.main()
