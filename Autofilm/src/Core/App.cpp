#include "autofilmpch.h"
#include "App.h"
#include "Log.h"

namespace Autofilm
{
    App::App(const AppProperties& props)
    {
        m_props = props;
    }

    App::~App()
    {

    }

    void App::Run()
    {
        m_windows.push_back(std::unique_ptr<Window>(Window::Create()));
        m_windows.push_back(std::unique_ptr<Window>(Window::Create()));
        while (m_running)
        {
            for (auto& window : m_windows)
            {
                window->onUpdate();
            }
        }
    }
}