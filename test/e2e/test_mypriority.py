import unittest
from qemu_tcp_wrapper import qemu_oneshot_test

TIMEOUT = 10  # in seconds

exp_mypriority = 'Task should have priority 20, and is 20'

class TestMyPriority(unittest.TestCase):
    def test_mypriority_happypath(self):
        ret = qemu_oneshot_test('test_mypriority', '', TIMEOUT)
        l = ret.split('\n\r')[0]
        self.assertEqual(l, exp_mypriority)

if __name__ == "__main__":
    unittest.main()
