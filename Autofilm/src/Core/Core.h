#pragma once

#ifdef AF_PLATFORM_WINDOWS
	#ifdef AF_BUILD_DLL
		#define AUTOFILM_API __declspec(dllexport)
	#else
		#define AUTOFILM_API __declspec(dllimport)
	#endif
#elif defined(AF_PLATFORM_LINUX)
    #ifdef AF_BUILD_DLL
        #define AUTOFILM_API __attribute__((visibility("default")))
    #else
        #define AUTOFILM_API
    #endif
#else
    #error Platform not supported
#endif

#include "Core/Log.h"
#include <iostream>

#ifdef AF_ENABLE_ASSERTS
	#define AF_ASSERT(x, ...) { if(!(x)) { AF_ERROR("Assertion failed {0}", __VA_ARGS__); __debugbreak(); } }
	#define AF_CORE_ASSERT(x, ...) { if(!(x)) { AF_CORE_ERROR("Assertion failed {0}", __VA_ARGS__); __debugbreak(); } }
	#define AF_VK_ASSERT(x, ...) { if(!(x)) { AF_VK_ERROR("Assertion failed {0}", __VA_ARGS__); __debugbreak(); } }
#else
	#define AF_ASSERT(x, ...)
	#define AF_CORE_ASSERT(x, ...)
	#define AF_VK_ASSERT(x, ...)
#endif

#define BIT(x) (1 << x)

#define AF_BIND_EVENT_FN(fn) [this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }