#include <generator.h>

#include <iomanip>
#include <nova/ext/string.h>
#include <sstream>

using nova::ext::string::join;
using std::ostream;
using std::ostringstream;
using std::quoted;
using std::string;
using std::vector;

namespace commline::generator {
    ostream& operator<<(ostream& os, const option& opt) {
        os << "option(";

        std::ostringstream boolstream;
        boolstream << std::boolalpha << opt.has_arg;

        os << join(vector<string> {
            "\"" + opt.short_opt + "\"",
            "\"" + opt.long_opt + "\"",
            boolstream.str(),
            "\"" + opt.description + "\"",
        }, ", ");

        os << ")";

        return os;
    }

    ostream& operator<<(ostream& os, const command& com) {
        os << "{" << "\"" + com.name + "\"" << ", command(";
        os << "\n\t\t\t\"" + com.description + "\", \n\t\t\t{\n\t\t\t\t";
        os << join(com.options, ",\n\t\t\t\t");
        os << "\n\t\t\t},\n\t\t\t";
        os << "commline::commands::" << com.name;
        os << "\n\t\t)}";

        return os;
    }

    string origin::format_command_functions() const {
        vector<string> functions;

        for (const auto& com : commands) {
            ostringstream os;
            os << "void " << com.name << "(const cli&);";
            functions.push_back(os.str());
        }

        return join(functions, "\n\t");
    }

    string origin::format_commands() const {
        return join(commands, ",\n\t\t");
    }

    string origin::format_options() const {
        return join(options, ",\n\t\t");
    }
}
