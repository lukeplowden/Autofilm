#pragma once

#ifdef AF_PLATFORM_WINDOWS
#include "Core.h"
#include "App.h"

extern Autofilm::App* Autofilm::CreateApp();

int	main(int argc, char** argv)
{
	Autofilm::Log::Init();

	AF_CORE_INFO("Initialised Log!");
	AF_INFO("Initialised Log!");
	auto app = Autofilm::CreateApp();
	Autofilm::WindowManager::Init(app->_settings.renderer); 
	Autofilm::WindowManager::createWindow();
    Autofilm::Renderer::init(app->_settings.renderer);
	app->Run();
	delete app;
}
#endif