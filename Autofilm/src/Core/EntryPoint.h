#pragma once

#ifdef AF_PLATFORM_WINDOWS
#include "Core.h"
#include "App.h"

extern Autofilm::App* Autofilm::CreateApp();

int	main(int argc, char** argv)
{
	auto app = Autofilm::CreateApp();
	app->Run();
	delete app;
}
#endif