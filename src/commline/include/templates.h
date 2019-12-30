#pragma once

#include <functional>
#include <string>
#include <unordered_map>

namespace commline::templates {
    enum variable {
        commands,
        header,
        name,
        options,
        version
    };

    using variable_handler = std::function<std::string()>;
    using variable_map = std::unordered_map<variable,variable_handler>;

    extern const std::string commands_header;
    extern const std::string main_source;

    std::string fill_template(
        const std::string& templ,
        const variable_map& handlers
    );
}
