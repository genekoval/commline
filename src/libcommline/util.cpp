#include <commline/commline.h>
#include <iostream>
#include <nova/color.h>

using std::cout;
using std::endl;
using std::exception;
using std::runtime_error;
using std::string;

namespace commline {
    cli_error::cli_error(const string& message) : runtime_error(message) {}

    namespace util {
        void print_error(const exception& ex) {
            using namespace nova::termcolor;

            cout
                << set(format::bold, color::red)
                << "error: "
                << reset()
                << ex.what()
                << endl;
        }
    }
}
