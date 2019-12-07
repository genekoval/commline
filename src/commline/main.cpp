#include <commline/commline.h>
#include <vector>
#include <unordered_map>

using commline::origin;
using commline::option;
using std::string;
using std::vector;
using std::unordered_map;

namespace commline {
    program& current_program() {
        static program instance(NAME, VERSION);
        return instance;
    }
}

int main(int argc, char** argv) {
    return origin({
        option(
            "n",
            "build-name",
            true,
            "Program name"
        ),
        option(
            "V",
            "build-version",
            true,
            "Program version"
        ),
        option(
            "c",
            "config-path",
            true,
            "Path to the configuration file"
        ),
        option(
            "h",
            "header-dir",
            true,
            "Where to write header file"
        )
    }, {
    }).run(argc, argv);
}
