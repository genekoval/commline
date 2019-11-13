#include <nova/cli.h>
#include <iostream>

using nova::cli::launcher;
using std::cout;
using std::endl;

void nova::cli::start(const launcher& cli) {
    cout
        << "Hello, " << cli.options().value("hello").value_or("no one") << "!"
        << endl;

    if (cli.options().selected("daemon"))
        cout << "Forking the process..." << endl;

    if (!cli.args().empty()) {
        cout << "You gave me the following args:" << endl;
        for (const auto& arg : cli.args())
            cout << "\t* " << arg << endl;
    }
}
