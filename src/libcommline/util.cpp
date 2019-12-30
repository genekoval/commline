#include <commline/commline.h>
#include <iostream>
#include <nova/color.h>

using std::cout;
using std::endl;
using std::exception;
using std::runtime_error;
using std::string;
using std::string_view;

namespace commline {
    constexpr string_view line_prefix(">> ");

    cli_error::cli_error(const string& message) : runtime_error(message) {}

    namespace util {
        void print_error(string_view message) {
            using namespace nova::termcolor;

            cout
                << set(format::bold, color::red)
                << line_prefix
                << reset()
                << message
                << endl;
        }

        void print_error(const exception& ex) {
            print_error(ex.what());
        }
    }
}
