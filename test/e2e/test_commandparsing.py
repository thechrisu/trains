import unittest
from qemu_tcp_wrapper import qemu_oneshot_test

TIMEOUT = 10

def cmd_line(r, c):
    return '\033[' + str(r) + ';' + str(c) + 'H'

hide_cursor = '\033[?25l'
show_cursor = '\033[?25h'
hide_cursor_line = '\033[K'
preamble = hide_cursor + cmd_line(2, 1)
kexit_msg = cmd_line(12, 1) + 'Bye.\n\r\n\r'

class TestCommandParsing(unittest.TestCase):
    def test_quit(self):
        self.assertEqual(qemu_oneshot_test('k4', 'q\r', TIMEOUT, timer_interrupts_on=True, iointerrupts_on=True), cmd_line(12, 2) + 'q' + hide_cursor_line + hide_cursor + kexit_msg)

        # Cursor addressing is too much of a mess :(
    '''    def test_go_happypath(self):
        self.assertEqual(qemu_oneshot_test('k4', 'go\rq\r', TIMEOUT, timer_interrupts_on=True, iointerrupts_on=True), preamble + 'GO' + hide_cursor_line + kexit_msg)'''

if __name__ == "__main__":
    unittest.main()