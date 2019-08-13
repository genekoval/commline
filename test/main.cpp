#include <nova/launcher.h>

#include <iostream>
#include <stdexcept>

using nova::launcher;
using std::cout;
using std::endl;
using std::runtime_error;

int main() {
    launcher program("launcher test", "0.0.0");

    return program.start([]() {
        throw runtime_error("A test error.");
    });
}
