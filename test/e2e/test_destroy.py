import unittest
from qemu_tcp_wrapper import qemu_oneshot_test

TIMEOUT = 10

def split_output(s):
    return list(filter(lambda x: x != '', s.split('\n\r')))

def test_destroy(self, test_name):
    real_output = qemu_oneshot_test(test_name, '', TIMEOUT)
    real_lines = split_output(real_output)
    self.assertEqual(real_lines, ['Success!'])

class TestDestroy(unittest.TestCase):
    def test_destroy_many_tasks(self):
        test_destroy(self, 'destroy_many_tasks')

    def test_destroy_task_limit(self):
        test_destroy(self, 'destroy_task_limit')

    def test_destroy_nameserver(self):
        test_destroy(self, 'destroy_nameserver')
