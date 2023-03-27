
$(info Unit tests requires Google Test to be installed)
vpath %.c ../src
SRCS=cap.c cnode.c exception.c ticket_lock.c proc.c schedule.c syscall.c \
     syscall_lock.c syscall_monitor.c mockup.c
TEST_SRCS=$(wildcard test_*.C)

OBJS=$(patsubst %.c,obj/%.o, $(SRCS))
DEPS=$(patsubst %.d,obj/%.o, $(SRCS))
TEST_OBJS=$(patsubst %.cc,obj/%.o, $(TEST_SRCS))

CFLAGS=-I../inc -I. -MMD -include config.h
CXXFLAGS=-I../inc -I. -include config.h

test: test_detail.json

obj/%.o : %.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c -o $@ $<

obj/%.o: %.cc
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -c -o $@ $^ -lgtest

run_tests.out: $(OBJS) $(TEST_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ -lgtest -lgtest_main

test_detail.json: run_tests.out
	./run_tests.out --gtest_output=json

.PHONY: test

-include: $(DEPS)
