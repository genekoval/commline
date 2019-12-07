#include <templates.h>

#include <nova/ext/string.h>
#include <regex>
#include <resources.h>
#include <stdexcept>

using nova::ext::string::replace;
using std::invalid_argument;
using std::out_of_range;
using std::regex;
using std::smatch;
using std::string;
using std::unordered_map;

namespace commline::templates {
    const std::string commands_header(
        reinterpret_cast<char*>(resources_commands_h),
        resources_commands_h_len
    );

    const std::string main_source(
        reinterpret_cast<char*>(resources_main_template_cpp),
        resources_main_template_cpp_len
    );

    variable variable_parse(const string& str) {
        static const unordered_map<string,variable> lookup = {
            {"commands", variable::commands},
            {"name", variable::name},
            {"options", variable::options},
            {"version", variable::version}
        };

        try {
            return lookup.at(str);
        }
        catch (const out_of_range& ex) {
            throw invalid_argument(
                "'" + str + "' is not defined for the 'variable' enum"
            );
        }
    }

    const regex variable_regex(R"(\$([a-zA-Z0-9]+))");

    string fill_template(
        const std::string& templ,
        const variable_map& handlers
    ) {
        return replace<string>(
            templ,
            variable_regex,
            [&handlers](const smatch& match) {
                const auto var = variable_parse(string(match[1]));
                return handlers.at(var)();
            }
        );
    }
}
