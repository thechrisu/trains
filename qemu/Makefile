all:
	- arm-none-eabi-as -mcpu=arm926ej-s -g startup.s -o startup.o
	- arm-none-eabi-gcc -c -mcpu=arm926ej-s -g cp_vec.c -o cp_vec.o
	- arm-none-eabi-gcc -c -mcpu=arm926ej-s -g test.c -o test.o
	- arm-none-eabi-ld -T test.ld test.o startup.o cp_vec.o -o test.elf
	- arm-none-eabi-objcopy -O binary test.elf test.bin
	- qemu-system-arm -M versatilepb -m 32M -nographic -kernel test.bin -serial tcp,127.0.0.1:9990 -serial tcp,127.0.0.1:9991

# launch with qemu-system-arm -M versatilepb -m 128M -nographic -kernel test.bin
#qemu-system-arm -M versatilepb -m 32M -kernel test.bin -serial vc -serial vc mon:stdio
# qemu-system-arm -M versatilepb -m 32M -kernel test.bin -chardev tty,id=pts2,path=/dev/pts/2 -device isa-serial,chardev=uart0 -nographic

#qemu-system-arm -M versatilepb -m 128M -nographic -kernel test.bin -serial chardev:train -serial chardev:train -serial chardev:terminal -chardev socket,id=train,path=train.sock -chardev socket,id=terminal,path=terminal.sock
