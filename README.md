# trains
TODO include gtest install /usr/local/include/gtest (GTEST env var)
TODO include /u/wbcowan/gnuarm-4.0.2/lib/gcc/arm-elf/4.0.2 (armlibs env var)


## Instructions
1. I'm sorry, but this currently only works on my machine.
2. Install googletest (1. Clone via git submodules, 2. Use google to find out how to install googletest)
3. Stare at the Makefile
4. buildlab is for builds on the UW servers, build is for local ARM builds, buildx64 is for x86-x64 local builds

# Wishlist
1. QEMU
2. Assertions that really work and give nice output
3. Automated HiL Unit Testing
4. Profiling
5. Valgrind
6. Virtual Memory
(7. GDB -B Cowan has said people did not find it useful)
(8. SiL Testing in CI)

Gotchas:
1. Always. Document. (Need to hand in documentation after we're done with the kernel)
2. Get QEMU running ASAP
3. Get googletest running on Travis ASAP
4. Read algos/DS in kernel spec!
5. Read the RTOS books
6. For T1, set up dynamic calibration (talked about in T2 description)
7. Think about project proposal early

Kernel
1. "Scheduler"
2. "Message passing"
3. "Interrupts + clock server"
4. "Interrupt-driven IO + a0 + loose ends"

Train Control
1. "Stop train anywhere reliably" -> Stop reliably is more important than having a large set of possible destinations
2. "Improve stopping precision, detect single switch/sensor error"
