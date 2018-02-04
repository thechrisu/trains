import re
import unittest
from qemu_tcp_wrapper import qemu_oneshot_test

def read_snapshot(file_name):
    return re.sub(r'\r?\n', '\n\r', open('e2e/snapshots/' + file_name, 'r').read())

TIMEOUT = 10

# Note: Task id 1 is the task runner
expected_k1_output = 'Created: 3.\n\r' \
                     'Created: 4.\n\r' \
                     'In other task: MyTid(): 5, MyParentTid(): 2\n\r' \
                     'In other task: MyTid(): 5, MyParentTid(): 2\n\r' \
                     'Created: 5.\n\r' \
                     'In other task: MyTid(): 6, MyParentTid(): 2\n\r' \
                     'In other task: MyTid(): 6, MyParentTid(): 2\n\r' \
                     'Created: 6.\n\r' \
                     'FirstUserTask: exiting\n\r' \
                     'In other task: MyTid(): 3, MyParentTid(): 2\n\r' \
                     'In other task: MyTid(): 4, MyParentTid(): 2\n\r' \
                     'In other task: MyTid(): 3, MyParentTid(): 2\n\r' \
                     'In other task: MyTid(): 4, MyParentTid(): 2\n\r'

expected_k2_output = read_snapshot('k2.txt')

expected_k3_output = read_snapshot('k3.txt')


class TestKernelDemoPrograms(unittest.TestCase):
    def test_k1(self):
        k1_output = qemu_oneshot_test('k1', '', TIMEOUT)
        expected_lines = expected_k1_output.split('\n\r')
        real_lines = k1_output.split('\n\r')
        self.assertEqual(expected_lines, real_lines)
        for i, line in enumerate(expected_lines):
            self.assertEqual(line, real_lines[i])

    def test_k2(self):
        k2_output = qemu_oneshot_test('k2', '', TIMEOUT)
        expected_lines = expected_k2_output.split('\n\r')
        real_lines = k2_output.split('\n\r')
        self.assertEqual(expected_lines, real_lines)

    def test_k3(self):
        k3_output = qemu_oneshot_test('k3', '', TIMEOUT, timer_interrupts_on = True)
        expected_lines = expected_k3_output.split('\n\r')
        real_lines = k3_output.split('\n\r')
        self.assertEqual(expected_lines, real_lines)


if __name__ == "__main__":
    unittest.main()
