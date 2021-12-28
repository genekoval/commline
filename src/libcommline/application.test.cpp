#include "test.h"

#include <commline/application.h>

#include <string_view>

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

    ASSERT_EQ(0, commline::application(
        name,
        version,
        description,
        [this](
            const commline::app& app,
            const commline::argv& argv
        ) {
            assert_app_info(app);
        }
    ).run(argc, argv));
}

TEST_F(ApplicationTest, MulipleCommands) {
    const int argc = 5;
    const char* argv[argc] = {
        "./app", "start", "--fork", "--threads", "4"
    };

    auto application = commline::application(
        name,
        version,
        description,
        [](
            const commline::app& app,
            const commline::argv& argv
        ) {
            FAIL() << "Command should not run.";
        }
    );

    application.subcommand(commline::command(
        "start",
        "Start the server.",
        commline::options(
            commline::flag(
                {"fork"},
                "Fork the process."
            ),
            commline::option<int>(
                {"threads"},
                "Number of threads.",
                "count"
            )
        ),
        [this](
            const commline::app& app,
            const commline::argv& argv,
            bool fork,
            int threads
        ) {
            assert_app_info(app);

            ASSERT_TRUE(fork);
            ASSERT_EQ(4, threads);
        }
    ));

    ASSERT_EQ(0, application.run(argc, argv));
}
