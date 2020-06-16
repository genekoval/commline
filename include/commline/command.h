#pragma once

#include <commline/argv.h>
#include <commline/context.h>
#include <commline/option_list.h>

#include <array>
#include <functional>
#include <memory>
#include <unordered_map>

namespace commline {
    class command_node : public describable {
        std::unordered_map<std::string, std::unique_ptr<command_node>> commands;
    public:
        const std::string name;

        command_node(std::string_view name, std::string_view description) :
            describable(description),
            name(name)
        {}

        virtual auto execute(const app& context, const argv& args) -> void = 0;

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

    template <typename Callable, typename ...Options>
    class command_impl : public command_node {
        Callable fn;
        option_list<Options...> opts;
    public:
        command_impl(
            std::string_view name,
            std::string_view description,
            Callable fn
        ) :
            command_impl(name, description, options(), fn)
        {}

        command_impl(
            std::string_view name,
            std::string_view description,
            std::tuple<Options...>&& opts,
            Callable fn
        ) :
            command_node(name, description),
            fn(fn),
            opts(std::move(opts))
        {}

        auto execute(const app& context, const argv& args) -> void override {
            auto arguments = argv();

            opts.parse(
                args.begin(),
                args.end(),
                [&arguments](std::string_view arg) {
                    arguments.push_back(arg);
                }
            );

            std::apply(fn, std::tuple_cat(
                std::make_tuple(
                    context,
                    arguments
                ),
                opts.extract()
            ));
        }
    };

    template <typename Callable>
    auto command(
        std::string_view name,
        std::string_view description,
        Callable fn
    ) -> std::unique_ptr<command_node> {
        return std::make_unique<command_impl<Callable>>(
            name,
            description,
            fn
        );
    }

    template <typename Callable, typename ...Options>
    auto command(
        std::string_view name,
        std::string_view description,
        std::tuple<Options...>&& opts,
        Callable fn
    ) -> std::unique_ptr<command_node> {
        return std::make_unique<command_impl<Callable, Options...>>(
            name,
            description,
            std::move(opts),
            fn
        );
    }
}
