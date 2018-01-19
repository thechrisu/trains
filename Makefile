.PHONY: default x64stdlib arm versatilepb trainslab labdebug upload test qemu docs
default: upload;

OPTIMIZATION = -O0
#-DCONTEXT_SWITCH_DEBUG -DSCHEDULE_DEBUG -DTRAPFRAME_DEBUG
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

TOOLPATH = $(current_dir)gcc-arm-none-eabi-7-2017-q4-major/bin/arm-none-eabi-
LABPATH = /u/wbcowan/gnuarm-4.0.2/bin/arm-elf-
.PRECIOUS: $(builddir)/main.s $(builddirversatilepb)/src/a0terminal.s $(builddirversatilepb)/include/kernel/glue/myio.s $(builddirversatilepb)/main.s $(builddirversatilepb)/src/multitasking/task.s $(builddirversatilepb)/usr/tasks.s $(builddirversatilepb)/src/interrupt.s $(builddirlab)/usr/tasks.s $(builddirlab)/src/interrupt.s $(builddirlab)/main.s %.s

XCC	= arm-none-eabi-gcc
AS	= arm-none-eabi-as
LD	= arm-none-eabi-ld
OBJCOPY = arm-none-eabi-objcopy

# Detect if in Windows Subsystem for Linux
ifeq (,$(wildcard /proc/version))
QEMU = qemu-system-arm
else
QEMU = qemu-system-arm.exe
endif

QEMUARGS = -M versatilepb -m 32M -kernel $(builddirversatilepb)/main.bin -semihosting
QEMUGUIARGS = $(QEMUARGS) -serial vc -serial vc -d guest_errors
QEMUCONSOLEARGS = $(QEMUARGS) -serial null -serial stdio

QEMUTESTINGBASEARGS = -M versatilepb -m 32M -kernel $(builddirtesting)/main.bin -semihosting
QEMUTESTINGGUIARGS = $(QEMUTESTINGBASEARGS) -serial vc -serial vc
QEMUTCPARGS = $(QEMUTESTINGBASEARGS) -nographic -serial null -serial tcp:127.0.0.1:9991,server

CFLAGSBASE = -c -fPIC -Wall -Wextra -std=c99 -msoft-float -Isrc -Itest-resources -Iusr -Iinclude/common -Iinclude/kernel/glue -fno-builtin
CFLAGS_ARM_LAB  = $(CFLAGSBASE) -mcpu=arm920t $(OPTIMIZATION) $(DEBUGFLAGS)
CFLAGS_x64 = $(CFLAGSBASE) -DHOSTCONFIG
CFLAGS_versatilepb = $(CFLAGSBASE) -DVERSATILEPB -mcpu=arm920t -g -nostdlib $(OPTIMIZATION) $(DEBUGFLAGS)
CFLAGS_versatilepb_e2e = $(CFLAGSBASE) -DVERSATILEPB -DE2ETESTING -mcpu=arm920t -g -nostdlib $(OPTIMIZATION) $(DEBUGFLAGS)
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
LDFLAGSversatilepb_e2e = -init main -Map=$(builddirtesting)/main.map -N -T versatilepb_e2e.ld \
	-L$(armlibs) -nostartfiles # SET THIS ENV VAR
LDFLAGSlab = -init main -Map=$(builddirlab)/main.map -N -T main.ld \
	-L/u/wbcowan/gnuarm-4.0.2/lib/gcc/arm-elf/4.0.2
#- ../gcc-arm-none-eabi-7-2017-q4-major/bin/arm-none-eabi-objcopy -O binary test.elf test.bin

SOURCESx64=main.c $(shell find src -name '*.c' -not -name 'cp_vec.c') $(shell find test-resources -name '*.c') \
                  $(shell find include/kernel/glue -name '*.c') $(shell find usr -name '*.c') \
                  $(shell find include/common -name '*.c')
SOURCES=$(SOURCESx64) $(shell find include/kernel/labenv -name '*.c')
SOURCESversatilepb=$(SOURCESx64) $(shell find include/kernel/versatilepb -name '*.c') src/cp_vec.c

ASM=$(shell find src -name '*.s' -not -name 'startup.s')
ASMversatilepb=$(shell find src -name '*.s')

OBJECTS=$(patsubst %.c, $(builddir)/%.o, $(SOURCES)) $(patsubst %.s, $(builddir)/%.o, $(ASM))
OBJECTSversatilepb=$(patsubst %.c, $(builddirversatilepb)/%.o, $(SOURCESversatilepb)) $(patsubst %.s, $(builddirversatilepb)/%.o, $(ASMversatilepb))
OBJECTSversatilepb_e2e=$(patsubst %.c, $(builddirtesting)/%.o, $(SOURCESversatilepb)) $(patsubst %.s, $(builddirtesting)/%.o, $(ASMversatilepb))
OBJECTSlab=$(patsubst %.c, $(builddirlab)/%.o, $(SOURCES)) $(patsubst %.s, $(builddirlab)/%.o, $(ASM))
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
	$(LABPATH)gcc $(CFLAGS_ARM_LAB)  $< -S -o $@

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
	-cd test && make alltests || cd ..

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


e2etest:
	-mkdir -p build
	-mkdir -p $(builddirtesting)
	-make $(builddirtesting)/main.bin

$(builddirtesting)/%.s: %.c
	@mkdir -p $(dir $@)
	$(XCC) $(CFLAGS_versatilepb_e2e)  $< -S -o $@

$(builddirtesting)/src/%.o: src/%.s
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
	-make arm
	-make versatilepb
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
	-cp $(builddirlab)/main.elf /u/cs452/tftp/ARM/$(shell whoami)/

qemu:
	-make versatilepb
	-$(QEMU) $(QEMUGUIARGS)

qemuconsole: versatilepb
	-$(QEMU) $(QEMUCONSOLEARGS)

qemutesting: e2etest
	-$(QEMU) $(QEMUTESTINGGUIARGS)

qemutcprun: e2etest
	- $(QEMU) $(QEMUTCPARGS)

docs:
	-doxygen Doxyfile
