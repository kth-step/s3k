PROJECTS=projects/hello projects/ping-pong
all:
	@for i in $(PROJECTS); do \
		make -C $$i all; \
	done

clean:
	@for i in $(PROJECTS); do \
		make -C $$i clean; \
	done

docs:
	doxygen

format:
	clang-format -i $(shell find -name '*.[hc]' -not -path '*/.*')

.PHONY: all docs clean
