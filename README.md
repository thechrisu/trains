# Trains
##Supported Make targets
-`make versatilepb` Builds for the Versatile/PB platform, which we use for QEMU.
-`make qemu`/`make qemuwin` Does `make versatilepb` and starts the QEMU GUI.
-`make qemuconsole`/`make qemuwinconsole` Does `make versatilepb` and starts QEMU with terminal output showing in the console.
-`make arm` Compile the kernel with the train lab configuration on your machine.
-`make trainslab` Compile the kernel with the train lab configuration (needs access to Prof. Cowan's compiler in `/u/wbcowan`).
-`make upload` Cleans, compiles with the trains lab configuration and uploads it to be accessed in the lab.
-`make test` Runs all Googletest unit tests.
-`make` Does the same as `make upload`.

##Environment variables
Some targets use environment variables to find their compiler/libraries.
###Googletest/Boost
`make test` requires Googletest and Boost to be installed.
- `GTEST` should point to the installed header files of Googletest (on our machines `/usr/local/include/gtest`).
- `LIBGTEST` should point to the Googletest library (on our machines `/usr/local/lib/libgtest.a`).
- `BOOST` should point to the installed header files of Boost (on our machines `/usr/local/include/boost`).

###ARM Compilation
To compile to ARM outside of the student environment, you need to
- Add a cross-compiler to your PATH (e.g. `/some/dir/gcc-arm-none-eabi-7-2017-q4-major/bin/arm-none-eabi-gcc`
- Set the environment variable `armlibs` to the libraries of the cross-compiler. We only need it for `libgcc.a`
  (e.g. `/some/dir/gcc-arm-none-eabi-7-2017-q4-major/lib/gcc/arm-none-eabi/7.2.1`).

## Assignment progress
Kernel
- [X] "Scheduler"
- [ ] "Message passing"
- [ ] "Interrupts + clock server"
- [ ] "Interrupt-driven IO + a0 + loose ends"

Train Control
- [ ] "Stop train anywhere reliably" -> Stop reliably is more important than having a large set of possible destinations
- [ ] "Improve stopping precision, detect single switch/sensor error"
