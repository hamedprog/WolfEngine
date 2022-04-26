#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp> // NOLINT

#include <wolf.hpp>

TEST_CASE("w_memory tests", "[single-file]")
{
    using w_string = wolf::system::memory::w_string;
    auto _str = w_string("hello");

    REQUIRE(_str == "hello"); // NOLINT
}