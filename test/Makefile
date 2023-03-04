TESTS=$(wildcard test_*.cc)
TESTS_EXEC=$(TESTS:.cc=.out)

FLAGS=-std=gnu++17 -I../inc

all: $(TESTS_EXEC)

test_cap.out: test_cap.cc
	g++ $(FLAGS) -o $@ $< -lgtest
