#pragma once
#ifndef Platform_hpp__
#define Platform_hpp__

#if defined(__linux__)
#define LINUX
#elif defined(_WIN32)
#define WINDOWS
#else
#error Unsupported platform
#endif

#if defined(_M_X64) || defined(__amd64__)
#define XD_X64
#else
#define XD_X86
#endif

#include "Compiler.inl"

#if defined(LINUX)
#include "PlatformLinux.inl"
#elif defined(WINDOWS)
#include "PlatformWindows.inl"
#endif

#endif // Platform_hpp__
