#!/bin/bash

qemu-system-arm -M versatilepb -m 32M -kernel test.bin -serial vc -serial vc
