/*
    Project: Wolf Engine. Copyright © 2014-2022 Pooya Eimandar
    https://github.com/WolfEngine/WolfEngine
*/

module;

#include <chrono>
#include <gsl/gsl>

export module wolf.system.gametime;

constexpr uint64_t MILLISECONDS_PER_SECOND = 1000;
constexpr uint64_t MAX_DELTA_TIME_IN_MILLISECONDS = 3600000; //60 minutes
constexpr uint64_t SIXTY_FPS = 60;

export struct gametime {
public:
  // default constructor
  gametime() = default;

  // destructor
  virtual ~gametime() = default;

  void reset() noexcept {
    this->last_time = std::chrono::steady_clock::now();
    this->left_over_milliseconds = std::chrono::milliseconds::zero();
    this->seconds_counter = std::chrono::milliseconds::zero();
    this->fps = 0;
    this->frames_this_second = 0;
  }

  [[nodiscard]] constexpr uint64_t get_elapsed_ticks() const noexcept {
    return this->elapsed_milliseconds.count();
  }

  [[nodiscard]] constexpr uint64_t get_total_ticks() const noexcept {
    return this->total_milliseconds.count();
  }

  [[nodiscard]] constexpr uint64_t get_elapsed_milliseconds() const noexcept {
    return this->elapsed_milliseconds.count();
  }

  [[nodiscard]] constexpr uint64_t get_total_milliseconds() const noexcept {
    return this->total_milliseconds.count();
  }

  [[nodiscard]] uint32_t get_frames_count() const noexcept {
    return this->frames_count;
  }

  [[nodiscard]] uint32_t get_frames_per_second() const noexcept {
    return this->fps;
  }

  [[nodiscard]] bool get_fixed_time_step() const noexcept {
    return this->fixed_time_step;
  }

  void set_fixed_time_step(_In_ bool p_value) {
    this->fixed_time_step = p_value;
  }

  void set_target_elapsed_milliseconds(_In_ uint64_t p_value) {
    this->target_elapsed_milliseconds = std::chrono::milliseconds(p_value);
  }

  template <typename F> void tick(F &&p_tick_function) {
    using namespace std::chrono;

    p_tick_function();

    // Query the current time.
    auto _current_time = std::chrono::steady_clock::now();
    auto _time_delta =
        std::chrono::duration_cast<milliseconds>(_current_time - this->last_time);

    this->last_time = _current_time;
    this->seconds_counter += _time_delta;

    // clamp excessively large time deltas (e.g. after paused in the debugger).
    if (_time_delta.count() > MAX_DELTA_TIME_IN_MILLISECONDS) {
      _time_delta = std::chrono::milliseconds(MAX_DELTA_TIME_IN_MILLISECONDS);
    }

    uint32_t _last_frame_count = this->frames_count;
    if (this->fixed_time_step) {
      /*
         If the app is running very close to the target elapsed time (within
         1/4 of a millisecond) just clamp the clock to exactly match the target
         value. This prevents tiny and irrelevant errors from accumulating over
         time. Without this clamping, a game that requested a 60 fps fixed
         update, running with vsync enabled on a 59.94 NTSC display, would
         eventually accumulate enough tiny errors that it would drop a frame. It
         is better to just round small deviations down to zero to leave things
         running smoothly.
      */

      if (abs(_time_delta.count() - this->target_elapsed_milliseconds.count()) <
          (MILLISECONDS_PER_SECOND / 4)) {
        _time_delta = this->target_elapsed_milliseconds;
      }

      this->left_over_milliseconds += _time_delta;
      while (this->left_over_milliseconds >=
             this->target_elapsed_milliseconds) {
        this->elapsed_milliseconds = this->target_elapsed_milliseconds;
        this->total_milliseconds += this->target_elapsed_milliseconds;
        this->left_over_milliseconds -= this->target_elapsed_milliseconds;
        this->frames_count++;

        p_tick_function;
      }
    } else {
      // Variable timestep update logic.
      this->elapsed_milliseconds = _time_delta;
      this->total_milliseconds += _time_delta;
      this->left_over_milliseconds = std::chrono::milliseconds::zero();
      this->frames_count++;

      p_tick_function;
    }

    // track the current framerate.
    this->frames_this_second += (this->frames_count - _last_frame_count);

    constexpr auto one_sec_in_milliseconds = std::chrono::seconds(1);
    if (this->seconds_counter >= one_sec_in_milliseconds) {
      this->fps = this->frames_this_second;
      this->frames_this_second = 0;
      this->seconds_counter -= one_sec_in_milliseconds;
    }
  }

private:
  static uint64_t milliseconds_to_seconds(uint64_t p_microseconds) {
    return p_microseconds / MILLISECONDS_PER_SECOND;
  }
  static uint64_t seconds_to_milliseconds(uint64_t p_seconds) {
    return p_seconds * MILLISECONDS_PER_SECOND;
  }

  // configuring fixed timestep mode.
  bool fixed_time_step = false;

  uint32_t frames_count = 0;
  uint32_t fps = 0;
  uint32_t frames_this_second = 0;

  std::chrono::milliseconds target_elapsed_milliseconds{MILLISECONDS_PER_SECOND /
                                                 SIXTY_FPS};
  std::chrono::milliseconds elapsed_milliseconds = {};
  std::chrono::milliseconds total_milliseconds = {};
  std::chrono::milliseconds left_over_milliseconds = {};
  std::chrono::milliseconds seconds_counter = {};

  std::chrono::steady_clock::time_point last_time = {
      std::chrono::steady_clock::now()};
}
#ifdef __clang__
__attribute__((packed)) __attribute__((aligned(64)))
#endif
;