#include "test.h"

#include <commline/argv.h>
#include <commline/command.h>

class CommandTest : public testing::Test {
protected:
    static constexpr auto description = "a test command"sv;
    static constexpr auto app_info = commline::app {
        "testapp",
        "0.0.0",
        "Unit tests.",
        "/app"
    };

    std::vector<std::string> arguments;
    std::ostringstream out;

    auto make_argv(std::initializer_list<std::string> il) -> commline::argv {
        arguments = std::vector<std::string>(il);

        auto argv = commline::argv();
        for (const auto& arg : arguments) argv.push_back(arg);

        return argv;
    }
};

TEST_F(CommandTest, Execution) {
    commline::command(
        "name",
        description,
        commline::options(commline::flag({"foo"}, "")),
        [](
            const commline::app& app,
            const commline::argv& argv,
            bool foo
        ) {
            ASSERT_EQ(app_info.name, app.name);
            ASSERT_EQ(app_info.version, app.version);
            ASSERT_EQ(app_info.description, app.description);
            ASSERT_EQ(app_info.argv0, app.argv0);

            ASSERT_TRUE(foo);
            ASSERT_EQ(1, argv.size());
            ASSERT_EQ("hello"sv, argv[0]);
        }
    )->execute(app_info, make_argv({"--foo", "hello"}), out);
}

TEST_F(CommandTest, Subcommand) {
    constexpr auto argument = "argument";
    constexpr auto cmd = "cmd";

    auto root = commline::command(
        "root",
        description,
        [](
            const commline::app& app,
            const commline::argv& argv
        ) {
            FAIL() << "Command should not run.";
        }
    );

    auto child = commline::command(
        cmd,
        description,
        [argument](
            const commline::app& app,
            const commline::argv& argv
        ) {
            ASSERT_EQ(1, argv.size());
            ASSERT_EQ(argument, argv[0]);
        }
    );

    root->subcommand(std::move(child));

    auto argv = make_argv({cmd, argument});

    auto it = argv.begin();
    auto end = argv.end();

    auto command = root->find(it, end);

    command->execute(app_info, commline::argv(it, end), out);
}

TEST_F(CommandTest, Help) {
    commline::command(
        "foo",
        description,
        [](
            const commline::app& app,
            const commline::argv& argv
        ) {
            FAIL() << "Command should not execute";
        }
    )->execute(app_info, make_argv({"--help"}), out);

    const auto result = out.str();

    ASSERT_EQ("foo: a test command\n"sv, result);
}

TEST_F(CommandTest, HelpOptions) {
    commline::command(
        "foo",
        description,
        commline::options(
            commline::flag({"bar"}, "A flag named bar"),
            commline::option<std::string_view>(
                {"h", "hello"},
                "A friendly greeting",
                "world"
            ),
            commline::option<std::string_view>(
                {"l", "really-long-option-name"},
                "A description on the next line",
                "value"
            )
        ),
        [](
            const commline::app& app,
            const commline::argv& argv,
            bool bar,
            std::string_view hello,
            std::string_view long_opt
        ) {
            FAIL() << "Command should not execute";
        }
    )->execute(app_info, make_argv({"--help"}), out);

    const auto result = out.str();

    ASSERT_EQ(
R"(foo: a test command

Options:
    --bar                         A flag named bar
    -h, --hello world             A friendly greeting
    -l, --really-long-option-name value
                                  A description on the next line
)",
        result
    );
}

TEST_F(CommandTest, HelpCommands) {
    auto root = commline::command(
        "foo",
        description,
        [](
            const commline::app& app,
            const commline::argv& argv
        ) {
            FAIL() << "Command should not execute";
        }
    );

    root->subcommand(commline::command(
        "bar",
        "A second test command",
        [](
            const commline::app& app,
            const commline::argv& argv
        ) {
            FAIL() << "Command should not execute";
        }
    ));

    root->execute(app_info, make_argv({"--help"}), out);

    const auto result = out.str();

    ASSERT_EQ(
R"(foo: a test command

Commands:
    bar            A second test command
)",
        result
    );
}

TEST_F(CommandTest, HelpMixed) {
    auto root = commline::command(
        "foo",
        description,
        commline::options(
            commline::flag({"bar"}, "A flag named bar"),
            commline::option<std::string_view>(
                {"h", "hello"},
                "A friendly greeting",
                "world"
            )
        ),
        [](
            const commline::app& app,
            const commline::argv& argv,
            bool bar,
            std::string_view hello
        ) {
            FAIL() << "Command should not execute";
        }
    );

    root->subcommand(commline::command(
        "bar",
        "A second test command",
        [](
            const commline::app& app,
            const commline::argv& argv
        ) {
            FAIL() << "Command should not execute";
        }
    ));

    root->execute(app_info, make_argv({"--help"}), out);

    const auto result = out.str();

    ASSERT_EQ(
R"(foo: a test command

Options:
    --bar                         A flag named bar
    -h, --hello world             A friendly greeting

Commands:
    bar            A second test command
)",
        result
    );
}
