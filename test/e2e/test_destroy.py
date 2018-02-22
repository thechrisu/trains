import unittest
from qemu_tcp_wrapper import qemu_oneshot_test

TIMEOUT = 10

destroy_send_queue_output = \
    ['Parent exiting'] + \
    ['Child with tid {} exiting'.format(tid) for tid in range(67, 77)]

destroy_parent_tid_output = [
    'Parent - MyTid: 2',
    'Child - MyTid: 67, MyParentTid: 2',
    'Grandchild - MyTid: 4, MyParentTid: 67'
]

def split_output(s):
    return list(filter(lambda x: x != '', s.split('\n\r')))

def test_destroy_with_output(self, test_name, expected_lines):
    real_output = qemu_oneshot_test(test_name, '', TIMEOUT)
    real_lines = split_output(real_output)
    self.assertEqual(real_lines, expected_lines)

def test_destroy(self, test_name):
    test_destroy_with_output(self, test_name, ['Success!'])

class TestDestroy(unittest.TestCase):
    def test_destroy_many_tasks(self):
        test_destroy(self, 'destroy_many_tasks')

    def test_destroy_task_limit(self):
        test_destroy(self, 'destroy_task_limit')

    def test_destroy_nameserver(self):
        test_destroy(self, 'destroy_nameserver')

    def test_destroy_send_queue(self):
        test_destroy_with_output(self, 'destroy_send_queue', destroy_send_queue_output)

    def test_destroy_block_kill(self):
        test_destroy(self, 'destroy_block_kill')

    def test_destroy_parent_tid(self):
        test_destroy_with_output(self, 'destroy_parent_tid', destroy_parent_tid_output)
