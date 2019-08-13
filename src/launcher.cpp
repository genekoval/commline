#include <nova/launcher.h>
#include <nova/logger.h>

#include <cstdlib>
#include <exception>
#include <iostream>

using nova::launcher;
using std::cout;
using std::endl;
using std::exception;
using std::function;
using std::string_view;

launcher::launcher(string_view name, string_view version) :
    name(name),
    version(version)
{}

int launcher::start(const function<void()>& exec) {
    try {
        exec();
    } catch (const exception& ex) {
        cout << "Error: " << ex.what() << endl;
        cout << "program returned " << EXIT_FAILURE << endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
