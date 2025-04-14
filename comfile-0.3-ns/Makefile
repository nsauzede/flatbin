# Generic top-level Makefile.

.PHONY: all clean

all clean:
	for d in mod lib src; do \
	(cd $$d ; $(MAKE) $@); \
	done
