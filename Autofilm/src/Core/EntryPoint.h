#pragma once

#ifdef AF_PLATFORM_WINDOWS
#include "App.h"

extern Autofilm::App* Autofilm::CreateApp();

int	main(int argc, char** argv)
{
	Autofilm::Log::Init();
	AF_CORE_WARN("Initialised Log!");
	AF_INFO("Initialised Log!");
	// Autofilm::Renderer::init(Autofilm::RenderAPIType::OpenGL);
	auto app = Autofilm::CreateApp();
	app->Run();
	delete app;
}
#endif