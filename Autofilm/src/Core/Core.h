#pragma once

#ifdef AF_PLATFORM_WINDOWS
	#ifdef AF_BUILD_DLL
		#define AUTOFILM_API __declspec(dllexport)
	#else
		#define AUTOFILM_API __declspec(dllimport)
	#endif
#else
	#error No linux yet :~(
#endif

#ifdef AF_DEBUG
	#define AF_ASSERT(x, ...) { if(!(x)) { AF_ERROR("Assertion failed {0}", __VA_ARGS__); __debugbreak(); } }
	#define AF_CORE_ASSERT(x, ...) { if(!(x)) { AF_CORE_ERROR("Assertion failed {0}", __VA_ARGS__); __debugbreak(); } }
	#define AF_CORE_ASSERT_EQUAL(x, y, ...) { if(x != y) { AF_CORE_ERROR("Assertion failed {0}", __VA_ARGS__); __debugbreak(); } }
	#define AF_VK_ASSERT(x, ...) { if(!(x)) { AF_VK_ERROR("Assertion failed {0}", __VA_ARGS__); __debugbreak(); } }
	#define AF_VK_ASSERT_EQUAL(x, y, ...) { if(x != y) { AF_VK_ERROR("Assertion failed {0}", __VA_ARGS__); __debugbreak(); } }
	#define AF_VK_ASSERT_NOT_EQUAL(x, y, ...) { if(x == y) { AF_VK_ERROR("Assertion failed {0}", __VA_ARGS__); __debugbreak(); } }
#else
	#define AF_ASSERT(x, ...)
	#define AF_CORE_ASSERT(x, ...)
#endif