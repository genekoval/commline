#include <nova/cli.h>

#include <iostream>
#include <stdexcept>

using namespace nova::cli;
using namespace std;

void program(const launcher& runner) {
    cout
        << "Hello, " << runner.options().value("h").value_or("on one") << "!"
        << endl;

    if (runner.options().selected("hello"))
        cout << "You selected 'hello'." << endl;

    for (const auto& arg : runner.args())
        cout << "You gave me: " << arg << endl;

    if (runner.options().selected("daemon"))
        cout << "Forking the process..." << endl;
}

int main(int argc, const char** argv) {
    return launcher(PROJECT, VERSION, {
        option("h", "hello", true, "a nice greeting"),
        option("d", "daemon", false, "run the process as a daemon")
    }).start(argc, argv, program);
}
