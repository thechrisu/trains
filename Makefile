.PHONY: default x64stdlib arm versatilepb trainslab labdebug upload test qemu
default: upload;

OPTIMIZATION = -O0

# https://stackoverflow.com/questions/18136918/how-to-get-current-relative-directory-of-your-makefile
current_dir := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))
builddir = $(current_dir)build/arm
# default flags, same compiler
builddirx64 = $(current_dir)build/x8664
builddirlab = $(current_dir)build/lab
builddirversatilepb =build/versatilepb
#$(current_dir)build/versatilepb

TOOLPATH = $(current_dir)gcc-arm-none-eabi-7-2017-q4-major/bin/arm-none-eabi-
LABPATH = /u/wbcowan/gnuarm-4.0.2/bin/arm-elf-
.PRECIOUS: $(builddir)/main.s $(builddirversatilepb)/src/a0terminal.s $(builddirversatilepb)/include/kernel/glue/myio.s $(builddirversatilepb)/main.s $(builddirversatilepb)/src/cp_vec.s $(builddirversatilepb)/usr/tasks.s $(builddirversatilepb)/src/interrupt.s

XCC	= arm-none-eabi-gcc
AS	= arm-none-eabi-as
LD	= arm-none-eabi-ld
OBJCOPY = arm-none-eabi-objcopy

QEMU = qemu-system-arm
QEMUWIN = qemu-system-arm.exe
QEMUARGS = -M versatilepb -m 32M -kernel $(builddirversatilepb)/main.bin
QEMUGUIARGS = $(QEMUARGS) -serial vc -serial vc -d guest_errors
QEMUCONSOLEARGS = $(QEMUARGS) -serial null -serial stdio

CFLAGSBASE = -c -fPIC -Wall -Wextra -std=c99 -msoft-float -Isrc -Itest-resources -Iusr -Iinclude/kernel/glue -fno-builtin
CFLAGS_ARM_LAB  = $(CFLAGSBASE) -mcpu=arm920t $(OPTIMIZATION)
CFLAGS_x64 = $(CFLAGSBASE) -DHOSTCONFIG
CFLAGS_versatilepb = $(CFLAGSBASE) -DVERSATILEPB -mcpu=arm926ej-s -g -nostdlib
# -c: only compile
# -fpic: emit position-independent code
# -msoft-float: use software for floating point


ASFLAGS	= -mcpu=arm920t -mapcs-32
ASFLAGS_versatilepb = -mcpu=arm926ej-s -mapcs-32 -g
# -mapcs-32: always create a complete stack frame



LDFLAGSarm = -init main -Map=$(builddir)/main.map -N -T main.ld \
	-L$(armlibs) # SET THIS ENV VAR
LDFLAGSversatilepb = -init main -Map=$(builddirversatilepb)/main.map -N -T versatilepb.ld \
	-L$(armlibs) -nostartfiles # SET THIS ENV VAR
LDFLAGSlab = -init main -Map=$(builddirlab)/main.map -N -T main.ld \
	-L/u/wbcowan/gnuarm-4.0.2/lib/gcc/arm-elf/4.0.2
#- ../gcc-arm-none-eabi-7-2017-q4-major/bin/arm-none-eabi-objcopy -O binary test.elf test.bin

SOURCESx64=main.c $(shell find src -name '*.c') $(shell find test-resources -name '*.c') \
                  $(shell find include/kernel/glue -name '*.c') $(shell find usr -name '*.c')
SOURCES=$(SOURCESx64) $(shell find include/kernel/labenv -name '*.c')
SOURCESversatilepb=$(SOURCESx64) $(shell find include/kernel/versatilepb -name '*.c')

ASM=$(shell find src -name '*.s' -not -name 'startup.s')
ASMversatilepb=$(shell find src -name '*.s')

OBJECTS=$(patsubst %.c, $(builddir)/%.o, $(SOURCES)) $(patsubst %.s, $(builddir)/%.o, $(ASM))
OBJECTSversatilepb=$(patsubst %.c, $(builddirversatilepb)/%.o, $(SOURCESversatilepb)) $(patsubst %.s, $(builddirversatilepb)/%.o, $(ASMversatilepb))
OBJECTSlab=$(patsubst %.c, $(builddirlab)/%.o, $(SOURCES))
OBJECTSx64=$(patsubst %.c, $(builddirx64)/%.o, $(SOURCESx64))

x64stdlib:
	- mkdir -p build
	- mkdir -p $(builddirx64)
	- make $(builddirx64)/main

$(builddirx64)/%.s: %.c
	@mkdir -p $(dir $@)
	gcc $(CFLAGSx64) $< -S -o $@

$(builddirx64)/src/%.o: src/%.s
	@mkdir -p $(dir $@)
	as $< -o $@

$(builddirx64)/%.o: $(builddirx64)/%.s
	@mkdir -p $(dir $@)
	as $< -o $@

$(builddirx64)/main: $(OBJECTSx64)
	@mkdir -p $(dir $@)
	ld -arch x86_64 -o $@ $(OBJECTSx64) -lc



arm:
	-mkdir -p build
	-mkdir -p $(builddir)
	-make $(builddir)/main.bin

$(builddir)/%.s: %.c
	@mkdir -p $(dir $@)
	$(XCC) $(CFLAGS_ARM_LAB) $< -S -o $@

$(builddir)/src/%.o: src/%.s
	@mkdir -p $(dir $@)
	$(AS) $(ASFLAGS) $< -o $@

$(builddir)/%.o: $(builddir)/%.s
	@mkdir -p $(dir $@)
	$(AS) $(ASFLAGS) $< -o $@

$(builddir)/main.elf: $(OBJECTS)
	$(LD) $(LDFLAGSarm) -o $@ $(OBJECTS) -lgcc

$(builddir)/main.bin: $(builddir)/main.elf
	$(OBJCOPY) -O binary $(builddir)/main.elf $(builddir)/main.bin


trainslab:
	-mkdir -p build
	-mkdir -p $(builddirlab)
	-make $(builddirlab)/main.bin

$(builddirlab)/%.s: %.c
	@mkdir -p $(dir $@)
	$(LABPATH)gcc $(CFLAGS_ARM_LAB) -O2  $< -S -o $@

$(builddirlab)/src/%.o: src/%.s
	@mkdir -p $(dir $@)
	$(LABPATH)as $(ASFLAGS_ARM_LAB) $< -o $@

$(builddirlab)/%.o: $(builddirlab)/%.s
	@mkdir -p $(dir $@)
	$(LABPATH)as $(ASFLAGS_ARM_LAB) $< -o $@

$(builddirlab)/main.elf: $(OBJECTSlab)
	$(LABPATH)ld $(LDFLAGSlab) -o $@ $(OBJECTSlab) -lgcc

$(builddirlab)/main.bin: $(builddirlab)/main.elf
	$(LABPATH)objcopy -O binary $(builddirlab)/main.elf $(builddirlab)/main.bin

#bwio.s: bwio.c
#	$(XCC) -S $(CFLAGS) bwio.c
#bwio.o: bwio.s
#   $(AS) $(ASFLAGS) -o bwio.o bwio.s
#bwio.a: bwio.o
#	$(AR) $(ARFLAGS) $@ bwio.o

test:
	-cd test && make all && make test || cd ..

versatilepb:
	-mkdir -p build
	-mkdir -p $(builddirversatilepb)
	-make $(builddirversatilepb)/main.bin

$(builddirversatilepb)/%.s: %.c
	@mkdir -p $(dir $@)
	$(XCC) $(CFLAGS_versatilepb) -O0  $< -S -o $@

$(builddirversatilepb)/src/%.o: src/%.s
	@mkdir -p $(dir $@)
	$(AS) $(ASFLAGS_versatilepb) $< -o $@

$(builddirversatilepb)/%.o: $(builddirversatilepb)/%.s
	@mkdir -p $(dir $@)
	$(AS) $(ASFLAGS_versatilepb) $< -o $@

$(builddirversatilepb)/main.elf: $(OBJECTSversatilepb)
	$(LD) $(LDFLAGSversatilepb) -o $(builddirversatilepb)/main.elf $(OBJECTSversatilepb) -lgcc

$(builddirversatilepb)/main.bin: $(builddirversatilepb)/main.elf
	$(OBJCOPY) -O binary $(builddirversatilepb)/main.elf $(builddirversatilepb)/main.bin


all:
	-make arm
	-make x64stdlib
	-make test

ci:
	-make x64stdlib
	-make test

clean:
	-rm -f *.s *.a *.o \
	  $(builddir)/main.map $(builddir)/main.elf $(builddir)/*.o
	-rm -rf build/*
	-cd test && make clean && cd ..

upload:
	-make clean
	-make trainslab
	-cp $(builddirlab)/main.elf /u/cs452/tftp/ARM/csulshoe/

qemu:
	-make versatilepb
	-$(QEMU) $(QEMUGUIARGS)

qemuwin: versatilepb
	-$(QEMUWIN) $(QEMUGUIARGS)

qemuconsole: versatilepb
	-$(QEMU) $(QEMUCONSOLEARGS)

qemuwinconsole: versatilepb
	-$(QEMUWIN) $(QEMUCONSOLEARGS)
