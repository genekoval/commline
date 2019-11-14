project = cli++
version = 0.1.0

libcli++.type = shared

cli++.type = executable
cli++.libs = color++ cli++
cli++.deps = libcli++

cli-gen.type = executable
cli-gen.libs = color++ cli++ yaml-cpp
cli-gen.deps = libcli++
cli-gen: CXXFLAGS += -DNAME='"cli-gen"' -DVERSION='"$(version)"'

sample.type = executable
sample.libs = color++ cli++
sample.deps = cli-gen
sample.ext = cli

include ../makefiles/include/base.mk

define ext.cli.template
 $(obj)/$(1)/ext/cli.o: $(src)/$(1)/cli.yaml
	$(MKDIR) $(obj)/$(1)/ext
	$(cli-gen.path) \
		--build-name=$(1) \
		--build-version=$(version) \
		--config-path=$(src)/$(1)/cli.yaml \
	| $(COMPILE.cpp) -o $(obj)/$(1)/ext/cli.o -c -x c++ $(CXXFLAGS) -
endef

$(foreach target,$(targets),\
 $(if $(findstring cli,$($(target).ext)),\
  $(eval $(call ext.cli.template,$(target)))))

run: cli-gen
	$($<.path) --build-name=cli-gen --build-version=1.2.3 --config-path=cli.yaml
