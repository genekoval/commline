#include <commline/error.h>
#include <commline/parameter_types.h>

#include <gtest/gtest.h>
#include <limits>
#include <sstream>

using namespace std::literals;

class ParameterTypesTest : public testing::Test {
protected:
    std::vector<std::string> aliases;
    int default_int;
    std::string default_string;
    std::string description;
    std::string value_name;

    ParameterTypesTest() :
        aliases({"version", "v"}),
        default_int(2),
        default_string("Hello"),
        description("Print the version number."),
        value_name("test")
    {}
};

TEST_F(ParameterTypesTest, FlagParameter) {
    auto flag = commline::flag(aliases, description);

    ASSERT_EQ(aliases, flag.aliases);
    ASSERT_EQ(description, flag.description);

    ASSERT_FALSE(flag.value());

    flag.enable();

    ASSERT_TRUE(flag.value());
}

TEST_F(ParameterTypesTest, ValueParameter) {
    const auto new_value = "World"s;

    auto param = commline::string(
        aliases,
        description,
        value_name,
        default_string
    );

    ASSERT_EQ(value_name, param.value_name);
    ASSERT_EQ(default_string, param.value());

    param.set(new_value);
    ASSERT_EQ(new_value, param.value());
}

TEST_F(ParameterTypesTest, IntegerSuccess) {
    const auto value = "10"s;
    const auto parsed_value = 10;

    auto param = commline::integer(
        aliases,
        description,
        value_name,
        default_int
    );

    ASSERT_EQ(default_int, param.value());
    param.set(value);
    ASSERT_EQ(parsed_value, param.value());
}

TEST_F(ParameterTypesTest, IntegerInvalid) {
    const auto value = "bad"s;

    auto param = commline::integer(
        aliases,
        description,
        value_name,
        default_int
    );

    try {
        param.set(value);
        FAIL() << "Parsing should have failed.";
    }
    catch (const commline::cli_error& ex) {
        ASSERT_EQ("invalid integer: " + value, ex.what());
    }
}

TEST_F(ParameterTypesTest, IntegerOutOfRange) {
    auto param = commline::integer(
        aliases,
        description,
        value_name,
        default_int
    );

    const auto max = std::numeric_limits<commline::integer::value_type>::max();

    auto os = std::ostringstream();
    os << max;
    const auto max_str = os.str();

    param.set(max_str);
    ASSERT_EQ(max, param.value());

    os << "0";
    const auto oor = os.str(); // Out of range.

    try {
        param.set(oor);
        FAIL() << "Value should be out of range.";
    }
    catch (const commline::cli_error& ex) {
        ASSERT_EQ("integer argument out of range: " + oor, ex.what());
    }
}
