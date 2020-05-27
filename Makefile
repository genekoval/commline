project = commline
version = 0.1.0

STD := gnu++2a

library = lib$(project)

install = $(project) $(library)
targets := $(install) sample

extensions = cli

$(library).type = shared

$(project).type = executable
$(project).libs = $(project) yaml-cpp
$(project).deps = $(library)

sample.type = executable
sample.libs = $(project)
sample.deps = $(project) cli

BUILD = /tmp/$(project)

include mkbuild/base.mk

$($(project)): CXXFLAGS += -DNAME='"$(project)"' -DVERSION='"$(version)"'

$(obj)/%/cli.o: $(src)/%/cli.yaml $($(project))
	$($(project)) \
		--build-name=$* \
		--build-version=$(version) \
		--config-path=$< \
		--header-dir=$(src)/$*/include | \
	$(COMPILE.cpp) -o $@ -c -x c++ $(CXXFLAGS) -

CLEAN += $(src)/*/include/$(project)

runopts = -n "artifact" -V "0.0.1" -c "$(src)/sample"

run: $($(project))
	@$< $(runopts)

debug: $($(project))
	@gdb --ex=start --args $< $(runopts)
