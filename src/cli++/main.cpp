#include "file.h"

#include <nova/cli.h>

#include <iostream>
#include <stdexcept>

using namespace nova::cli;
using namespace std;

void program(const launcher& runner) {
    if (runner.options().selected("hex")) {
        cout << get_file();
        return;
    }

    cout
        << "Hello, " << runner.options().value("h").value_or("on one") << "!"
        << endl;

    for (const auto& arg : runner.args())
        cout << "Arg: " << arg << endl;

    if (runner.options().selected("daemon"))
        cout << "Forking the process..." << endl;
}

int main(int argc, const char** argv) {
    return launcher("sample", "0.1.0", {
        option("h", "hello", true, "a nice greeting"),
        option("d", "daemon", false, "run the process as a daemon"),
        option("x", "hex", false, "hexdump")
    }).start(argc, argv, program);
}
