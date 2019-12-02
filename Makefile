project = cli++
version = 0.1.0

targets = commline libcli++ sample
extensions = cli

libcli++.type = shared

commline.type = executable
commline.libs = color++ cli++ yaml-cpp
commline.deps = libcli++

sample.type = executable
sample.libs = color++ cli++
sample.deps = commline cli

include $(DEVROOT)/include/mkbuild/base.mk

$(commline): CXXFLAGS += -DNAME='"commline"' -DVERSION='"$(version)"'

$(obj)/%/cli.o: $(src)/%/cli.yaml $(commline)
	$(commline) \
		--build-name=$* \
		--build-version=$(version) \
		--config-path=$< \
		--header-dir=$(src)/$*/include | \
	$(COMPILE.cpp) -o $@ -c -x c++ $(CXXFLAGS) -

CLEAN += $(src)/*/include/commline

runopts = -n "artifact" -V "0.0.1" -c "$(src)/sample"

run: $(commline)
	@$< $(runopts)

debug: $(commline)
	@gdb --ex=start --args $< $(runopts)
