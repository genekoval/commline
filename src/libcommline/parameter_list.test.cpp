#include <test.h>

#include <algorithm>
#include <functional>

class ParameterListTest : public testing::Test {
protected:
    std::vector<std::string> arguments;
    std::function<void(const std::string&)> add_arg = [this](auto arg) {
        arguments.push_back(arg);
    };

    template <typename... Parameters>
    auto make_list(
        Parameters&&... parameters
    ) -> commline::parameter_list<Parameters...> {
        return commline::parameter_list<Parameters...>(parameters...);
    }

    template <typename ParamList, typename... Args>
    auto parse(ParamList& list, Args&&... args) -> void {
        arguments.clear();
        const auto argv = std::array<std::string, sizeof...(Args)>{args...};
        list.parse(argv.begin(), argv.end(), add_arg);
    }
};

TEST_F(ParameterListTest, NoOptions) {
    auto list = make_list(commline::flag({"help"}, ""));
    auto& flag = std::get<0>(list.options());

    parse(list);

    ASSERT_FALSE(flag.value());
    ASSERT_TRUE(arguments.empty());
}

TEST_F(ParameterListTest, ArgumentsOnly) {
    auto list = make_list(commline::flag({"hello", "h"}, ""));
    auto& flag = std::get<0>(list.options());

    parse(list, "hello", "world", "h");

    ASSERT_FALSE(flag.value());
    ASSERT_EQ(3, arguments.size());
    ASSERT_EQ("hello", arguments[0]);
    ASSERT_EQ("world", arguments[1]);
    ASSERT_EQ("h", arguments[2]);
}

TEST_F(ParameterListTest, LongOption) {
    auto list = make_list(commline::flag({"help"}, ""));
    auto& flag = std::get<0>(list.options());

    ASSERT_FALSE(flag.value());

    parse(list, "--help");

    ASSERT_TRUE(flag.value());
}

TEST_F(ParameterListTest, ShortOption) {
    auto list = make_list(commline::flag({"h"}, ""));
    auto& flag = std::get<0>(list.options());

    ASSERT_FALSE(flag.value());
    parse(list, "-h");
    ASSERT_TRUE(flag.value());
}

TEST_F(ParameterListTest, ShortFlagSequence) {
    auto list = make_list(
        commline::flag({"a"}, ""),
        commline::flag({"b"}, ""),
        commline::flag({"c"}, ""),
        commline::flag({"d"}, "")
    );

    auto& a = std::get<0>(list.options());
    auto& b = std::get<1>(list.options());
    auto& c = std::get<2>(list.options());
    auto& d = std::get<3>(list.options());

    parse(list, "-abd");

    ASSERT_TRUE(a.value());
    ASSERT_TRUE(b.value());
    ASSERT_FALSE(c.value());
    ASSERT_TRUE(d.value());
}

TEST_F(ParameterListTest, MixedParams) {
    auto list = make_list(
        commline::flag({"help", "h"}, ""),
        commline::flag({"version", "v"}, ""),
        commline::flag({"a"}, ""),
        commline::flag({"b"}, ""),
        commline::flag({"c"}, "")
    );

    auto& h = std::get<0>(list.options());
    auto& v = std::get<0>(list.options());
    auto& a = std::get<0>(list.options());
    auto& b = std::get<0>(list.options());
    auto& c = std::get<0>(list.options());

    const auto hello = "hello"s;
    const auto world = "world"s;

    parse(list, hello, "--help", "-v", world, "-abc");

    for (auto opt : {h, v, a, b, c}) ASSERT_TRUE(opt.value());
    ASSERT_EQ(2, arguments.size());
    ASSERT_EQ(hello, arguments[0]);
    ASSERT_EQ(world, arguments[1]);
}

TEST_F(ParameterListTest, EndOfOptions) {
    auto list = make_list(
        commline::flag({"help"}, ""),
        commline::flag({"version"}, "")
    );
    const auto& help = std::get<0>(list.options());
    const auto& version = std::get<1>(list.options());

    parse(list, "--help", "--", "--version", "-world");

    ASSERT_TRUE(help.value());
    ASSERT_FALSE(version.value());

    ASSERT_EQ(2, arguments.size());
    ASSERT_EQ("--version", arguments[0]);
    ASSERT_EQ("-world", arguments[1]);
}

TEST_F(ParameterListTest, ValueOptionLong) {
    auto list = make_list(commline::string({"name"}, "", "", "default"));
    const auto& opt = std::get<0>(list.options());

    ASSERT_EQ("default", opt.value());

    parse(list, "--name", "commline");

    ASSERT_EQ("commline", opt.value());
}

TEST_F(ParameterListTest, ValueOptionShort) {
    auto list = make_list(commline::string({"n"}, "", "", "default"));
    const auto& opt = std::get<0>(list.options());

    ASSERT_EQ("default", opt.value());

    parse(list, "-n", "commline");

    ASSERT_EQ("commline", opt.value());
}

TEST_F(ParameterListTest, IntegerOption) {
    auto list = make_list(commline::integer({"jobs"}, "", "", 1));
    const auto& opt = std::get<0>(list.options());

    ASSERT_EQ(1, opt.value());

    parse(list, "--jobs", "4");

    ASSERT_EQ(4, opt.value());
}

TEST_F(ParameterListTest, NumberOption) {
    auto list = make_list(commline::number({"load-average"}, "", "", 1.5f));
    const auto& opt = std::get<0>(list.options());

    ASSERT_EQ(1.5f, opt.value());

    parse(list, "--load-average", "0.9");

    ASSERT_EQ(0.9f, opt.value());
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
        commline::string({"name", "n"}, "", "", "default"),
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
        commline::string({"file", "f"}, "", "", "/etc/commline/config"),
        commline::flag({"verbose", "v"}, "")
    );
    auto& create = std::get<0>(list.options());
    auto& file = std::get<1>(list.options());
    auto& verbose = std::get<2>(list.options());

    auto config = "/home/commline/.config"s;

    parse(list, "-cvf", config);

    ASSERT_TRUE(create.value());
    ASSERT_EQ(config, file.value());
    ASSERT_TRUE(verbose.value());
}
