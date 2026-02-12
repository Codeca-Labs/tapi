# compiler and compiler flags
cc ?= gcc
cflags := -g -O0 -Wall -Wextra -std=c17

# dirs relative to test/
root_dir  := ..
src_dir   := .
build_dir := $(root_dir)/build/test
bin_dir   := $(root_dir)/bin

# library we link against
lib_name := tapi
lib_file := $(bin_dir)/lib$(lib_name).so

# include paths (public headers)
cflags += -I$(root_dir)/include

# link flags:
# -L../bin so -ltapi finds libtapi.so
# rpath=$$ORIGIN so test binaries (in bin/) find libtapi.so (also in bin/) at runtime
ldflags := -L$(bin_dir) -Wl,-rpath,'$$ORIGIN'
ldlibs  := -l$(lib_name)

# each test .c becomes its own executable in bin/
test_srcs := $(shell find $(src_dir) -maxdepth 1 -name '*.c')
test_objs := $(patsubst $(src_dir)/%.c,$(build_dir)/%.o,$(test_srcs))
test_bins := $(patsubst $(src_dir)/%.c,$(bin_dir)/%,$(test_srcs))

.PHONY: all
all: $(test_bins)

$(bin_dir)/%: $(build_dir)/%.o $(lib_file)
	@mkdir -p $(@D)
	$(cc) $(cflags) $< -o $@ $(ldflags) $(ldlibs)

$(build_dir)/%.o: $(src_dir)/%.c
	@mkdir -p $(dir $@)
	$(cc) $(cflags) -c $< -o $@

.PHONY: run
run: all
	@set -e; for t in $(test_bins); do echo "==> $$t"; $$t; done

.PHONY: clean
clean:
	rm -rf $(build_dir)
	rm -f $(test_bins)