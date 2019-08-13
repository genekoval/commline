#include <nova/launcher.h>

#include <iostream>
#include <stdexcept>

using nova::launcher;
using std::cout;
using std::endl;
using std::runtime_error;

int exec() {
    cout << "Running test program." << endl;
    throw runtime_error("A test error.");
}

int main() {
    return launcher(PROJECT, VERSION).start(exec);
}
