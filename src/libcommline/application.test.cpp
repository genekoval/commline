#include "test.h"

#include <commline/application.h>

#include <string_view>

using commline::application;
using commline::arguments;
using commline::command;
using commline::flag;
using commline::option;
using commline::options;

class ApplicationTest : public testing::Test {
protected:
    static constexpr auto name = "myapp"sv;
    static constexpr auto version = "0.0.0"sv;
    static constexpr auto description = "Test application."sv;
    static constexpr auto executable = "./app"sv;

    auto assert_app_info(const commline::app& app) -> void {
        ASSERT_EQ(name, app.name);
        ASSERT_EQ(version, app.version);
        ASSERT_EQ(description, app.description);
        ASSERT_EQ(executable, app.argv0);
    }
};

TEST_F(ApplicationTest, Create) {
    const int argc = 1;
    const char* argv[argc] = { "./app" };

    ASSERT_EQ(0, application(
        name,
        version,
        description,
        options(),
        arguments(),
        [this](const commline::app& app) { assert_app_info(app); }
    ).run(argc, const_cast<char**>(argv)));
}

TEST_F(ApplicationTest, MulipleCommands) {
    const int argc = 5;
    const char* argv[argc] = {
        "./app", "start", "--fork", "--threads", "4"
    };

    auto app = application(
        name,
        version,
        description,
        options(),
        arguments(),
        [](const commline::app& app) { FAIL() << "Command should not run."; }
    );

    app.subcommand(command(
        "start",
        "Start the server.",
        options(
            flag(
                {"fork"},
                "Fork the process."
            ),
            option<int>(
                {"threads"},
                "Number of threads.",
                "count"
            )
        ),
        arguments(),
        [this](const commline::app& app, bool fork, int threads) {
            assert_app_info(app);

            ASSERT_TRUE(fork);
            ASSERT_EQ(4, threads);
        }
    ));

    ASSERT_EQ(0, app.run(argc, const_cast<char**>(argv)));
}
