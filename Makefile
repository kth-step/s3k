PROJECTS=projects/hello projects/ping-pong projects/trapped projects/demonstrator
all:
	@for i in $(PROJECTS); do \
		make -C $$i all; \
	done

clean:
	@for i in $(PROJECTS); do \
		make -C $$i clean; \
	done
	rm -rf docs

docs:
	doxygen

format:
	clang-format -i $(shell find -name '*.[hc]' -not -path '*/.*')

.PHONY: all docs clean
