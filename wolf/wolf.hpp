#pragma once

#include <export.h>

#ifndef MODULE
#include <string>
#endif

namespace wolf {
/**
 * returns wolf version
 * @return string format with the following style
 * "<major>.<minor>.<patch>.<debug>"
 */

EXPORT auto w_version() -> std::string {

  // std::vector<int, boost::fast_pool_allocator<int>> vect;
  // vect.push_back(4);
  // vect.push_back(9);

  // Making incompatible API changes
  constexpr auto WOLF_MAJOR_VERSION = 3;
  // Adding functionality in a backwards - compatible manner
  constexpr auto WOLF_MINOR_VERSION = 0;
  // bug fixes
  constexpr auto WOLF_PATCH_VERSION = 0;
  // for debugging
  constexpr auto WOLF_DEBUG_VERSION = 0;

  // auto _ver = std::format(
  //     "v{}.{}.{}.{}", WOLF_MAJOR_VERSION, WOLF_MINOR_VERSION,
  //                 WOLF_PATCH_VERSION, WOLF_DEBUG_VERSION);

  // return std::string(_ver);
  return std::string("3");
}
} // namespace wolf
