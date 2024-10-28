#include "autofilmpch.h"

#include "Vulkan/VulkanWindowManager.h"

namespace Autofilm
{
    void VulkanWindowManager::createWindow(const WindowProperties& props)
    {
        auto window = std::make_unique<VulkanWindow>(_nextWindowID++, props);
        VulkanWindow* windowPtr = window.get();
        _windows.push_back(std::move(window));
    }
    
    void VulkanWindowManager::destroyWindow(const int& windowID)
    {
        auto it = std::find_if(_windows.begin(), _windows.end(),
            [windowID](const std::unique_ptr<VulkanWindow>& window)
            {
                return window->_data.ID == windowID;
            });

        if (it != _windows.end())
        {
            (*it)->shutdown();
            _windows.erase(it);
        }
    }
}