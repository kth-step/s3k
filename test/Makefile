TEST_CC=$(wildcard test_*.cc)
TEST_OUT=$(TEST_CC:.cc=.out)
TEST_RES=$(TEST_CC:.cc=.res)

FLAGS=-std=gnu++17 -I../inc

all: $(TEST_OUT) $(TEST_RES)

test_%.out: test_%.cc
	g++ $(FLAGS) -o $@ $< -lgtest

test_%.res: test_%.out
	./$< > $@ 
	cat $@
