.PHONY: test_all.out test_buffer.out test_ready_queue.out test_scheduler.out test_messaging.out test_task.out test_stdlib.out alltests

.PRECIOUS: test_buffer.out test_all.out

FLAGS = -DTESTING --std=c++1y -pthread -I../include -I../kernel/src -I../lib -I../kernel/src/multitasking -I../kernel/src/syscall -I../usr/lib -I$(GTEST) -I$(BOOST) $(LIBGTEST) -Wno-deprecated -Wno-write-strings
SRC = $(shell find ../kernel/src -name '*.c' -not -name 'cp_vec.c') $(shell find ../lib -name '*.c') $(shell find ../test-resources -name '*.c') $(shell find ../include/mytimer.c) $(shell find ../usr/track -name '*.c') $(shell find ../usr/a0 -name '*.c') $(shell find ../usr/lib -name '*.c')
SRC_SCHEDULER = ../kernel/src/multitasking/scheduler.c ../kernel/src/multitasking/ready_queue.c
SRC_TASK = $(SRC)
SRC_CLOCK_WAIT_QUEUE = ../usr/lib/clock_wait_queue.c

SRC_TEST = $(shell find unit -name '*.cc')

all:
	$(MAKE) clean
	$(MAKE) all1

all1: test_buffer.out test_ready_queue.out test_scheduler.out test_task.out test_messaging.out \
			test_stdlib.out test_all.out
	./unit/test_all.out
	./unit/test_buffer.out
	./unit/test_ready_queue.out
	./unit/test_scheduler.out
	./unit/test_messaging.out
	./unit/test_stdlib.out
	./unit/test_task.out

alltests: test_all.out
	$(MAKE) test

test:
	./unit/test_all.out
	python3 -m unittest discover -s e2e/ -p 'test_*.py' -v

test_all.out: $(SRC) $(SRC_TEST)
	g++ -o unit/$@ $(SRC) $(SRC_TEST) $(FLAGS) -DALLTESTS

test_buffer.out: unit/test_buffer.cc $(SRC)
	g++ -o unit/$@ $< $(SRC) $(FLAGS)

test_ready_queue.out: unit/test_ready_queue.cc $(SRC_SCHEDULER)
	g++ -o unit/$@ $< $(SRC_SCHEDULER) $(FLAGS)

test_clock_wait_queue.out: unit/test_clock_wait_queue.cc $(SRC_CLOCK_WAIT_QUEUE)
	g++ -o unit/$@ $< $(SRC_CLOCK_WAIT_QUEUE) $(FLAGS)

test_scheduler.out: unit/test_scheduler.cc $(SRC_SCHEDULER)
	g++ -o unit/$@ $< $(SRC_SCHEDULER) $(FLAGS)

test_messaging.out: unit/test_messaging.cc $(SRC)
	g++ -o unit/$@ $< $(SRC) $(FLAGS)

test_task.out: unit/test_task.cc $(SRC_TASK)
	g++ -o unit/$@ $< $(SRC_TASK) $(FLAGS)

test_stdlib.out: unit/test_stdlib.cc $(SRC)
	g++ -o unit/$@ $< $(SRC) $(FLAGS)

clean:
	rm -f *.out unit/*.out
