#pragma once

#include <array>
#include <functional> // function
#include <initializer_list> // initializer_list
#include <optional> // optional
#include <stdexcept> // runtime_error
#include <string> // string
#include <string_view> // string_view
#include <unordered_map> // unordered_map
#include <variant>
#include <utility>
#include <vector> // vector

namespace commline {
    struct cli_error : std::runtime_error {
        using runtime_error::runtime_error;
    };

    struct has_description {
        const std::string description;
    };

    class program;
    program& current_program();

    class program {
    friend program& current_program();
    private:
        std::string m_name;
        std::string m_version;

        program(
            std::string_view name,
            std::string_view version
        );
    public:
        std::string_view name();
        std::string_view version();
    };

    class action;

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
            parameter_type param,
            std::initializer_list<std::string> aliases
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
                if (current.starts_with("-- ")) {
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

    class option_table {
    friend class cli;
    private:
        std::vector<option> opts;
        std::unordered_map<std::string,option*> opt_map;

        void add_to_map(option* opt);
        option& get(const std::string& key);
    public:
        option_table(std::initializer_list<option> option_list);

        void add(const option& opt);
        bool selected(const std::string& key) const;
        std::optional<std::string> value(const std::string& key) const;
    };

    class cli {
    friend class action;
    private:
        std::vector<std::string> m_args;
        std::string m_exec_path;
        option_table* option_ptr;

        cli(std::string_view exec_path, option_table* option_ptr);

        void parse(unsigned int argc, char** argv);
    public:
        const std::vector<std::string>& args() const;
        std::string_view exec_path() const;
        const option_table& options() const;
    };

    void main(const cli&);

    using exec = std::function<void(const cli&)>;

    class action {
        exec function;
        option_table opts;
    public:
        action(
            std::initializer_list<option> option_list,
            const exec function
        );

        void operator()(char* exec_path, unsigned int argc, char** argv);
    };

    class command {
        action m_action;
        std::string m_description;
    public:
        command(
            std::string_view description,
            std::initializer_list<option> option_list,
            const exec function
        );

        const action& get_action();
    };

    class origin {
        action m_action;
        std::unordered_map<std::string, command> commands;
    public:
        origin(
            std::initializer_list<option> options,
            std::initializer_list<std::pair<
                const std::string,
                command
            >> commands
        );

        int run(unsigned int argc, char** argv);
    };

    namespace util {
        void print_error(std::string_view message);
        void print_error(const std::exception& ex);
    }
}
