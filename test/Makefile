
vpath %.c ../src
SRCS=cap.c cnode.c exception.c lock.c proc.c schedule.c syscall.c syscall_monitor.c \
     mockup.c
TEST_SRCS=$(wildcard test_*.cc)

OBJS=$(patsubst %.c,obj/%.o, $(SRCS))
TEST_OBJS=$(patsubst %.cc,obj/%.o, $(TEST_SRCS))

CFLAGS=-I../inc -DNPROC=2 -DNCAP=32 -DNSLACK=1000 -DNSLICE=128 -DNTICK=10000 -DNHART=2
CXXFLAGS=-I../inc

all: run_tests.out

obj/%.o : %.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c -o $@ $<

obj/%.o: %.cc
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -c -o $@ $^ -lgtest

run_tests.out: $(OBJS) $(TEST_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ -lgtest -lgtest_main

clean:
	rm -f $(OBJS) $(TEST_OBJS) run_test.out
