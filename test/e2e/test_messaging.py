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

expected_messaging_sequence_output = 'Created task 3\n\r' + '\n\r'.join([
    'Received message from parent: Message to child',
    'Received reply from child: Reply to parent',
    'Sent reply to parent: Reply to parent',
    'Received message from child: Message to parent',
    'Sent reply to child: Reply to child',
    'Received reply from parent: Reply to child',
] * 4) + '\n\r'


def test_messaging(self, test_name, expected_output):
    real_output = qemu_oneshot_test(test_name, '', TIMEOUT)
    expected_lines = expected_output.split('\n\r')
    real_lines = real_output.split('\n\r')
    self.assertEqual(expected_lines, real_lines)
    for i, line in enumerate(expected_lines):
        self.assertEqual(line, real_lines[i])


class TestMessaging(unittest.TestCase):
    def test_messaging_basic(self):
        test_messaging(self, 'messaging_basic', expected_messaging_basic_output)

    def test_messaging_receive_before_send(self):
        test_messaging(self, 'messaging_receive_before_send', expected_messaging_receive_before_send_output)

    def test_messaging_sequence(self):
        test_messaging(self, 'messaging_sequence', expected_messaging_sequence_output)


if __name__ == "__main__":
    unittest.main()
