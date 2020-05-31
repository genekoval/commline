#include <commline/argv.h>

namespace commline {
    auto collect(int argc, const char** argv) -> commline::argv {
        auto args = commline::argv();

        for (int i = 0; i < argc; i++) {
            args.push_back(std::string_view(argv[i]));
        }

        return args;
    }
}
