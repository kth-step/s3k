SUBDIRS=kernel projects/hello projects/trapped projects/ping-pong projects/demonstrator

all:
	for i in $(SUBDIRS); do \
		make -C $$i all; \
	done

$(SUBDIRS):
	make -C $@ all

clean:
	for i in $(SUBDIRS); do \
		make -C $$i clean; \
	done
	rm -rf docs

docs:
	doxygen

format:
	clang-format -i $(shell find -name '*.[hc]' -not -path '*/.*')

.PHONY: all docs clean $(SUBDIRS)
