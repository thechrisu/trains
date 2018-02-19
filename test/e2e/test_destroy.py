import unittest
from qemu_tcp_wrapper import qemu_oneshot_test

def split_output(s):
    return list(filter(lambda x: x != '', s.split('\n\r')))

class TestDestroy(unittest.TestCase):
    def test_destroy_many_tasks(self):
        real_output = qemu_oneshot_test('destroy_many_tasks', '', TIMEOUT)
        real_lines = split_output(real_output)
        self.assertEqual(real_lines, ['Success!'])
