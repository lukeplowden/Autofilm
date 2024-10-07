#pragma once

#ifdef AF_PLATFORM_WINDOWS
#include "App.h"

extern Autofilm::App* Autofilm::CreateApp();

int	main(int argc, char** argv)
{
	Autofilm::Log::Init();
	Autofilm::Log::GetCoreLogger()->warn("Initialised Log!");
	Autofilm::Log::GetClientLogger()->info("Initialised Log!");

	auto app = Autofilm::CreateApp();
	app->Run();
	delete app;
}
#endif