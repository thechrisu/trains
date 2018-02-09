import unittest
from qemu_tcp_wrapper import qemu_oneshot_test

TIMEOUT = 10

expected_lines_getcputc_errors = [
    'Servers not yet up, Getc -1 == -1',
    'Servers not yet up, Putc -1 == -1',
]

class TestGetcPutc(unittest.TestCase):
    def test_getcputc_errors(self):
        ret = real_output = qemu_oneshot_test('test_getcputc_errors', '', TIMEOUT, iointerrupts_on=True)
        real_lines = list(filter(lambda x: x != '', real_output.split('\n\r')))
        self.assertEqual(len(real_lines), len(expected_lines_getcputc_errors))
        for r, e in zip(real_lines, expected_lines_getcputc_errors):
            self.assertEqual(r, e)
'''
    def test_getcputc_errors(self):
        ret = real_output = qemu_oneshot_test('test_getcputc_errors', '', TIMEOUT, iointerrupts_on=True)
        real_lines = list(filter(lambda x: x != '', real_output.split('\n\r')))
        self.assertEqual(len(real_lines), len(expected_lines_getcputc_errors))
        for r, e in zip(real_lines, expected_lines_getcputc_errors):
            self.assertEqual(r, e)
'''
