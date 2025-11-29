.POSIX:

# Load saved environment if it exists
BUILDDIR	?= builddir
ENVFILE         = $(BUILDDIR)/env.mk
-include $(ENVFILE)

# Tools and defaults (overridable):
MESON		?= meson
NINJA		?= ninja
CROSS_FILE	?= ./cross/rv64imac.ini
BUILD_TYPE	?= debugoptimized
NINJA_FLAGS	?=

# Default target
all: build

debugoptimized:
	$(MAKE) -B BUILD_TYPE=debugoptimized build

debug:
	$(MAKE) -B BUILD_TYPE=debug build

release:
	$(MAKE) -B BUILD_TYPE=release build

minsize:
	$(MAKE) -B BUILD_TYPE=minsize build

# Build
build: | ${BUILDDIR}
	$(NINJA) -C $(BUILDDIR) $(NINJA_FLAGS)

$(BUILDDIR):
	$(MESON) setup $(BUILDDIR) --cross-file $(CROSS_FILE) --buildtype=$(BUILD_TYPE) --reconfigure
	@echo "MESON?=$(MESON)" > $(ENVFILE)
	@echo "NINJA?=$(NINJA)" >> $(ENVFILE)
	@echo "CROSS_FILE?=$(CROSS_FILE)" >> $(ENVFILE)
	@echo "BUILD_TYPE?=$(BUILD_TYPE)" >> $(ENVFILE)
	@echo "NINJA_FLAGS?=$(NINJA_FLAGS)" >> $(ENVFILE)
	@echo "Environment saved to $(ENVFILE)"


# Clean artifacts but keep configuration
clean:
	-$(NINJA) -C $(BUILDDIR) clean

# Remove build directory entirely
distclean:
	rm -rf $(BUILDDIR) $(ENVFILE)

# Show common targets and knobs
help:
	@echo "Targets:"
	@echo "  build            Build with ninja (default)"
	@echo "  debugoptimized   Build with BUILD_TYPE=debugoptimized"
	@echo "  debug            Build with BUILD_TYPE=debug"
	@echo "  release          Build with BUILD_TYPE=release"
	@echo "  minsize          Build with BUILD_TYPE=minsize"
	@echo "  clean            Clean build outputs (keep config)"
	@echo "  distclean        Remove build directory entirely"
	@echo "  env              Print current configuration"
	@echo ""
	@echo "Variables (override with make VAR=value):"
	@echo "  BUILDDIR=$(BUILDDIR)"
	@echo "  MESON=$(MESON)"
	@echo "  NINJA=$(NINJA)"
	@echo "  CROSS_FILE=$(CROSS_FILE)"
	@echo "  BUILD_TYPE=$(BUILD_TYPE) (e.g., debug, debugoptimized, release)"
	@echo "  NINJA_FLAGS=$(NINJA_FLAGS) (e.g., -j$$(nproc))"

# Print current config variables
env:
	@echo BUILDDIR=$(BUILDDIR)
	@echo MESON=$(MESON)
	@echo NINJA=$(NINJA)
	@echo CROSS_FILE=$(CROSS_FILE)
	@echo BUILD_TYPE=$(BUILD_TYPE)
	@echo NINJA_FLAGS=$(NINJA_FLAGS)

.PHONY: all build clean distclean help env debug debugoptimized release minsize
