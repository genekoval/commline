CLEAN += $(src)/*/include/commline/commands.h

$(obj)/%/cli.o: $(src)/%/cli.yaml
	commline \
		--build-name=$* \
		--build-version=$(version) \
		--config-path=$< \
		--header-dir=$(src)/$*/include | \
	$(COMPILE.cpp) -o $@ -c -x c++ $(CXXFLAGS) -
