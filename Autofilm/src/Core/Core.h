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