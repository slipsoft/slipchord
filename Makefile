SUBDIRS := $(wildcard ex*/.)

# Top-level phony targets.
.PHONY: all clean run
all clean run: $(SUBDIRS)

# Recurse `make` into each subdirectory
# Pass along targets specified at command-line (if any).
.PHONY: $(SUBDIRS)
$(SUBDIRS):
	$(MAKE) -C $@ $(MAKECMDGOALS)
