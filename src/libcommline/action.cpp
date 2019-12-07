#include <commline/commline.h>

using std::initializer_list;
using std::string;
using std::string_view;
using std::vector;

namespace commline {
    action::action(
        initializer_list<option> option_list,
        const exec function
    ) :
        function(function),
        opts(option_list)
    {}

    void action::operator()(char* exec_path, unsigned int argc, char** argv) {
        cli c(exec_path, &opts);
        c.parse(argc, argv);
        function(c);
    }
}
