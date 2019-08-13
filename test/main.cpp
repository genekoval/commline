#include <nova/launcher.h>

#include <iostream>
#include <stdexcept>

using nova::launcher;
using std::cout;
using std::endl;
using std::runtime_error;

int main() {
    launcher program(PROJECT, VERSION);

    return program.start([]() {
        throw runtime_error("A test error.");
    });
}
