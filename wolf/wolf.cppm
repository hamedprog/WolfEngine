/*
    Project: Wolf Engine. Copyright © 2014-2022 Pooya Eimandar
    https://github.com/WolfEngine/WolfEngine
*/
module;

#include <format>
#include <string>
#include <gsl/gsl> 

#pragma warning(push)
#pragma warning(disable : 4668)
//#pragma warning(disable : 4820)
#include <boost/pool/pool_alloc.hpp> // NOLINT
#pragma warning(pop)

#ifdef WOLF_SYSTEM_MIMALLOC
#include <mimalloc-new-delete.h> // NOLINT
#include <mimalloc-override.h> // NOLINT
#endif

#if defined(_WIN32) && !defined(WASM)

#ifndef NOMINMAX
#define NOMINMAX
#endif // NOMINMAX

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif // WIN32_LEAN_AND_MEAN

#include <Windows.h> // NOLINT

#endif // _WIN32

export module wolf;
export import wolf.system;
//export import wolf.system.execution;
export import wolf.system.gametime;

namespace wolf {

/**
 * returns wolf version
 * @return string format with the following style
 * "<major>.<minor>.<patch>.<debug>"
 */
export auto w_version() -> std::string {

  //std::vector<int, boost::fast_pool_allocator<int>> vect;

  // Making incompatible API changes
  constexpr auto WOLF_MAJOR_VERSION = 3;
  // Adding functionality in a backwards - compatible manner
  constexpr auto WOLF_MINOR_VERSION = 0;
  // bug fixes
  constexpr auto WOLF_PATCH_VERSION = 0;
  // for debugging
  constexpr auto WOLF_DEBUG_VERSION = 0;

  auto ver = std::format("v{}.{}.{}.{}", WOLF_MAJOR_VERSION, WOLF_MINOR_VERSION,
                         WOLF_PATCH_VERSION, WOLF_DEBUG_VERSION);

  return std::string(ver);
}

} // namespace wolf
