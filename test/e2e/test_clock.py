import unittest
from qemu_tcp_wrapper import qemu_oneshot_test

TIMEOUT = 10

expected_clock_errors_output = [
    'Creating nameserver',
    'Creating clock server',
    'Getting time',
    'Ticks at start: 0',
    'Response type on negative delay: 5',
    'Response type on zero delay: 5',
    'Response type on delay to time in past/present: 5'
]

def test(self, test_name, expected_lines):
    real_output = qemu_oneshot_test(test_name, '', TIMEOUT, timer_interrupts_on = True)
    real_lines = list(filter(lambda x: x != '', real_output.split('\n\r')))
    self.assertEqual(expected_lines, real_lines)


class TestMessaging(unittest.TestCase):
    def test_clock_errors(self):
        test(self, 'clock_errors', expected_clock_errors_output)

    def test_clock_accuracy(self):
        qemu_oneshot_test('clock_errors', '', TIMEOUT, timer_interrupts_on = True)


if __name__ == "__main__":
    unittest.main()
