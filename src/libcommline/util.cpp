#include <commline/commline.h>
#include <iostream>

using std::exception;
using std::runtime_error;
using std::string;
using std::string_view;

namespace commline {
    cli_error::cli_error(const string& message) : runtime_error(message) {}

    namespace util {
        void print_error(string_view message) {
            std::cout << message << std::endl;
        }

        void print_error(const exception& ex) {
            print_error(ex.what());
        }
    }
}
