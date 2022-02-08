#pragma once

#include <commline/arguments.h>
#include <commline/argv.h>
#include <commline/context.h>
#include <commline/option_list.h>

#include <array>
#include <functional>
#include <memory>
#include <map>

namespace commline {
    class command_node : public describable {
        std::map<std::string, std::unique_ptr<command_node>> commands;
    protected:
        auto print_help(std::ostream& out) const -> void {
            constexpr auto spacing = 15;

            if (commands.empty()) return;

            print::header(out, "Commands");

            for (const auto& [key, value] : commands) {
                print::indent(out);
                out << key;

                print::spaces(out, spacing - key.size());
                out << value->description << "\n";
            }
        }
    public:
        const std::string name;

        command_node(std::string_view name, std::string_view description) :
            describable(description),
            name(name)
        {}

        virtual auto execute(
            const app& context,
            const argv& args,
            std::ostream& out
        ) -> void = 0;

        template <typename InputIt>
        auto find(InputIt& first, InputIt last) -> command_node* {
            if (first != last) {
                auto node = commands.find(std::string(*first));

                if (node != commands.end()) {
                    return node->second->find(++first, last);
                }
            }

            return this;
        }

        auto subcommand(std::unique_ptr<command_node>&& node) -> command_node* {
            auto result = commands.insert({
                node->name,
                std::move(node)
            });

            return result.first->second.get();
        }
    };

    template <typename Callable, typename Options, typename Arguments>
    class command_impl : public command_node {
        Callable fn;
        Options options;
        Arguments arguments;

        template <typename Opts, typename Args>
        auto print_help(
            std::ostream& out,
            const Opts& opts,
            const Args& args
        ) const -> void {
            out << description << "\n\n" << "Usage: " << name;

            if (opts.size() > 0) {
                out << " [options]";
                if (args.size() > 0) out << " [--]";
            }

            args.print_help(out);
            out << "\n";

            opts.print_help(out);

            command_node::print_help(out);
        }
    public:
        command_impl(
            std::string_view name,
            std::string_view description,
            Callable&& fn,
            Options&& options,
            Arguments&& arguments
        ) :
            command_node(name, description),
            fn(std::move(fn)),
            options(std::move(options)),
            arguments(std::move(arguments))
        {}

        auto execute(
            const app& context,
            const argv& argv,
            std::ostream& out
        ) -> void override {
            auto positional = std::vector<std::string_view>();
            auto opts = option_list(std::move(options));
            auto args = positional_arguments(std::move(arguments));

            opts.parse(
                argv.begin(),
                argv.end(),
                [&positional](std::string_view arg) {
                    positional.push_back(arg);
                }
            );

            if (opts.help()) {
                print_help(out, opts, args);
                return;
            }

            std::apply(fn, std::tuple_cat(
                std::make_tuple(context),
                opts.extract(),
                args.parse(positional)
            ));
        }
    };

    template <typename Callable, typename Options, typename Arguments>
    auto command(
        std::string_view name,
        std::string_view description,
        Options&& options,
        Arguments&& arguments,
        Callable&& fn
    ) -> std::unique_ptr<command_node> {
        return std::make_unique<command_impl<
            Callable,
            Options,
            Arguments
        >>(
            name,
            description,
            std::move(fn),
            std::move(options),
            std::move(arguments)
        );
    }
}
