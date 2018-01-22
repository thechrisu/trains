.PHONY: test_buffer

.PRECIOUS: test_buffer.out

FLAGS = -DTESTING --std=c++14 -pthread -I../include/kernel/glue -I../src/attributes.h -I$(GTEST) -I$(BOOST) $(LIBGTEST) -Wno-deprecated -Wno-writable-strings
SRC = $(shell find ../src -name '*.c' -not -name 'cp_vec.c') $(shell find ../test-resources -name '*.c') $(shell find ../include/kernel/glue/mytimer.c)
SRC_SCHEDULER = ../src/multitasking/scheduler.c ../src/multitasking/ready_queue.c
SRC_TASK = $(SRC)

all:
	-make clean
	-make test_buffer.out
	-make test_ready_queue.out
	-make test_scheduler.out
	-make test_task.out
test:
	#-./test_buffer.out
	-./unit/test_ready_queue.out
	-./unit/test_scheduler.out
	-./unit/test_task.out
	-python3 -m unittest discover -s e2e/ -p 'test_*.py' -v

test_buffer.out: unit/test_buffer.cc $(SRC)
	g++ -o unit/$@ $< $(SRC) $(FLAGS)

test_ready_queue.out: unit/test_ready_queue.cc $(SRC_SCHEDULER)
	g++ -o unit/$@ $< $(SRC_SCHEDULER) $(FLAGS)

test_scheduler.out: unit/test_scheduler.cc $(SRC_SCHEDULER)
	g++ -o unit/$@ $< $(SRC_SCHEDULER) $(FLAGS)

test_task.out: unit/test_task.cc $(SRC_TASK)
	g++ -o unit/$@ $< $(SRC_TASK) $(FLAGS)

clean:
	-  rm -f *.out unit/*.out
