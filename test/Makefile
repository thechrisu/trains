.PHONY: test_buffer

.PRECIOUS: test_buffer.out

all:
	-make clean
	-make test_buffer.out

test:
	-./test_buffer.out

test_buffer.out:
	g++ -o test_buffer.out test_buffer.cc ../src/buffer.c \
	../src/a0terminal.c ../src/a0trackstate.c ../test-resources/assert.c \
	../src/track/track_data.c ../include/glue/mytimer.c -DTESTING \
	  --std=c++14 -pthread -I/usr/local/include/gtest \
	  -I/usr/local/include/boost -pthread /usr/local/lib/libgtest.a
# -I../include/glue \

clean:
	-  rm -f a.out test_buffer.out
