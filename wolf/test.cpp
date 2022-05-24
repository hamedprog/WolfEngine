#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp> // NOLINT

import wolf;

#include <iostream>
#include <format>

TEST_CASE("gametime", "[single-file]") {

  auto gtime = gametime();
  gtime.set_fixed_time_step(true);
  gtime.set_target_elapsed_milliseconds(16); // ticks every 16 ms (60 fps)

  while (true) {
    gtime.tick([&]() {
      auto _msg = std::format(
          "elapsed seconds from last milliseconds {}. total elapsed milliseconds {}",
          gtime.get_elapsed_milliseconds(), gtime.get_total_milliseconds());
      std::cout << _msg << std::endl;
    });

    // break after 5 seconds
    if (gtime.get_total_milliseconds() > 5000) {
      break;
    }
  }
}