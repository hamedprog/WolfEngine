/*
    Project: Wolf Engine. Copyright © 2014-2022 Pooya Eimandar
    https://github.com/WolfEngine/WolfEngine
*/

module;

#include <chrono>
#include <iostream>

export module wolf.system.gametime;

constexpr auto MAX_DELTA_TIME_IN_SECS = 60000.0; //60 minutes
constexpr auto SIXTY_FPS_MILLISECONDS_TICK = 16;

export struct gametime {
public:
  // default constructor
  gametime() = default;

  // destructor
  virtual ~gametime() = default;

  void reset() noexcept {
    this->last_time = std::chrono::steady_clock::now();
    this->left_over_ticks = 0.0;
    this->fps = 0;
    this->frames_this_sec = 0;
    this->secs_counter = 0.0;
  }

  [[nodiscard]] constexpr auto get_elapsed_secs() noexcept {
    return this->elapsed_secs;
  }

  [[nodiscard]] constexpr auto get_total_secs() noexcept {
    return this->total_secs;
  }

  [[nodiscard]] auto get_frames_count() const noexcept {
    return this->frames_count;
  }

  [[nodiscard]] auto get_fps() const noexcept {
    return this->fps;
  }

  [[nodiscard]] auto get_fixed_time_step() const noexcept {
    return this->fixed_time_step;
  }

  void set_fixed_time_step(_In_ bool p_value) {
    this->fixed_time_step = p_value;
  }

  void set_target_elapsed_secs(_In_ double p_value) {
    this->target_elapsed_secs = p_value;
  }

  template <typename F> 
  void tick(F &&p_tick_function) {
    using namespace std::chrono;

    // Query the current time.
    auto current_time = std::chrono::steady_clock::now();
    auto delta = std::chrono::duration<double, std::milli>(current_time -
                                                           this->last_time)
                     .count() /
                 1000.000;

    this->last_time = current_time;
    this->secs_counter += delta;

    // clamp excessively large time deltas (e.g. after paused in the debugger).
    if (delta > MAX_DELTA_TIME_IN_SECS) {
      delta = MAX_DELTA_TIME_IN_SECS;
    }

    auto last_frames_count = this->frames_count;
    if (this->fixed_time_step) {
      /*
         If the app is running very close to the target elapsed time (within
         1/4 of a millisecond (400 microseconds)) just clamp the clock to exactly match the target
         value. This prevents tiny and irrelevant errors from accumulating over
         time. Without this clamping, a game that requested a 60 fps fixed
         update, running with vsync enabled on a 59.94 NTSC display, would
         eventually accumulate enough tiny errors that it would drop a frame. It
         is better to just round small deviations down to zero to leave things
         running smoothly.
      */
      constexpr auto quarter_of_one_milli_in_sec = 0.0004;
      if (std::abs(delta - this->target_elapsed_secs) <
          quarter_of_one_milli_in_sec) {
        delta = this->target_elapsed_secs;
      }

      this->left_over_ticks += delta;
      this->elapsed_secs = this->target_elapsed_secs;

      while (this->left_over_ticks >= this->target_elapsed_secs) {

        this->total_secs += this->target_elapsed_secs;
        this->left_over_ticks -= this->target_elapsed_secs;

        this->frames_count++;
        p_tick_function();
      }
    } else {
      // variable timestep update logic.
      this->left_over_ticks = 0.0;
      this->total_secs += delta;
      this->elapsed_secs = delta;
      this->frames_count++;
      p_tick_function();
    }

    // track the current framerate.
    this->frames_this_sec += (this->frames_count - last_frames_count);

    auto one_sec = 1.0;
    if (this->secs_counter >= one_sec) {
      this->fps = this->frames_this_sec;
      this->frames_this_sec = 0;
      this->secs_counter = std::modf(this->secs_counter, &one_sec);
    }
  }

private:

  // configuring fixed timestep mode.
  bool fixed_time_step = false;

  double target_elapsed_secs = {1.0 / 60.0};
  double elapsed_secs = 0.0;
  double total_secs = 0.0;
  double left_over_ticks = 0.0;
  double secs_counter = 0.0;
  uint32_t fps = 0;
  uint32_t frames_count = 0;
  uint32_t frames_this_sec = 0;

  std::chrono::steady_clock::time_point last_time = {
      std::chrono::steady_clock::now()};
}
#ifdef __clang__
__attribute__((packed)) __attribute__((aligned(64)))
#endif
;