.PHONY: test_buffer

.PRECIOUS: test_buffer.out

FLAGS = -DTESTING --std=c++14 -pthread -I../include/kernel/glue -I$(GTEST) -I$(BOOST) $(LIBGTEST)
SRC = $(shell find ../src -name '*.c' -not -name 'cp_vec.c' -not -name 'interrupt.c') $(shell find ../test-resources -name '*.c') $(shell find ../include/kernel/glue/mytimer.c)

all:
	-make clean
	-make test_buffer.out

test:
	-./test_buffer.out

test_buffer.out:
	g++ -o test_buffer.out test_buffer.cc $(SRC) $(FLAGS)

clean:
	-  rm -f a.out test_buffer.out
