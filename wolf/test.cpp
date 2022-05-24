#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp> // NOLINT

import wolf;

#include <iostream>

TEST_CASE("gametime tests", "[single-file]") {
  auto a = 10;
  auto gtime = gametime();
  gtime.tick([&]() { 

      std::cout << "tick: " << a << std::endl; 
      });

  INFO("Wolf version: " << wolf::w_version()); // NOLINT
                                               // REQUIRE( != "3.0.0.0");
}