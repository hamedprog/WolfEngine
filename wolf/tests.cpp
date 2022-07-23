#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp> // NOLINT

#include <stream/http/w_http_server.hpp>
#include <stream/webrtc/w_desktop_capturer.hpp>
#include <stream/webrtc/w_vcm_capturer.hpp>
#include <system/w_task.hpp>
#include <wolf.hpp>

TEST_CASE("wolf", "version") {
  // NOLINTBEGIN
  REQUIRE(wolf::w_version() == "v3.0.0.0");

#ifdef WOLF_SYSTEM_EXECUTION

  unifex::static_thread_pool context(2);
  auto sched = context.get_scheduler();
  auto task = wolf::system::w_task<std::string>(sched, wolf::w_version);
  unifex::sync_wait(std::move(task) | unifex::then([](auto &&p_result) {
                      REQUIRE(p_result == "v3.0.0.0");
                    }));
#endif

  // NOLINTEND
}

#ifdef WOLF_STREAM_WEBRTC
TEST_CASE("wolf:stream", "webRTC") {
  using namespace wolf::stream;
  auto webrtc_desktop_capturer = webRTC::w_desktop_capturer();

  auto funcs = std::map<std::string, http::w_http_function>();
  auto options = std::vector<std::string>();
  auto http_server = http::w_http_server(options, nullptr);
}
#endif