#include <nova/launcher.h>
#include <nova/logger.h>

#include <cstdlib>

using nova::launcher;
using std::function;
using std::string_view;

launcher::launcher(string_view name, string_view version) :
    name(name),
    version(version)
{}

int launcher::start(const function<void()>& exec) {
    exec();

    return EXIT_SUCCESS;
}
