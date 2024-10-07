#include "autofilmpch.h"
#include "App.h"
#include "Log.h"

namespace Autofilm
{
    App::App()
    {

    }

    App::~App()
    {

    }

    void App::Run()
    {
        m_window = std::unique_ptr<Window>(Window::Create());
        while (m_running)
        {
            m_window->onUpdate();
        }
    }
}