#pragma once

#include <wolf.h>

namespace wolf::system::chrono
{
    class w_timer
    {
        WOLF_API w_timer();
        WOLF_API virtual ~w_timer() = default;
    };
} // namespace wolf::system::chrono