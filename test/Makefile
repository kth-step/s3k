
vpath %.c ../src
SRCS=cap.c cnode.c exception.c lock.c proc.c schedule.c syscall.c syscall_monitor.c mockup.c
OBJS=$(SRCS:.c=.o)

TEST_CC=$(wildcard test_*.cc)
TEST_OBJS=$(TEST_CC:.cc=.o)

CFLAGS=-I../inc -DNPROC=2 -DNCAP=32 -DNSLACK=1000 -DNSLICE=128 -DNTICK=10000 -DNHART=2
CXXFLAGS=-I../inc

all: run_tests.out

%.o : %.c
	$(CC) $(CFLAGS) -c -o $@ $<

%.o: test_%.cc
	$(CXX) $(CXXFLAGS) -c -o $@ $^ -lgtest

run_tests.out: $(OBJS) $(TEST_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ -lgtest -lgtest_main

clean:
	rm -f *.o *.out
