//#define CATCH_CONFIG_MAIN
//#include <catch2/catch.hpp> // NOLINT
//
//import wolf;
//import wolf.system;
//
//TEST_CASE("gametime tests", "[single-file]")
//{
//    wolf::w_version();
//
//
//    //REQUIRE(_str1 != _str2); // NOLINT
//}

#ifdef WOLF_ENABLE_MODULES
import wolf;
#else
#include <wolf.hpp>
#endif
#include <iostream>

auto main() -> int {

  std::cout <<  wolf::w_version() <<  std::endl;
  return 0;
}