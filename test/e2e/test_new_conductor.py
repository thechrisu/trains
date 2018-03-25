
import unittest
from qemu_tcp_wrapper import qemu_oneshot_test

TIMEOUT = 10

class TestConductor(unittest.TestCase):
    def test_conductor(self):
        self.AssertEqual(1, 0)

if __name__ == "__main__":
    unittest.main()
