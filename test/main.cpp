#include <nova/cli.h>

#include <iostream>
#include <stdexcept>

using namespace nova::cli;
using namespace std;

static auto opts = options{}
    .add(option("h", "hello", true, "a nice greeting"));

void program(const launcher& runner) {
    cout << "Hello, " << opts.value("h").value_or("no one") << "!" << endl;

    for (const auto& arg : runner.args())
        cout << "You gave me: " << arg << endl;
}

int main(int argc, const char** argv) {
    return launcher(PROJECT, VERSION, opts)
        .start(argc, argv, program);
}
