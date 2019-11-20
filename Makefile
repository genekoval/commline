project = cli++
version = 0.1.0

targets = cli-gen libcli++ sample
extensions = cli

libcli++.type = shared

cli-gen.type = executable
cli-gen.libs = color++ cli++ yaml-cpp
cli-gen.deps = libcli++

sample.type = executable
sample.libs = color++ cli++
sample.deps = cli-gen cli

include $(DEVROOT)/include/mkbuild/base.mk

$(cli-gen): CXXFLAGS += -DNAME='"cli-gen"' -DVERSION='"$(version)"'

$(obj)/%/cli.o: $(src)/%/cli.yaml $(cli-gen)
	$(cli-gen) \
		--build-name=$* \
		--build-version=$(version) \
		--config-path=$< | \
	$(COMPILE.cpp) -o $@ -c -x c++ $(CXXFLAGS) -
