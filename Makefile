.PHONY: default ci arm versatilepb trainslab labdebug upload test qemu docs
default: upload;

OPTIMIZATION = -O2
#-DCONTEXT_SWITCH_DEBUG -DSCHEDULE_DEBUG -DTRAPFRAME_DEBUG -DMESSAGE_PASSING_DEBUG -DCONTEXT_SWITCH_BENCHMARK
DEBUGFLAGS=

# https://stackoverflow.com/questions/18136918/how-to-get-current-relative-directory-of-your-makefile
current_dir := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))
builddir = $(current_dir)build/arm
# default flags, same compiler
builddirx64 = $(current_dir)build/x8664
builddirlab = $(current_dir)build/lab
builddirversatilepb =build/versatilepb
builddirtesting =build/testing
#$(current_dir)build/versatilepb

LABPATH = /u/wbcowan/gnuarm-4.0.2/bin/arm-elf-
.SECONDARY:

XCC	= arm-elf-gcc
AS	= arm-elf-as
LD	= arm-elf-ld
OBJCOPY = arm-elf-objcopy

# Detect if in Windows Subsystem for Linux
ifeq ($(shell cat /proc/version | grep Microsoft || echo Linux),Linux)
QEMU = qemu-system-arm
else
QEMU = qemu-system-arm.exe
endif

ifeq ($(CI), true)
QEMU=qemu-system-arm
endif

ifeq (true,$(TEST_RUNNER))
TEST_RUNNER_FLAG = -DE2ETESTING
endif

QEMUARGS = -M versatilepb -m 32M -kernel $(builddirversatilepb)/main.bin -semihosting
QEMUGUIARGS = $(QEMUARGS) -serial vc -serial vc -d guest_errors
QEMUCONSOLEARGS = $(QEMUARGS) -serial null -serial stdio

QEMUTESTINGBASEARGS = -M versatilepb -m 32M -kernel $(builddirtesting)/main.bin -semihosting
QEMUTESTINGGUIARGS = $(QEMUTESTINGBASEARGS) -serial vc -serial vc
QEMUTESTINGARGS = $(QEMUTESTINGBASEARGS) -serial null -serial stdio
QEMUTCPARGS = $(QEMUTESTINGBASEARGS) -nographic -serial null -serial tcp:127.0.0.1:9991,server

#
CFLAGSBASE = -c -fPIC -Wall -Wextra -std=c99 -msoft-float -Ikernel/src -Ikernel/src/syscall -Ikernel/src/multitasking \
							-Itest-resources -Iusr -Iusr/test -Itest/messaging -Itest/nameserver -Ilib -Iinclude/ -fno-builtin
CFLAGS_ARM_LAB  = $(CFLAGSBASE) -mcpu=arm920t $(OPTIMIZATION) $(DEBUGFLAGS) $(TEST_RUNNER_FLAG)
CFLAGS_x64 = $(CFLAGSBASE) -DHOSTCONFIG
CFLAGS_versatilepb = $(CFLAGSBASE) -DVERSATILEPB -mcpu=arm920t -g -nostdlib $(OPTIMIZATION) $(DEBUGFLAGS)
CFLAGS_versatilepb_e2e = $(CFLAGSBASE) -DVERSATILEPB -DE2ETESTING -mcpu=arm920t -g -nostdlib $(OPTIMIZATION) $(DEBUGFLAGS)
# -c: only compile
# -fpic: emit position-independent code
# -msoft-float: use software for floating point


ASFLAGS	= -mcpu=arm920t -mapcs-32
ASFLAGS_versatilepb = -mcpu=arm920t -mapcs-32 -g
# -mapcs-32: always create a complete stack frame



LDFLAGSarm = -init main -Map=$(builddir)/main.map -N -T main.ld \
	-L$(armlibs) # SET THIS ENV VAR
LDFLAGSversatilepb = -init main -Map=$(builddirversatilepb)/main.map -N -T versatilepb.ld \
	-L$(armlibs) -nostartfiles # SET THIS ENV VAR
LDFLAGSversatilepb_e2e = -init main -Map=$(builddirtesting)/main.map -N -T versatilepb_e2e.ld \
	-L$(armlibs) -nostartfiles # SET THIS ENV VAR
LDFLAGSlab = -init main -Map=$(builddirlab)/main.map -N -T main.ld \
	-L/u/wbcowan/gnuarm-4.0.2/lib/gcc/arm-elf/4.0.2
#- ../gcc-arm-none-eabi-7-2017-q4-major/bin/arm-none-eabi-objcopy -O binary test.elf test.bin

SOURCESx64=main.c $(shell find kernel ! -path '*kernel/include/*' -name '*.c' -not -name 'cp_vec.c') $(shell find test-resources -name '*.c') \
                  $(shell find include -name '*.c') $(shell find usr -name '*.c') \
                  $(shell find lib -name '*.c')
SOURCES=$(SOURCESx64) $(shell find kernel/include/labenv -name '*.c')
SOURCESversatilepb=$(SOURCESx64) $(shell find kernel/include/versatilepb -name '*.c') kernel/src/cp_vec.c

ASM=$(shell find kernel -name '*.s' -not -name 'startup.s')
ASMversatilepb=$(shell find kernel -name '*.s')

OBJECTS=$(patsubst %.c, $(builddir)/%.o, $(SOURCES)) $(patsubst %.s, $(builddir)/%.o, $(ASM))
OBJECTSversatilepb=$(patsubst %.c, $(builddirversatilepb)/%.o, $(SOURCESversatilepb)) $(patsubst %.s, $(builddirversatilepb)/%.o, $(ASMversatilepb))
OBJECTSversatilepb_e2e=$(patsubst %.c, $(builddirtesting)/%.o, $(SOURCESversatilepb)) $(patsubst %.s, $(builddirtesting)/%.o, $(ASMversatilepb))
OBJECTSlab=$(patsubst %.c, $(builddirlab)/%.o, $(SOURCES)) $(patsubst %.s, $(builddirlab)/%.o, $(ASM))
OBJECTSx64=$(patsubst %.c, $(builddirx64)/%.o, $(SOURCESx64))

x64stdlib:
	mkdir -p build
	mkdir -p $(builddirx64)
	make $(builddirx64)/main

$(builddirx64)/%.s: %.c
	@mkdir -p $(dir $@)
	gcc $(CFLAGSx64) $< -S -o $@

$(builddirx64)/kernel/%.o: kernel/%.s
	@mkdir -p $(dir $@)
	as $< -o $@

$(builddirx64)/%.o: $(builddirx64)/%.s
	@mkdir -p $(dir $@)
	as $< -o $@

$(builddirx64)/main: $(OBJECTSx64)
	@mkdir -p $(dir $@)
	ld -arch x86_64 -o $@ $(OBJECTSx64) -lc



arm:
	mkdir -p build
	mkdir -p $(builddir)
	make $(builddir)/main.bin -j2

$(builddir)/%.s: %.c
	@mkdir -p $(dir $@)
	$(XCC) $(CFLAGS_ARM_LAB) $< -S -o $@

$(builddir)/kernel/%.o: kernel/%.s
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
	mkdir -p build
	mkdir -p $(builddirlab)
	make $(builddirlab)/main.bin -j2

$(builddirlab)/%.s: %.c
	@mkdir -p $(dir $@)
	$(LABPATH)gcc $(CFLAGS_ARM_LAB)  $< -S -o $@

$(builddirlab)/kernel/%.o: kernel/%.s
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
	cd test && make alltests
	cd ..

versatilepb:
	mkdir -p build
	mkdir -p $(builddirversatilepb)
	make $(builddirversatilepb)/main.bin -j2

$(builddirversatilepb)/%.s: %.c
	@mkdir -p $(dir $@)
	$(XCC) $(CFLAGS_versatilepb) $< -S -o $@

$(builddirversatilepb)/kernel/%.o: kernel/%.s
	@mkdir -p $(dir $@)
	$(AS) $(ASFLAGS_versatilepb) $< -o $@

$(builddirversatilepb)/%.o: $(builddirversatilepb)/%.s
	@mkdir -p $(dir $@)
	$(AS) $(ASFLAGS_versatilepb) $< -o $@

$(builddirversatilepb)/main.elf: $(OBJECTSversatilepb)
	$(LD) $(LDFLAGSversatilepb) -o $(builddirversatilepb)/main.elf $(OBJECTSversatilepb) -lgcc

$(builddirversatilepb)/main.bin: $(builddirversatilepb)/main.elf
	$(OBJCOPY) -O binary $(builddirversatilepb)/main.elf $(builddirversatilepb)/main.bin


e2etest:
	mkdir -p build
	mkdir -p $(builddirtesting)
	make $(builddirtesting)/main.bin -j2

$(builddirtesting)/%.s: %.c
	@mkdir -p $(dir $@)
	$(XCC) $(CFLAGS_versatilepb_e2e)  $< -S -o $@

$(builddirtesting)/kernel/%.o: kernel/%.s
	@mkdir -p $(dir $@)
	$(AS) $(ASFLAGS_versatilepb) $< -o $@

$(builddirtesting)/%.o: $(builddirtesting)/%.s
	@mkdir -p $(dir $@)
	$(AS) $(ASFLAGS_versatilepb) $< -o $@

$(builddirtesting)/main.elf: $(OBJECTSversatilepb_e2e)
	$(LD) $(LDFLAGSversatilepb_e2e) -o $(builddirtesting)/main.elf $(OBJECTSversatilepb_e2e) -lgcc

$(builddirtesting)/main.bin: $(builddirtesting)/main.elf
	$(OBJCOPY) -O binary $(builddirtesting)/main.elf $(builddirtesting)/main.bin

all:
	set -e
	make docs -j2
	make arm -j2
	make versatilepb -j2
	make e2etest -j2
	make test -j2

ci:
	set -e
	make docs -j2
	make arm -j2
	make versatilepb -j2
	make e2etest -j2
	set +e
	cd test && make all -j2
	cd ..
	set -e
	make test -j2

clean:
	rm -f *.s *.a *.o \
	  $(builddir)/main.map $(builddir)/main.elf $(builddir)/*.o
	rm -rf build/*
	find . -name ".#*" -print0 | xargs -0 rm -rf
	cd test && make clean && cd ..

upload:
	make clean
	make trainslab
	cp $(builddirlab)/main.elf /u/cs452/tftp/ARM/$(shell whoami)/

qemu:
	make versatilepb
	$(QEMU) $(QEMUGUIARGS)

qemuconsole: versatilepb
	$(QEMU) $(QEMUCONSOLEARGS)

qemutesting: e2etest
	$(QEMU) $(QEMUTESTINGGUIARGS)

qemutestingconsole: e2etest
	$(QEMU) $(QEMUTESTINGARGS)

qemutcprun: e2etest
	$(QEMU) $(QEMUTCPARGS)

docs:
	doxygen Doxyfile
