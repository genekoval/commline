#pragma once

#include <array>
#include <functional> // function
#include <initializer_list> // initializer_list
#include <iostream>
#include <optional> // optional
#include <string> // string
#include <string_view> // string_view
#include <system_error>
#include <unordered_map> // unordered_map
#include <variant>
#include <utility>
#include <vector> // vector

namespace commline {
    struct has_description {
        const std::string description;
    protected:
        has_description(const std::string& desc) : description(desc) {}
    };

    template <typename Value>
    class parameter : public has_description {
    protected:
        Value val;
    public:
        using value_type = Value;

        const bool required;

        auto value() const -> const Value&;
    };

    struct flag : public parameter<bool> {
        auto enable() -> void {
            val = true;
        }
    };

    template <typename Derived, typename Value>
    class has_value : public parameter<Value> {
    protected:
        using input_type = const std::string&;
    public:
        const std::string value_name;

        auto set(input_type input) -> void {
            val = Derived::parse(input);
        }
    };

    struct string : public has_value<string, std::string> {
        static auto parse(input_type input) -> value_type {
            return input;
        }
    };

    struct integer : public has_value<integer, int> {
        static auto parse(input_type input) -> value_type {
            return std::stoi(input);
        }
    };

    struct number : public has_value<number, float> {
        static auto parse(input_type input) -> value_type {
            return std::stof(input);
        }
    };

    using parameter_type = std::variant<flag, integer, number, string>;

    class parameter_list {
        std::unordered_map<std::string, parameter_type*> aliases;
        std::vector<parameter_type> parameters;
    public:
        auto add(
            std::initializer_list<std::string> aliases,
            parameter_type param
        ) -> void {
            parameters.push_back(param);

            for (const auto& alias : aliases) {
                this->aliases[alias] = &parameters.back();
            }
        }

        auto get(const std::string& alias) -> parameter_type& {
            try {
                return *(aliases.at(alias));
            }
            catch (const std::out_of_range& ex) {
                throw cli_error("unknown option: " + alias);
            }
        }

        template <typename InputIt, typename Callable>
        auto parse(InputIt first, InputIt last, Callable handle_arg) -> void {
            for (; first != last; first++) {
                const auto current = std::string(*first++);

                // A `--` by itself signifies the end of options.
                // Everything that follows is an argument.
                if (current == "--") {
                    while (first != last) handle_arg(*first++);
                }
                // Handle a long option.
                else if (current.starts_with("--")) {
                    const auto alias = current.substr(2);
                    auto& ptype = get(alias);

                    if (auto param = std::get_if<flag>(&ptype)) {
                        (*param).enable();
                    }
                    else { // The parameter requires a value.
                        // No more args to parse.
                        if (first == last) {
                            throw cli_error("missing value for: " + alias);
                        }

                        // Take the next arg as the value.
                        std::visit([first](auto&& param) {
                            param.set(std::string(*first));
                        }, ptype);
                    }
                }
                else if (current.starts_with('-')) {
                    // A sequence of short options.
                    const auto sequence = current.substr(1);

                    auto it = sequence.begin();
                    auto end = sequence.end();

                    while (it != end) {
                        const auto alias = std::string(1, *(it++));
                        auto& ptype = get(alias);

                        if (auto param = std::get_if<flag>(&ptype)) {
                            (*param).enable();
                        }
                        else { // The parameter requires a value.
                            // The option value is the next arg after the
                            // sequence of short options. If there are more
                            // options in the sequence or there are no more
                            // args after the sequence, the nthe value is
                            // missing.
                            if (it != end || first == last) {
                                throw cli_error("missing value for: " + alias);
                            }

                            // Take the next arg as the value.
                            std::visit([first](auto&& param) {
                                param.set(std::string(*first));
                            }, ptype);
                        }
                    }
                }
                else handle_arg(current);
            }
        }
    };

    class command_metadata : public has_description {
        std::unorderd_map<std::string, command> commands;
    protected:
        parameter_list parameters;
        std::vector<std::string> arguments;

        command_metadata(const std::string& desc) : has_description(desc) {}

        virtual auto action() -> void = 0;
    public:
        auto comm(
            const std::string& name,
            const command& subcommand
        ) -> command_metadata& {
            commands[name] = subcommand;
            return *this;
        }

        template <typename InputIt>
        auto execute(InputIt first, InputIt last) -> int {
            try {
                if (first != last && commands.count(*first)) {
                    commands[*first].execute(++first, last);
                }
                else {
                    parameters.parse(
                        first,
                        last,
                        [&arguments](const std::string& arg) {
                            arguments.push_back(arg);
                        }
                    );
                    action();
                }
            }
            catch (const std::system_error& ex) {
                std::err << ex.what() << std::endl;
                return ex.code().value();
            }
            catch (const std::exception& ex) {
                std::err << ex.what() << std::endl;
                return EXIT_FAILURE;
            }

            return EXIT_SUCCESS;
        }

        auto param(
            std::initializer_list<std::string> aliases,
            parameter_type param
        ) -> command_metadata& {
            parameters.add(aliases, param);
            return *this;
        }
    }
}
