#include "w_string.hpp"

#ifdef WOLF_ENABLE_FIBER

#include <unifex/for_each.hpp>
#include <unifex/range_stream.hpp>
#include <unifex/single_thread_context.hpp>
#include <unifex/sync_wait.hpp>
#include <unifex/then.hpp>
#include <unifex/transform_stream.hpp>
#include <unifex/via_stream.hpp>

#endif

#include <iostream>

using w_string = wolf::system::memory::w_string;

#ifdef WOLF_ENABLE_FIBER

auto w_string::test_lib() -> void
{
    constexpr auto _num = 10;

    unifex::single_thread_context context;

    unifex::sync_wait(
        unifex::then(
            unifex::for_each(
                unifex::via_stream(
                    context.get_scheduler(),
                    unifex::transform_stream(
                        unifex::range_stream{0, _num},
                        [](int value)
                        {
                            return value * value;
                        })),
                [](int value)
                {
                    std::cout << "got " << value << std::endl;
                }),
            []()
            {
                std::cout << "done" << std::endl;
            }));
}

#endif