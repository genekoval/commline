#include "test.h"

#include <commline/arguments.h>

using commline::cli_error;
using commline::optional;
using commline::required;
using commline::variadic;

class ArgumentTest : public testing::Test {
protected:
    template <typename ...Arguments>
    auto arguments(
        Arguments&&... args
    ) -> commline::positional_arguments<Arguments...> {
        return commline::positional_arguments<Arguments...>(
            commline::arguments(std::forward<Arguments>(args)...)
        );
    }
};

TEST_F(ArgumentTest, RequiredArgument) {
    auto parser = arguments(required<std::string_view>("hello"));
    const auto [result] = parser.parse({"world"});

    ASSERT_EQ("world", result);
}

TEST_F(ArgumentTest, RequiredArgumentMissing) {
    auto parser = arguments(required<std::string_view>("hello"));

    try {
        parser.parse({});
        FAIL() << "Parsing should have failed";
    }
    catch (const cli_error& ex) {
        ASSERT_EQ(
            "not enough arguments: missing value for: hello"s,
            ex.what()
        );
    }
}

TEST_F(ArgumentTest, OptionalArgument) {
    auto parser = arguments(optional<std::string_view>("hello"));
    const auto [result] = parser.parse({"world"});

    ASSERT_TRUE(result.has_value());
    ASSERT_EQ("world", *result);
}

TEST_F(ArgumentTest, OptionalArgumentMissing) {
    auto parser = arguments(optional<std::string_view>("hello"));
    const auto [result] = parser.parse({});

    ASSERT_FALSE(result.has_value());
}

TEST_F(ArgumentTest, VariadicEmpty) {
    auto parser = arguments(variadic<std::string_view>("foo"));
    const auto [result] = parser.parse({});

    ASSERT_TRUE(result.empty());
}

TEST_F(ArgumentTest, VariadicOne) {
    auto parser = arguments(variadic<std::string_view>("foo"));
    const auto [result] = parser.parse({"bar"});

    ASSERT_EQ(1, result.size());
    ASSERT_EQ("bar", result.front());
}

TEST_F(ArgumentTest, VariadicMultiple) {
    auto parser = arguments(variadic<std::string_view>("foo"));
    const auto [result] = parser.parse({"bar", "baz"});

    ASSERT_EQ(2, result.size());
    ASSERT_EQ("bar", result[0]);
    ASSERT_EQ("baz", result[1]);
}

TEST_F(ArgumentTest, VariadicLeading) {
    auto parser = arguments(
        variadic<int>("numbers"),
        required<std::string_view>("word")
    );
    const auto [numbers, word] = parser.parse({"2", "4", "8", "foo"});

    ASSERT_EQ(3, numbers.size());
    ASSERT_EQ(2, numbers[0]);
    ASSERT_EQ(4, numbers[1]);
    ASSERT_EQ(8, numbers[2]);
    ASSERT_EQ("foo", word);
}

TEST_F(ArgumentTest, VariadicCenter) {
    auto parser = arguments(
        required<std::string_view>("head"),
        variadic<std::string_view>("numbers"),
        required<std::string_view>("tail")
    );
    const auto [head, numbers, tail] = parser.parse({
        "foo", "one", "two", "three", "bar"
    });

    ASSERT_EQ("foo", head);
    ASSERT_EQ(3, numbers.size());
    ASSERT_EQ("one", numbers[0]);
    ASSERT_EQ("two", numbers[1]);
    ASSERT_EQ("three", numbers[2]);
    ASSERT_EQ("bar", tail);
}

TEST_F(ArgumentTest, VariadicTrailing) {
    auto parser = arguments(
        required<std::string_view>("word"),
        variadic<int>("numbers")
    );
    const auto [word, numbers] = parser.parse({"foo", "5", "10", "15"});

    ASSERT_EQ("foo", word);
    ASSERT_EQ(3, numbers.size());
    ASSERT_EQ(5, numbers[0]);
    ASSERT_EQ(10, numbers[1]);
    ASSERT_EQ(15, numbers[2]);
}
