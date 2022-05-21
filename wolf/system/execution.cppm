/*
    Project: Wolf Engine. Copyright © 2014-2022 Pooya Eimandar
    https://github.com/WolfEngine/WolfEngine
*/

module;

#include "wolf_api.h"

export module wolf.system.execution;

export class execution {
public:
  // default constructor
  WOLF_API
  execution() = default;

  // destructor
  WOLF_API
  virtual ~execution() = default;
};










//export class  { 
//public:
//  // default constructor
//  WOLF_API
//  execution() = default;
//
//  // destructor
//  WOLF_API
//  virtual ~execution() = default;
//
////  // constructor
////  WOLF_API
////  explicit execution(const char *p_val) : folly::fbstring(p_val) {}
////
////  // constructor
////  WOLF_API
////  explicit execution(const std::string &p_val) : folly::fbstring(p_val) {}
////
////  // move constructor
////  WOLF_API
////  execution(execution &&p_val) noexcept { *this = p_val; }
////
////  // copy assignment
////  WOLF_API
////  execution &operator=(const execution &p_val) {
////    if (this != &p_val) {
////#if _MSC_VER
////      strncpy(this->data(), p_val.data(), p_val.size());
////#else
////      strlcpy(this->data(), p_val.data(), p_val.size());
////#endif
////    }
////    return *this;
////  }
////
////  // move assignment
////  WOLF_API
////  execution &operator=(execution &&p_val) noexcept {
////    this->swap(p_val);
////    return *this;
////  }
//};
//
