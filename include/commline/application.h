#pragma once

#include <commline/command.h>

#include <system_error>
#include <iostream>

namespace commline {
    using error_handler_t = auto (*)(std::exception_ptr eptr) -> void;

    auto print_error(std::exception_ptr eptr) -> void;

    template <typename Callable, typename Options, typename Arguments>
    class app_impl final : public command_impl<Callable, Options, Arguments> {
        error_handler_t error_handler = &print_error;
    public:
        const std::string version;

        app_impl(
            std::string_view name,
            std::string_view version,
            std::string_view description,
            Callable&& fn,
            Options&& options,
            Arguments&& arguments
        ) :
            command_impl<Callable, Options, Arguments>(
                name,
                description,
                std::move(fn),
                std::move(options),
                std::move(arguments)
            ),
            version(version)
        {}

        auto on_error(error_handler_t handler) -> void {
            error_handler = handler;
        }

        auto run(
            int argc,
            char** argv,
            std::ostream& out = std::cout
        ) -> int {
            const auto args = commline::argv(argv, argc);

            auto first = args.begin();
            const auto last = args.end();

            const auto argv0 = *(first++);
            auto cmd = this->find(first, last);

            try {
                cmd->execute({
                        this->name,
                        version,
                        this->description,
                        argv0
                    },
                    commline::argv(first, last),
                    out
                );
            }
            catch (const std::system_error& ex) {
                error_handler(std::current_exception());
                return ex.code().value();
            }
            catch (...) {
                error_handler(std::current_exception());
                return EXIT_FAILURE;
            }

            return EXIT_SUCCESS;
        };
    };

    template <typename Callable, typename Options, typename Arguments>
    auto application(
        std::string_view name,
        std::string_view version,
        std::string_view description,
        Options&& options,
        Arguments&& arguments,
        Callable&& fn
    ) -> app_impl<Callable, Options, Arguments> {
        return app_impl<Callable, Options, Arguments>(
            name,
            version,
            description,
            std::move(fn),
            std::move(options),
            std::move(arguments)
        );
    }
}
