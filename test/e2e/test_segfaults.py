import unittest
from qemu_tcp_wrapper import qemu_oneshot_test

TIMEOUT = 10

expected_undefined_output = [
    '\x1b[94m\x1b[5mReceived abort\x1b[25m\x1b[39m type: Undefined',
    'Processor mode: Undefined',
    'CPSR: 0x6000009b',
    'Offending address: 0x1000000'
]

class TestSegfaultTypes(unittest.TestCase):
    def test_undefined(self):
        terminal_output = qemu_oneshot_test('test_undefined_handler', '', TIMEOUT)
        lines = list(filter(lambda x: x != '', terminal_output.split('\n\r')))
        self.assertEqual(len(lines), len(expected_undefined_output))
        for i, line in enumerate(lines):
            self.assertEqual(line, expected_undefined_output[i])
