cc := gcc
rm := rm -rf
cp := cp
mkdir := mkdir -p
pkg_config := pkg-config

cflags := -std=c17 -Wall -Wextra -g -O0 -fPIC

release ?= 0
ifeq ($(release),1)
  cflags := -std=c17 -Wall -Wextra -O2 -fPIC
endif

src_dir := src
include_dir := include
test_dir := test
build_dir := build

bin_dir := bin
bin_inc_dir := $(bin_dir)/include
bin_pkg_dir := $(bin_dir)/lib/pkgconfig

lib_name := tapi
lib_file := $(bin_dir)/lib$(lib_name).so

capstone_cflags := $(shell $(pkg_config) --cflags capstone 2>/dev/null)
capstone_libs   := $(shell $(pkg_config) --libs   capstone 2>/dev/null)

have_capstone := 0
ifneq ($(strip $(capstone_libs)),)
  have_capstone := 1
endif

cflags += -I$(include_dir)

includes := $(shell find $(include_dir) -type d 2>/dev/null)
includes += $(shell find $(src_dir) -type d 2>/dev/null)
cflags += $(addprefix -I,$(includes))

ifeq ($(have_capstone),1)
  cflags += $(capstone_cflags)
endif

srcs := $(shell find $(src_dir) -name '*.c')
objs := $(patsubst $(src_dir)/%.c,$(build_dir)/%.o,$(srcs))

ldflags := -shared -Wl,-rpath,'$$ORIGIN'
ldlibs :=

ifeq ($(have_capstone),1)
  ldlibs += $(capstone_libs)
endif

project_headers := $(shell find $(include_dir) -name '*.h')
project_headers_dst := $(patsubst $(include_dir)/%,$(bin_inc_dir)/%,$(project_headers))

pc_file := $(bin_pkg_dir)/$(lib_name).pc

prefix ?= /usr/local
includedir ?= $(prefix)/include
libdir ?= $(prefix)/lib
pkgconfigdir ?= $(libdir)/pkgconfig
destdir ?=

test_srcs := $(shell find $(test_dir) -maxdepth 1 -name '*.c' 2>/dev/null)
test_bins := $(patsubst $(test_dir)/%.c,$(bin_dir)/test_%,$(test_srcs))

test_cflags := -std=c17 -Wall -Wextra -g -O0 -I$(bin_inc_dir)
test_ldflags := -L$(bin_dir) -Wl,-rpath,'$$ORIGIN'
test_ldlibs := -l$(lib_name)

.PHONY: all
all: deps stage_headers $(pc_file) $(lib_file)

.PHONY: deps
deps:
ifeq ($(have_capstone),1)
	@true
else
	@echo "capstone not found (install libcapstone-dev / capstone-devel)"
	@exit 1
endif

$(build_dir)/%.o: $(src_dir)/%.c
	@$(mkdir) $(dir $@)
	$(cc) $(cflags) -c $< -o $@

$(lib_file): $(objs)
	@$(mkdir) $(dir $@)
	$(cc) $(cflags) $(ldflags) $^ -o $@ $(ldlibs)

.PHONY: stage_headers
stage_headers: $(project_headers_dst)

$(bin_inc_dir)/%: $(include_dir)/%
	@$(mkdir) $(dir $@)
	$(cp) $< $@

$(pc_file): $(lib_file)
	@$(mkdir) $(bin_pkg_dir)
	@printf "prefix=%s\n" "$(prefix)" >  $(pc_file)
	@printf "includedir=%s\n" "$(includedir)" >> $(pc_file)
	@printf "libdir=%s\n" "$(libdir)" >> $(pc_file)
	@printf "\n" >> $(pc_file)
	@printf "name: %s\n" "$(lib_name)" >> $(pc_file)
	@printf "description: tapi shared library\n" >> $(pc_file)
	@printf "version: 0\n" >> $(pc_file)
	@printf "cflags: -I$${includedir}\n" >> $(pc_file)
	@printf "libs: -L$${libdir} -l%s\n" "$(lib_name)" >> $(pc_file)

.PHONY: test
test: all $(test_bins)

$(bin_dir)/test_%: $(test_dir)/%.c $(lib_file) stage_headers
	@$(mkdir) $(dir $@)
	$(cc) $(test_cflags) $< -o $@ $(test_ldflags) $(test_ldlibs)

.PHONY: run_test
run_test: test
	@set -e; for t in $(test_bins); do echo "running $$t"; $$t; done

.PHONY: install
install: all
	@$(mkdir) "$(destdir)$(libdir)"
	@$(cp) $(lib_file) "$(destdir)$(libdir)/"
	@$(mkdir) "$(destdir)$(includedir)"
	@$(cp) -a "$(bin_inc_dir)/." "$(destdir)$(includedir)/"
	@$(mkdir) "$(destdir)$(pkgconfigdir)"
	@$(cp) $(pc_file) "$(destdir)$(pkgconfigdir)/"

.PHONY: uninstall
uninstall:
	@$(rm) "$(destdir)$(libdir)/lib$(lib_name).so"
	@$(rm) "$(destdir)$(pkgconfigdir)/$(lib_name).pc"

.PHONY: clean
clean:
	$(rm) $(build_dir) $(bin_dir)
