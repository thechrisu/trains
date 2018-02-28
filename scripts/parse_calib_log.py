from collections import defaultdict
from statistics import mean

import re
import sys

LINES_PER_BLOCK = 17


def print_default_value_dict(data, name):
    out = '  default_value ' + name + '[] = {\n'
    super_vals = defaultdict(list)
    def_array = []
    for train in data:
        for speed in data[train]:
            def_array.append(
                '    { .train = %d, .speed = %d, .value = %d }'
                % (train, speed, data[train][speed]))
            if speed == 0 or data[train][speed] != 0:
              super_vals[speed].append(data[train][speed])
    def_array.append('    { .train = 1337, .speed = 1337, .value = 1337 }')
    out += ',\n'.join(def_array) + '\n  };\n\n'
    out += '  uint32_t super_' + name + '[15] = {\n    '
    for i in range(0, 15):
        out += repr(int(mean(super_vals[i]))) + ', '
    out += '\n  };\n'
    print(out)


def get_log_lines():
    assert(len(sys.argv) == 2)
    f = open(sys.argv[1])
    log_file = f.read()
    f.close()
    lines = log_file.split('\n')
    assert(len(lines) % LINES_PER_BLOCK == 0)
    return lines


def parse_block(lines):
    vals = []
    for l in lines:
        e = l.split('|')
        vals.append((int(e[0]), int(e[1])))
    return vals

def app_if_not_key(d, t, s, v):
  if t not in d:
    d[t] = {}
  d[t][s] = v

def parse_log_lines(lines):
    default_speeds = {}
    default_stopping_distances = {}
    default_stopping_times = {}

    for i in range(0, len(lines), LINES_PER_BLOCK):
        train = int(lines[i].split(' ')[1])
        assert(0 < train < 81)

        blk = parse_block(lines[i + 2:i + LINES_PER_BLOCK])
        for speed, value in blk:
            assert(0 <= speed <= 14)
            if 'velocity calibration data' in lines[i]:
                app_if_not_key(default_speeds, train, speed, value)
            elif 'stopping distance data' in lines[i]:
                app_if_not_key(default_stopping_distances, train, speed, value)
            elif 'stopping time data' in lines[i]:
                app_if_not_key(default_stopping_times, train, speed, value)
            else:
                raise SyntaxError('Unable to get type in line: ' + lines[i])
    return default_speeds, default_stopping_distances, default_stopping_times


if __name__ == "__main__":
    lines = get_log_lines()
    sp, std, stt = parse_log_lines(lines)
    print_default_value_dict(sp, 'default_speeds')
    print_default_value_dict(std, 'default_stopping_distances')
    print_default_value_dict(stt, 'default_stopping_times')
