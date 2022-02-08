#include "test.h"

#include <commline/option_list.h>

#include <algorithm>
#include <functional>

class ParameterListTest : public testing::Test {
protected:
    std::vector<std::string> argv;
    std::vector<std::string> arguments;
    std::function<void(std::string_view)> add_arg = [this](auto arg) {
        arguments.emplace_back(arg);
    };

    template <typename ...Options>
    auto options(Options&&... opts) -> commline::option_list<Options...> {
        return commline::option_list<Options...>(
            commline::options(std::forward<Options>(opts)...)
        );
    }

    template <typename OptionList, typename ...Args>
    auto parse(OptionList& list, Args&&... args) -> void {
        arguments.clear();
        argv = std::vector<std::string>({args...});
        list.parse(argv.begin(), argv.end(), add_arg);
    }
};

TEST_F(ParameterListTest, NoOptions) {
    auto list = options(commline::flag({"hello"}, ""));

    parse(list);

    ASSERT_FALSE(list.get<0>());
    ASSERT_TRUE(arguments.empty());
}

TEST_F(ParameterListTest, ArgumentsOnly) {
    auto list = options(commline::flag({"hello", "h"}, ""));

    parse(list, "hello", "world", "h");

    ASSERT_FALSE(list.get<0>());
    ASSERT_EQ(3, arguments.size());
    ASSERT_EQ("hello", arguments[0]);
    ASSERT_EQ("world", arguments[1]);
    ASSERT_EQ("h", arguments[2]);
}

TEST_F(ParameterListTest, LongOption) {
    auto list = options(commline::flag({"hello"}, ""));

    ASSERT_FALSE(list.get<0>());

    parse(list, "--hello");

    ASSERT_TRUE(list.get<0>());
}

TEST_F(ParameterListTest, ShortOption) {
    auto list = options(commline::flag({"h"}, ""));

    ASSERT_FALSE(list.get<0>());
    parse(list, "-h");
    ASSERT_TRUE(list.get<0>());
}

TEST_F(ParameterListTest, ShortFlagSequence) {
    auto list = options(
        commline::flag({"a"}, ""),
        commline::flag({"b"}, ""),
        commline::flag({"c"}, ""),
        commline::flag({"d"}, "")
    );

    parse(list, "-abd");

    ASSERT_TRUE(list.get<0>());
    ASSERT_TRUE(list.get<1>());
    ASSERT_FALSE(list.get<2>());
    ASSERT_TRUE(list.get<3>());
}

TEST_F(ParameterListTest, MixedParams) {
    auto list = options(
        commline::flag({"hello", "h"}, ""),
        commline::flag({"version", "v"}, ""),
        commline::flag({"a"}, ""),
        commline::flag({"b"}, ""),
        commline::flag({"c"}, "")
    );

    const auto hello = "hello"s;
    const auto world = "world"s;

    parse(list, hello, "--hello", "-v", world, "-abc");

    for (auto value : {
        list.get<0>(),
        list.get<1>(),
        list.get<2>(),
        list.get<3>(),
        list.get<4>()
    }) ASSERT_TRUE(value);

    ASSERT_EQ(2, arguments.size());
    ASSERT_EQ(hello, arguments[0]);
    ASSERT_EQ(world, arguments[1]);
}

TEST_F(ParameterListTest, EndOfOptions) {
    auto list = options(
        commline::flag({"hello"}, ""),
        commline::flag({"version"}, "")
    );
    parse(list, "--hello", "--", "--version", "-world");

    ASSERT_TRUE(list.get<0>());
    ASSERT_FALSE(list.get<1>());

    ASSERT_EQ(2, arguments.size());
    ASSERT_EQ("--version", arguments[0]);
    ASSERT_EQ("-world", arguments[1]);
}

TEST_F(ParameterListTest, ValueOptionLong) {
    auto list = options(commline::option<std::string_view>({"name"}, "", ""));

    parse(list, "--name", "commline", "hello");

    ASSERT_EQ("commline", list.get<0>());
    ASSERT_EQ(1, arguments.size());
    ASSERT_EQ("hello", arguments[0]);
}

TEST_F(ParameterListTest, LongOptionEquals) {
    auto list = options(commline::option<std::string_view>({"name"}, "", ""));

    parse(list, "--name=commline");

    ASSERT_EQ("commline", list.get<0>());
}

TEST_F(ParameterListTest, LongOptionEqualsMissingValue) {
    auto list = options(commline::option<std::string_view>({"name"}, "", ""));

    try {
        parse(list, "--name=");
        FAIL() << "option value missing";
    }
    catch (const commline::cli_error& ex) {
        ASSERT_EQ("missing value for: name"s, ex.what());
    }
}

TEST_F(ParameterListTest, FlagEquals) {
    auto list = options(commline::flag({"version"}, ""));

    try {
        parse(list, "--version=foo");
        FAIL() << "flag given a value";
    }
    catch (const commline::cli_error& ex) {
        ASSERT_EQ("option 'version' does not support values"s, ex.what());
    }
}

TEST_F(ParameterListTest, ValueOptionShort) {
    auto list = options(commline::option<std::string_view>({"n"}, "", ""));

    parse(list, "-n", "commline", "hello");

    ASSERT_EQ("commline", list.get<0>());
    ASSERT_EQ(1, arguments.size());
    ASSERT_EQ("hello", arguments[0]);
}

TEST_F(ParameterListTest, UnknownOption) {
    auto list = options(commline::flag({"hello", "h"}, ""));

    try {
        parse(list, "--version");
        FAIL() << "Option 'version' does not exist.";
    }
    catch (const commline::cli_error& ex) {
        ASSERT_EQ("unknown option: version"s, ex.what());
    }

    try {
        parse(list, "-hello");
        FAIL() << "Parsed as a sequnece of short options.";
    }
    catch (const commline::cli_error& ex) {
        ASSERT_EQ("unknown option: e"s, ex.what());
    }
}

TEST_F(ParameterListTest, MissingValue) {
    auto list = options(
        commline::option<std::string_view>({"name", "n"}, "", ""),
        commline::flag({"v"}, "")
    );

    try {
        parse(list, "--name");
        FAIL() << "No name was given.";
    }
    catch (const commline::cli_error& ex) {
        ASSERT_EQ("missing value for: name"s, ex.what());
    }

    try {
        parse(list, "-n");
        FAIL() << "No name was given.";
    }
    catch (const commline::cli_error& ex) {
        ASSERT_EQ("missing value for: n"s, ex.what());
    }

    try {
        parse(list, "-nv", "hello");
        FAIL() << "No name was given.";
    }
    catch (const commline::cli_error& ex) {
        ASSERT_EQ("missing value for: n"s, ex.what());
    }
}

TEST_F(ParameterListTest, SequenceValue) {
    auto list = options(
        commline::flag({"create", "c"}, ""),
        commline::option<std::string_view>({"file", "f"}, "", ""),
        commline::flag({"verbose", "v"}, "")
    );

    const auto config = "/home/commline/Documents"s;

    parse(list, "-cvf", config);

    ASSERT_TRUE(list.get<0>());
    ASSERT_EQ(config, list.get<1>());
    ASSERT_TRUE(list.get<2>());
}

TEST_F(ParameterListTest, EmptyList) {
    auto list = options(
        commline::list<std::string_view>({"include"}, "", "")
    );

    parse(list, "");

    ASSERT_TRUE(list.get<0>().empty());
}

TEST_F(ParameterListTest, ListOne) {
    auto list = options(
        commline::list<std::string_view>({"include"}, "", "")
    );

    parse(list, "--include", "foo");
    const auto result = list.get<0>();

    ASSERT_EQ(1, result.size());
    ASSERT_EQ("foo", result.front());
}

TEST_F(ParameterListTest, ListMany) {
    auto list = options(
        commline::list<std::string_view>({"include"}, "", "")
    );

    parse(list, "--include=foo", "--include=bar");
    const auto result = list.get<0>();

    ASSERT_EQ(2, result.size());
    ASSERT_EQ("foo", result[0]);
    ASSERT_EQ("bar", result[1]);
}

TEST_F(ParameterListTest, ListNoValue) {
    auto list = options(
        commline::list<std::string_view>({"include"}, "", "")
    );

    try {
        parse(list, "--include");
        FAIL() << "No value provided";
    }
    catch (const commline::cli_error& ex) {
        ASSERT_EQ("missing value for: include"s, ex.what());
    }
}
