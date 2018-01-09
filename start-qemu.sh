#!/bin/bash

# COM1 train, COM2 display

qemu-system-arm -chardev tty,id=pts1,path=/dev/AMA1 id=COM1id -device isa-serial,chardev=pts1 id=COM1id -chardev BACKEND,OPTS, id=COM2id -device FRONEND,OPTS, id=COM2id -m 32M -nographic -kernel bild/main.bin
# todo timer devi
