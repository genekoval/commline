$(obj)/%/cli.o: $(src)/%/cli.yaml
	cli-gen \
		--build-name=$* \
		--build-version=$(version) \
		--config-path=$< | \
	$(COMPILE.cpp) -o $@ -c -x c++ $(CXXFLAGS) -
