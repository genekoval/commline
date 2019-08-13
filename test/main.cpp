#include <nova/launcher.h>

#include <iostream>
#include <stdexcept>

using namespace nova::cli;
using namespace std;

static auto opts = options{}
    .add(option("h", "hello", true, "a nice greeting"));

void exec(const vector<string> args) {
    cout << "Hello, " << opts.value("h").value_or("no one") << "!" << endl;
}

int main(int argc, const char** argv) {
    return launcher(PROJECT, VERSION)
        .start(opts, argc, argv, exec);
}
