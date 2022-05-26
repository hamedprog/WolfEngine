#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp> // NOLINT

import wolf;

#include <iostream>
#include <format>

TEST_CASE("gametime", "[single-file]") {

  auto gtime = gametime();
  gtime.set_fixed_time_step(true);

  while (true) {
    gtime.tick([&]() {
      auto _msg = std::format("elapsed time {} total time {} fps: {}",
                              gtime.get_elapsed_secs(), gtime.get_total_secs(),
                              gtime.get_fps());
      std::cout << _msg << std::endl;
    });

    // break after 5 seconds
    if (gtime.get_total_secs() > 5.0) {
      break;
    }
  }
}