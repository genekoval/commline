#include <commline/commands.h>
#include <commline/commline.h>
#include <iostream>

using std::cout;
using std::endl;

void commline::commands::start(const commline::cli& cli) {
    cout << "Starting the program..." << endl;

    cout
        << "Logging to: "
        << cli.options().value("log").value_or("stdout")
        << endl;
}

void commline::commands::stop(const commline::cli& cli) {
    if (cli.options().selected("force"))
        cout << "STOP!!! This is an emergency!" << endl;
    else
        cout << "Stopping the process..." << endl;
}

void commline::main(const commline::cli& cli) {
    cout
        << "Hello, "
        << cli.options().value("hello").value_or("no one")
        << "!" << endl;

    if (cli.options().selected("daemon"))
        cout << "Forking the process..." << endl;

    if (!cli.args().empty()) {
        cout << "You gave me the following args:" << endl;
        for (const auto& arg : cli.args())
            cout << "\t* " << arg << endl;
    }
}
