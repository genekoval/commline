#pragma once

#include <functional>
#include <string>
#include <unordered_map>

namespace commline::writer {
    enum variable {
        commands,
        name,
        options,
        version
    };

    using variable_handler = std::function<std::string()>;
    using variable_map = std::unordered_map<variable,variable_handler>;
    using template_string = std::string;

    extern const template_string commands_header;
    extern const template_string main_source;

    std::string fill_template(
        const template_string& templ,
        const variable_map& handlers
    );
}
