import unittest
from qemu_tcp_wrapper import qemu_oneshot_test

TIMEOUT = 10

expected_lines_printf_errors = [
    'Terminal transmit server not yet up: -1',
    'Printing too long of a string: -2',
]

class TestPrintf(unittest.TestCase):
    def test_printf_errors(self):
        real_output = qemu_oneshot_test('printf_errors', '', TIMEOUT, timer_interrupts_on = True, iointerrupts_on = True)
        real_lines = list(filter(lambda x: x != '', real_output.split('\n\r')))
        self.assertEqual(real_lines, expected_lines_printf_errors)
