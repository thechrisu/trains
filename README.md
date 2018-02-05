# Trains
## Supported Make targets
- `make docs` Build documentation. Required `doxygen` to be installed.
- `make versatilepb` Builds for the Versatile/PB platform, which we use for QEMU.
- `make e2etest` Builds for the Versatile/PB platform but with the test runner as the first user task.
- `make qemu` Does `make versatilepb` and starts the QEMU GUI.
- `make qemuconsole` Does `make versatilepb` and starts QEMU with terminal output showing in the console.
- `make qemutesting` Does `make e2etest` and starts QEMU with the test runner as the first user task and with terminal output showing in the gui.
- `make qemutestingconsole` Does `make e2etest` and starts QEMU with the test runner as the first user task and with terminal output showing in the console.
- `make qemutcprun` Does `make e2etest` and starts QEMU with terminal output sent over port 9991.
- `make arm` Compile the kernel with the train lab configuration on your machine.
- `make trainslab` Compile the kernel with the train lab configuration (needs access to Prof. Cowan's compiler in `/u/wbcowan`).
- `make upload` Cleans, compiles with the trains lab configuration and uploads it to be accessed in the lab. Run `TEST_RUNNER=true make upload` to build with the test runner as the first user task.
- `make test` Runs all Googletest unit tests.
- `make ci` Builds the docs, runs `make arm` and `make versatilepb`, and runs all the tests. Used by Travis CI.
- `make` Does the same as `make upload`.

## Compiler flags
Various debugging compiler flags can be passed (See `Makefile`).
In addition, you can the following environment variables to customize the build:
- `TEST_RUNNER=true` will run a program that spawns a number of other programs. We use this configuration in our end to end tests to dynamically decide which program to run. If you specify this option for builds, it will also enable timer interrupts.
- `TIMER_INTERRUPTS=true` will enable timer interrupts for build targets with `-DE2ETESTING` (_e.g._ `e2etest`, `qemutesting`, and `qemutestingconsole`). Without this variable, the target `e2etest` uses a simulated form of timer interrupts where a task instantly returns from a timer interrupt.
- `BWLOG=true` will change calls to `logprintf` to use busy-wait I/O instead of writing to a log. This is useful for debugging infinite loops.

## Environment variables
Some targets use environment variables to find their compiler/libraries.
### Googletest/Boost
`make test` requires Googletest and Boost to be installed.
- `GTEST` should point to the installed header files of Googletest (on our machines `/usr/local/include/gtest`).
- `LIBGTEST` should point to the Googletest library (on our machines `/usr/local/lib/libgtest.a`).

### ARM Compilation
To compile to ARM outside of the student environment, you need to
- Add a cross-compiler to your PATH (e.g. `/some/dir/gcc-arm-none-eabi-7-2017-q4-major/bin/arm-none-eabi-gcc`
- Set the environment variable `armlibs` to the libraries of the cross-compiler. We only need it for `libgcc.a`
  (e.g. `/some/dir/gcc-arm-none-eabi-7-2017-q4-major/lib/gcc/arm-none-eabi/7.2.1`).

## Assignment progress
Kernel
- [X] "Scheduler"
- [X] "Message passing"
- [X] "Interrupts + clock server"
- [ ] "Interrupt-driven IO + a0 + loose ends"

Train Control
- [ ] "Stop train anywhere reliably" -> Stopping reliably is more important than having a large set of possible destinations
- [ ] "Improve stopping precision, detect single switch/sensor error"
