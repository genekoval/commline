#include <nova/launcher.h>

#include <iostream>

using nova::launcher;
using std::cout;
using std::endl;

int main() {
    launcher program("launcher test", "0.0.0");

    return program.start([]() {
        cout << "Running test program!" << endl;
    });
}
