#pragma once

#include <commline/command.h>

#include <system_error>
#include <iostream>

namespace commline {
    using error_handler_t = auto (*)(const std::exception& ex) -> void;

    auto print_error(const std::exception& ex) -> void;

    template <typename Callable, typename ...Options>
    class application : public command_impl<Callable, Options...> {
        error_handler_t error_handler = &print_error;
    public:
        const std::string version;

        application(
            std::string_view name,
            std::string_view version,
            std::string_view description,
            Callable fn
        ) :
            command_impl<Callable, Options...>(name, description, fn),
            version(version)
        {}

        application(
            std::string_view name,
            std::string_view version,
            std::string_view description,
            std::tuple<Options...>&& opts,
            Callable fn
        ) :
            command_impl<Callable, Options...>(
                name,
                description,
                std::move(opts),
                fn
            ),
            version(version)
        {}

        auto on_error(error_handler_t handler) -> void {
            error_handler = handler;
        }

        auto run(int argc, const char** argv) -> int {
            const auto args = commline::collect(argc, argv);
            auto first = args.begin();
            auto last = args.end();

            const auto argv0 = *(first++);
            auto cmd = this->find(first, last);

            try {
                cmd->execute({
                        this->name,
                        version,
                        this->description,
                        argv0
                    },
                    commline::argv(first, last)
                );
            }
            catch (const std::system_error& ex) {
                error_handler(ex);
                return ex.code().value();
            }
            catch (const std::exception& ex) {
                error_handler(ex);
                return EXIT_FAILURE;
            }

            return EXIT_SUCCESS;
        };
    };
}
