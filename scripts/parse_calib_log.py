from collections import defaultdict
from statistics import mean

import re
import sys

LINES_PER_BLOCK = 16


def print_default_value_dict(data, name):
    out = '  default_value ' + name + '[] = {\n\r'
    super_vals = defaultdict()
    def_array = []
    for train in data:
        for speed in data[train]:
            def_array.append(
                '    {.train = %d, .speed = %d, .value = %d}'
                % (train, speed, data[train][speed]))
        super_vals[speed].append(data[train][speed])
    def_array.append('    {.train = 1337, .speed = 1337, .value = 1337}')
    out += ',\n\r'.join(def_array) + '  };\n\r\n\r'
    out += '  uint32_t super_' + name + '[15] = {\n\r'
    for i in range(0, 15):
        out += repr(int(mean(super_vals[i]))) + ', '
    out += '\n\r  };\n\r\n\r'
    print(out)


def get_log_lines():
    assert(len(sys.argv) == 2)
    f = open(sys.argv[1])
    log_file = f.read()
    f.close()
    lines = log_file.split('\n\r')
    assert(len(lines) % LINES_PER_BLOCK == 0)
    return lines


def parse_block(lines):
    vals = []
    for l in lines:
        e = l.split('|')
        vals += (int(e[0]), int(e[1]))
    return vals


def parse_log_lines(lines):
    default_speeds = {}
    default_stopping_distances = {}
    default_stopping_times = {}

    for i in range(0, len(lines), LINES_PER_BLOCK):
        train = int(re.match('[0-9]+', lines[i]))
        assert(0 < train < 81)

        blk = parse_block(lines[i + 1:i + LINES_PER_BLOCK + 1])
        for speed, value in blk:
            assert(0 <= speed <= 14)
            if 'stopping distance data' in lines[i]:
                d = default_speeds
            elif 'stopping time data' in lines[i]:
                d = default_stopping_distances
            elif 'velocity calibration data' in lines[i]:
                d = default_stopping_times
            else:
                raise SyntaxError('Unable to get type in line: ' + lines[i])
            if train not in d:
                d[train] = {}
                d[train][speed] = value
    return default_speeds, default_stopping_distances, default_stopping_times


if __name__ == "__main__":
    lines = get_log_lines()
    sp, std, stt = parse_log_lines(lines)
    print_default_value_dict(sp, 'default_speeds')
    print_default_value_dict(sp, 'default_stopping_distances')
    print_default_value_dict(sp, 'default_stopping_times')
