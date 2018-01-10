.PHONY: default x64stdlib arm trainslab upload test
default: upload;

# https://stackoverflow.com/questions/18136918/how-to-get-current-relative-directory-of-your-makefile
current_dir := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))
builddir = $(current_dir)build
builddirx64 = $(current_dir)buildx64
builddirlab = $(current_dir)buildlab
TOOLPATH = $(current_dir)gcc-arm-none-eabi-7-2017-q4-major/bin/arm-none-eabi-
LABPATH = /u/wbcowan/gnuarm-4.0.2/bin/arm-elf-
.PRECIOUS: $(builddir)/main.s

XCC	= $(TOOLPATH)gcc
AS	= $(TOOLPATH)as
LD	= $(TOOLPATH)ld
OBJCOPY = $(TOOLPATH)objcopy
CFLAGS  = -c -fPIC -Wall \
	-Wextra -std=c99 \
	-msoft-float -Isrc -Itest-resources -mcpu=arm920t -O0

CFLAGSx64 = -c -fPIC -Wall -Wpedantic -Wextra -DHOSTCONFIG -msoft-float -std=c99
# -c: only compile
# -fpic: emit position-independent code
# -msoft-float: use software for floating point

ASFLAGS	= -mcpu=arm920t -mapcs-32
# -mapcs-32: always create a complete stack frame

LDFLAGS = -init main -Map=$(builddir)/main.map -N -T main.ld \
	-Lgcc-arm-none-eabi-7-2017-q4-major/lib/gcc/arm-none-eabi/7.2.1
LDFLAGSlab = -init main -Map=$(builddir)/main.map -N -T main.ld \
	-L/u/wbcowan/gnuarm-4.0.2/lib/gcc/arm-elf/4.0.2
# TODO add cowan files

SOURCESx64=main.c $(shell find src -name '*.c') $(shell find test-resources -name '*.c')
SOURCES=$(SOURCESx64) $(shell find include/labenv -name '*.c')
OBJECTS=$(patsubst %.c, $(builddir)/%.o, $(SOURCES)) # TODO add assembly files
OBJECTSlab=$(patsubst %.c, $(builddirlab)/%.o, $(SOURCES))
OBJECTSx64=$(patsubst %.c, $(builddirx64)/%.o, $(SOURCESx64))

x64stdlib: $(builddirx64)/main

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



arm: $(builddir)/main.bin

$(builddir)/%.s: %.c
	@mkdir -p $(dir $@)
	$(XCC) $(CFLAGS) $< -S -o $@

$(builddir)/src/%.o: src/%.s
	@mkdir -p $(dir $@)
	$(AS) $(ASFLAGS) $< -o $@

$(builddir)/%.o: $(builddir)/%.s
	@mkdir -p $(dir $@)
	$(AS) $(ASFLAGS) $< -o $@

$(builddir)/main.elf: $(OBJECTS)
	$(LD) $(LDFLAGS) -o $@ $(OBJECTS) -lgcc

$(builddir)/main.bin: $(builddir)/main.elf
	$(OBJCOPY) -O binary $(builddir)/main.elf $(builddir)/main.bin


trainslab: $(builddirlab)/main.bin

$(builddirlab)/%.s: %.c
	@mkdir -p $(dir $@)
	$(LABPATH)gcc $(CFLAGS) $< -S -o $@

$(builddirlab)/src/%.o: src/%.s
	@mkdir -p $(dir $@)
	$(LABPATH)as $(ASFLAGS) $< -o $@

$(builddirlab)/%.o: $(builddirlab)/%.s
	@mkdir -p $(dir $@)
	$(LABPATH)as $(ASFLAGS) $< -o $@

$(builddirlab)/main.elf: $(OBJECTSlab)
	$(LABPATH)ld $(LDFLAGSlab) -o $@ $(OBJECTSlab) -lgcc

$(builddirlab)/main.bin: $(builddirlab)/main.elf
	$(LABPATH)objcopy -O binary $(builddirlab)/main.elf $(builddirlab)/main.bin

#$(current_dir)/src/track/track_data.c:
#	$(current_dir)/src/track/parse_track tracka trackb

#$(current_dir)/src/track/track_data.h:
#	$(current_dir)/src/track/parse_track tracka trackb

#bwio.s: bwio.c
#	$(XCC) -S $(CFLAGS) bwio.c
#bwio.o: bwio.s
#   $(AS) $(ASFLAGS) -o bwio.o bwio.s
#bwio.a: bwio.o
#	$(AR) $(ARFLAGS) $@ bwio.o

test:
	-cd test && make all && make test || cd ..

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
	-rm -rf buildx64/*
	-rm -rf buildlab/*
	-cd test && make clean && cd ..

upload:
	-make clean
	-make trainslab
	-cp $(builddirlab)/main.elf /u/cs452/tftp/ARM/csulshoe/
