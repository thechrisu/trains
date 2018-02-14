import unittest

from qemu_tcp_wrapper import qemu_oneshot_test

expected_wrapper_errors_output = [
    'Truncated RegisterAs: -2',
    'Truncated WhoIs: -2',
    'Too short RegisterAs: -3',
    'Too short WhoIs: -3',
    'Not found WhoIs: -4',
]

expected_too_many = [
    'RegisterAs before too many: 0',
    'RegisterAs one too many: -4'
]

expected_happypath = [
    'Basic RegisterAs/WhoIs: Me: 2, Task in question: 2',
    'Other task registers: Them: 4, Task registered: 4',
    'Overwriting task: Them: 68, Task registered: 68',
    'I\'m 2 and registered Task1 and Task3. Nameserver says Task1 and Task3 are 2 and 2'
]


class TestNameserver(unittest.TestCase):
    def test_wrapper_errors(self):
        terminal_output = qemu_oneshot_test('test_nameserver_wrapper_errors', '', 10)
        lines = list(filter(lambda x: x != '', terminal_output.split('\n\r')))
        self.assertEqual(len(lines), len(expected_wrapper_errors_output))
        for i, exp in enumerate(expected_wrapper_errors_output):
            self.assertEqual(lines[i], exp)

    def test_too_many(self):
        terminal_output = qemu_oneshot_test('test_nameserver_too_many', '', 10)
        lines = list(filter(lambda x: x != '', terminal_output.split('\n\r')))
        self.assertEqual(len(lines), len(expected_too_many))
        for i, exp in enumerate(expected_too_many):
            self.assertEqual(lines[i], exp)

    def test_happypath(self):
        terminal_output = qemu_oneshot_test('test_nameserver_happypath', '', 10)
        lines = list(filter(lambda x: x != '', terminal_output.split('\n\r')))
        self.assertEqual(len(lines), len(expected_happypath))
        for i, exp in enumerate(expected_happypath):
            self.assertEqual(lines[i], exp)


if __name__ == "__main__":
    unittest.main()
