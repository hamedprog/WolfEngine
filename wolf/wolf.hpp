/*
    Project: Wolf Engine. Copyright © 2014-2022 Pooya Eimandar
    https://github.com/WolfEngine/WolfEngine
*/

#pragma once

#include "w_memory/w_string.hpp"
#include <gsl/gsl>

#ifndef WOLF_API

#ifdef WIN32

#define WOLF_API __declspec(dllexport)

#ifndef NOMINMAX
#define NOMINMAX
#endif

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif // WIN32_LEAN_AND_MEAN

#include <Windows.h>

BOOL APIENTRY DllMain(_In_ HMODULE pHModule, _In_ DWORD pULReasonForCall, _In_ LPVOID pLPReserved)
{
    switch (pULReasonForCall)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

#else

#define WOLF_API

#endif // WIN32

#endif // WOLF_API

namespace wolf
{
    /**
     * returns wolf version
     * @return string format with the following style "<major>.<minor>.<patch>.<debug>"
     */
    WOLF_API auto w_version() -> system::memory::w_string;
} // namespace wolf
