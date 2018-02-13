import re
import unittest
from qemu_tcp_wrapper import qemu_oneshot_test

TIMEOUT = 10

def read_snapshot(file_name):
    return re.sub(r'\r?\n', '\n\r', open('e2e/snapshots/' + file_name, 'r').read())

expected_lines_printf_errors = [
    'Terminal transmit server not yet up: -1',
    'Printing too long of a string: -2',
]

expected_lines_printf_happy_path = read_snapshot('printf_happy_path.txt')

class TestPrintf(unittest.TestCase):
    def test_printf_errors(self):
        real_output = qemu_oneshot_test('printf_errors', '', TIMEOUT, timer_interrupts_on = True, iointerrupts_on = True)
        real_lines = list(filter(lambda x: x != '', real_output.split('\n\r')))
        self.assertEqual(real_lines, expected_lines_printf_errors)

    def test_printf_happy_path(self):
        real_output = qemu_oneshot_test('printf_happy_path', '', TIMEOUT, timer_interrupts_on = True, iointerrupts_on = True)
        real_lines = list(filter(lambda x: x != '', real_output.split('\n\r')))
        self.assertEqual(real_lines, expected_lines_printf_happy_path)
