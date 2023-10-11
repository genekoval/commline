#include "test.h"

#include <commline/command.h>

using commline::arguments;
using commline::command;
using commline::flag;
using commline::option;
using commline::options;
using commline::required;
using commline::variadic;

class CommandTest : public testing::Test {
protected:
    static constexpr auto description = "a test command"sv;
    static constexpr auto app_info =
        commline::app {"testapp", "0.0.0", "Unit tests.", "/app"};
    static constexpr auto help = std::array {"--help"};

    std::ostringstream out;
};

TEST_F(CommandTest, Execution) {
    constexpr auto args = std::array {"--foo", "hello"};

    command(
        "name",
        description,
        options(flag({"foo"}, "")),
        arguments(required<std::string_view>("greeting")),
        [](const commline::app& app, bool foo, std::string_view greeting) {
            ASSERT_EQ(app_info.name, app.name);
            ASSERT_EQ(app_info.version, app.version);
            ASSERT_EQ(app_info.description, app.description);
            ASSERT_EQ(app_info.argv0, app.argv0);

            ASSERT_TRUE(foo);
            ASSERT_EQ("hello"s, greeting);
        }
    )->execute(app_info, args, out);
}

TEST_F(CommandTest, Subcommand) {
    constexpr auto argument = "argument";
    constexpr auto cmd = "cmd";
    constexpr auto args = std::array {cmd, argument};
    const auto argv = commline::argv(args);

    auto root = command(
        "root",
        description,
        options(),
        arguments(),
        [](const commline::app& app) { FAIL() << "Command should not run."; }
    );

    auto child = commline::command(
        cmd,
        description,
        options(),
        arguments(required<std::string_view>("foo")),
        [argument](const commline::app& app, std::string_view foo) {
            ASSERT_EQ(argument, foo);
        }
    );

    root->subcommand(std::move(child));

    auto it = argv.begin();
    const auto end = argv.end();

    auto command = root->find(it, end);

    command->execute(app_info, commline::argv(it, end), out);
}

TEST_F(CommandTest, Help) {
    commline::command(
        "foo",
        description,
        options(),
        arguments(),
        [](const commline::app& app) { FAIL() << "Command should not execute"; }
    )->execute(app_info, help, out);

    const auto result = out.str();

    ASSERT_EQ(
        R"(a test command

Usage: foo
)",
        result
    );
}

TEST_F(CommandTest, HelpOptions) {
    command(
        "foo",
        description,
        options(
            flag({"bar"}, "A flag named bar"),
            option<std::string_view>(
                {"h", "hello"},
                "A friendly greeting",
                "world"
            ),
            option<std::string_view>(
                {"l", "really-long-option-name"},
                "A description on the next line",
                "value"
            )
        ),
        arguments(),
        [](const commline::app& app,
           bool bar,
           std::string_view hello,
           std::string_view long_opt) {
            FAIL() << "Command should not execute";
        }
    )->execute(app_info, help, out);

    const auto result = out.str();

    ASSERT_EQ(
        R"(a test command

Usage: foo [options]

Options:
    --bar                         A flag named bar
    -h, --hello world             A friendly greeting
    -l, --really-long-option-name value
                                  A description on the next line
)",
        result
    );
}

TEST_F(CommandTest, HelpArguments) {
    command(
        "foo",
        description,
        options(),
        arguments(required<std::string_view>("bar"), variadic<int>("baz")),
        [](const commline::app& app,
           std::string_view bar,
           const std::vector<int>& baz) {
            FAIL() << "Command should not execute";
        }
    )->execute(app_info, help, out);

    const auto result = out.str();

    ASSERT_EQ(
        R"(a test command

Usage: foo BAR BAZ...
)",
        result
    );
}

TEST_F(CommandTest, HelpCommands) {
    auto root = command(
        "foo",
        description,
        options(),
        arguments(),
        [](const commline::app& app) { FAIL() << "Command should not execute"; }
    );

    root->subcommand(command(
        "bar",
        "A second test command",
        options(),
        arguments(),
        [](const commline::app& app) { FAIL() << "Command should not execute"; }
    ));

    root->execute(app_info, help, out);

    const auto result = out.str();

    ASSERT_EQ(
        R"(a test command

Usage: foo

Commands:
    bar            A second test command
)",
        result
    );
}

TEST_F(CommandTest, HelpMixed) {
    auto root = command(
        "foo",
        description,
        options(
            commline::flag({"bar"}, "A flag named bar"),
            commline::option<std::string_view>(
                {"h", "hello"},
                "A friendly greeting",
                "world"
            )
        ),
        arguments(variadic<std::string_view>("baz"), required<int>("number")),
        [](const commline::app& app,
           bool bar,
           std::string_view hello,
           const std::vector<std::string_view>& baz,
           int number) { FAIL() << "Command should not execute"; }
    );

    root->subcommand(command(
        "bar",
        "A second test command",
        options(),
        arguments(),
        [](const commline::app& app) { FAIL() << "Command should not execute"; }
    ));

    root->execute(app_info, help, out);

    const auto result = out.str();

    ASSERT_EQ(
        R"(a test command

Usage: foo [options] [--] BAZ... NUMBER

Options:
    --bar                         A flag named bar
    -h, --hello world             A friendly greeting

Commands:
    bar            A second test command
)",
        result
    );
}
