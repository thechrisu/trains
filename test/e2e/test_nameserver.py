import unittest
from qemu_tcp_wrapper import qemu_oneshot_test


expected_errorcodes_output = [
    'Truncated: %d' % (ord('T') + 128),
    'Too short(0): %d' % (ord('S') + 128),
    'Too short(1): %d' % (ord('S') + 128),
    'Wrong: %d' % (ord('W') + 128),
    'Not found: %d' % (ord('N') + 128)
]

expected_too_many = [
    'fake_RegisterAs before too many: %d' % ord('C'),
    'fake_RegisterAs one too many: %d' % (ord('M') + 128)
]

class TestNameserver(unittest.TestCase):
    def test_errorcodes(self):
        terminal_output = qemu_oneshot_test('test_nameserver_errors', '', 10)
        lines = list(filter(lambda x: x != '', terminal_output.split('\n\r')))
        self.assertEqual(len(lines), len(expected_errorcodes_output))
        for i, exp in enumerate(expected_errorcodes_output):
            self.assertEqual(lines[i], exp)

    def test_too_many(self):
        terminal_output = qemu_oneshot_test('test_nameserver_too_many', '', 10)
        lines = list(filter(lambda x: x != '', terminal_output.split('\n\r')))
        self.assertEqual(len(lines), len(expected_too_many))
        for i, exp in enumerate(expected_too_many):
            self.assertEqual(lines[i], exp)

    def test_happypath(self):
        pass

if __name__ == "__main__":
    unittest.main()
