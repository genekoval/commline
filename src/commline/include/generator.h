#pragma once

#include <filesystem>
#include <string>
#include <vector>

namespace commline::generator {
    struct option {
        std::string short_opt;
        std::string long_opt;
        bool has_arg;
        std::string description;
    };

    struct command {
        std::string name;
        std::string description;
        std::vector<option> options;
    };

    struct origin {
        std::vector<command> commands;
        std::vector<option> options;

        std::string format_command_functions() const;
        std::string format_commands() const;
        std::string format_options() const;
    };

    origin decode_config(std::filesystem::path&& path);
}
