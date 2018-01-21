import unittest
from qemu_tcp_wrapper import qemu_oneshot_test

TIMEOUT = 10

expected_messaging_basic_output = 'Created task 3\n\r' \
                                  'Received message: Sending a message!\n\r' \
                                  'Message length: 19\n\r' \
                                  'Received reply: Got your message!\n\r' \
                                  'Reply length: 18\n\r' \
                                  'Sent reply: Got your message!\n\r'

expected_messaging_receive_before_send_output = 'Created task 3\n\r' \
                                                'Received message: Sending a message!\n\r' \
                                                'Message length: 19\n\r' \
                                                'Sent reply: Got your message!\n\r' \
                                                'Received reply: Got your message!\n\r' \
                                                'Reply length: 18\n\r'


class TestMessaging(unittest.TestCase):
    def test_messaging_basic(self):
        messaging_basic_output = qemu_oneshot_test('messaging_basic', '', TIMEOUT)
        expected_lines = expected_messaging_basic_output.split('\n\r')
        real_lines = messaging_basic_output.split('\n\r')
        self.assertEqual(expected_lines, real_lines)
        for i, line in enumerate(expected_lines):
            self.assertEqual(line, real_lines[i])

    def test_messaging_receive_before_send(self):
        output = qemu_oneshot_test('messaging_receive_before_send', '', TIMEOUT)
        expected_lines = expected_messaging_receive_before_send_output.split('\n\r')
        real_lines = output.split('\n\r')
        self.assertEqual(expected_lines, real_lines)
        for i, line in enumerate(expected_lines):
            self.assertEqual(line, real_lines[i])


if __name__ == "__main__":
    unittest.main()
