TESTS=$(wildcard test_*.cc)
TESTS_EXEC=$(TESTS:.cc=.out)

FLAGS=-lgtest -I../inc

all: $(TESTS_EXEC)

test_cap.out: test_cap.cc
	g++ $(FLAGS) -o $@ $<
