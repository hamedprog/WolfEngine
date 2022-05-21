/*
    Project: Wolf Engine. Copyright © 2014-2022 Pooya Eimandar
    https://github.com/WolfEngine/WolfEngine
*/

module;

#include "wolf_api.h"

export module wolf.system.gametime;

// integer format represents time using 10,000,000 ticks per second.
constexpr double TICKS_PER_SECOND = 10000000;

double ticks_to_seconds(uint64_t pTicks) {
  return (double)pTicks / TICKS_PER_SECOND;
}

uint64_t seconds_to_ticks(double pSeconds) {
  return (uint64_t)(pSeconds * TICKS_PER_SECOND);
}

export class gametime {
public:
  // default constructor
  WOLF_API
  gametime() = default;

  // destructor
  WOLF_API
  virtual ~gametime() = default;

private:
  double last_time;
  double max_delta;

  // derived timing data uses a canonical tick format.
  uint64_t elapsed_ticks;
  uint64_t total_ticks;
  uint64_t left_over_ticks;

  // members for tracking the framerate.
  uint32_t frame_count;
  uint32_t fps;
  uint32_t frames_this_second;
  double seconds_counter;

  // members for configuring fixed timestep mode.
  bool fixed_time_step;
  uint64_t target_elapsed_ticks;
};


