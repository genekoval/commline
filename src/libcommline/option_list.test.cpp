#include <test.h>

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
    auto make_list(
        Options&&... options
    ) -> commline::option_list<Options...> {
        return commline::option_list<Options...>(options...);
    }

    template <typename OptionList, typename ...Args>
    auto parse(OptionList& list, Args&&... args) -> void {
        arguments.clear();
        argv = std::vector<std::string>({args...});
        list.parse(argv.begin(), argv.end(), add_arg);
    }
};

TEST_F(ParameterListTest, NoOptions) {
    auto list = make_list(commline::flag({"help"}, ""));

    parse(list);

    ASSERT_FALSE(list.get<0>());
    ASSERT_TRUE(arguments.empty());
}

TEST_F(ParameterListTest, ArgumentsOnly) {
    auto list = make_list(commline::flag({"hello", "h"}, ""));

    parse(list, "hello", "world", "h");

    ASSERT_FALSE(list.get<0>());
    ASSERT_EQ(3, arguments.size());
    ASSERT_EQ("hello", arguments[0]);
    ASSERT_EQ("world", arguments[1]);
    ASSERT_EQ("h", arguments[2]);
}

TEST_F(ParameterListTest, LongOption) {
    auto list = make_list(commline::flag({"help"}, ""));

    ASSERT_FALSE(list.get<0>());

    parse(list, "--help");

    ASSERT_TRUE(list.get<0>());
}

TEST_F(ParameterListTest, ShortOption) {
    auto list = make_list(commline::flag({"h"}, ""));

    ASSERT_FALSE(list.get<0>());
    parse(list, "-h");
    ASSERT_TRUE(list.get<0>());
}

TEST_F(ParameterListTest, ShortFlagSequence) {
    auto list = make_list(
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
    auto list = make_list(
        commline::flag({"help", "h"}, ""),
        commline::flag({"version", "v"}, ""),
        commline::flag({"a"}, ""),
        commline::flag({"b"}, ""),
        commline::flag({"c"}, "")
    );

    const auto hello = "hello"s;
    const auto world = "world"s;

    parse(list, hello, "--help", "-v", world, "-abc");

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
    auto list = make_list(
        commline::flag({"help"}, ""),
        commline::flag({"version"}, "")
    );
    parse(list, "--help", "--", "--version", "-world");

    ASSERT_TRUE(list.get<0>());
    ASSERT_FALSE(list.get<1>());

    ASSERT_EQ(2, arguments.size());
    ASSERT_EQ("--version", arguments[0]);
    ASSERT_EQ("-world", arguments[1]);
}

TEST_F(ParameterListTest, ValueOptionLong) {
    auto list = make_list(commline::option<std::string_view>({"name"}, "", ""));

    parse(list, "--name", "commline");

    ASSERT_EQ("commline", list.get<0>());
}

TEST_F(ParameterListTest, ValueOptionShort) {
    auto list = make_list(commline::option<std::string_view>({"n"}, "", ""));

    parse(list, "-n", "commline");

    ASSERT_EQ("commline", list.get<0>());
}

TEST_F(ParameterListTest, UnknownOption) {
    auto list = make_list(commline::flag({"help", "h"}, ""));

    try {
        parse(list, "--version");
        FAIL() << "Option 'version' does not exist.";
    }
    catch (const commline::cli_error& ex) {
        ASSERT_EQ("unknown option: version"s, ex.what());
    }

    try {
        parse(list, "-help");
        FAIL() << "Parsed as a sequnece of short options.";
    }
    catch (const commline::cli_error& ex) {
        ASSERT_EQ("unknown option: e"s, ex.what());
    }
}

TEST_F(ParameterListTest, MissingValue) {
    auto list = make_list(
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
    auto list = make_list(
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
