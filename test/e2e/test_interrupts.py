import unittest
from qemu_tcp_wrapper import qemu_oneshot_test

TIMEOUT = 10

class TestInterrupts(unittest.TestCase):
    def test_timer_interrupt(self):
        ret = real_output = qemu_oneshot_test('test_timer_interrupt', '', TIMEOUT, timer_interrupts_on=True)
        real_lines = list(filter(lambda x: x != '', real_output.split('\n\r')))
        expected_lines = ['TimerInterrupt return code: 0'] * 5
        self.assertEqual(len(real_lines), len(expected_lines))
        for r, e in zip(real_lines, expected_lines):
            self.assertEqual(r, e)

    def test_rx_terminal(self):
        ret = real_output = qemu_oneshot_test('test_rx_terminal', 'ddddd', TIMEOUT, iointerrupts_on=True)
        real_lines = list(filter(lambda x: x != '', real_output.split('\n\r')))
        expected_lines = ['TerminalRxInterrupt return code: 0'] * 5
        self.assertEqual(len(real_lines), len(expected_lines))
        for r, e in zip(real_lines, expected_lines):
            self.assertEqual(r, e)
