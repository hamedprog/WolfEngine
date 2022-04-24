/*
    Project: Wolf Engine. Copyright © 2014-2022 Pooya Eimandar
    https://github.com/WolfEngine/WolfEngine
*/
#pragma once

#include <folly/FBString.h> // NOLINT

namespace wolf::system::memory
{
    class w_string : public folly::fbstring
    {
    public:
        // default constructor
        w_string();

        // destructor
        virtual ~w_string() = default;

        // constructor
        explicit w_string(const std::string &p_val) : folly::fbstring(p_val)
        {
        }

        // copy constructor
        w_string(const w_string &p_val) : folly::fbstring(p_val.data()) // NOLINT (fuchsia-default-arguments-calls)
        {
        }

        // move constructor
        w_string(w_string &&p_val) noexcept
        {
            *this = p_val;
        }

        // copy assignment
        w_string &operator=(const w_string &p_val)
        {
            if (this != &p_val)
            {
                strlcpy(this->data(), p_val.data(), p_val.size());
            }
            return *this;
        }

        // move assignment
        w_string &operator=(w_string &&p_val) noexcept
        {
            this->swap(p_val);
            return *this;
        }
    };

} // namespace wolf::system::memory
