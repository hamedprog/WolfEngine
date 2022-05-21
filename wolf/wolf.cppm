/*
    Project: Wolf Engine. Copyright © 2014-2022 Pooya Eimandar
    https://github.com/WolfEngine/WolfEngine
*/

module;

#include <gsl/gsl>
#include <string>
//#include <boost/pool/pool_alloc.hpp>

#ifdef WOLF_SYSTEM_MIMALLOC
#include <mimalloc-new-delete.h>
#include <mimalloc-override.h>
#endif

export module wolf;
//export import wolf.system;
//export import wolf.system.execution;
//export import wolf.system.gametime;

#if defined(_WIN32) && !defined(WASM)

#ifndef NOMINMAX
#define NOMINMAX
#endif // NOMINMAX

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif // WIN32_LEAN_AND_MEAN

#include <Windows.h>

#endif // _WIN32

#include "wolf.hpp"
