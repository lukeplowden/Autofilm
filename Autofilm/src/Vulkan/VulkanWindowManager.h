#pragma once

#include "Core/Core.h"
#include "Core/Window.h"
#include "Vulkan/VulkanWindow.h"

namespace Autofilm
{
    class VulkanWindowManager
    {
    public:
        VulkanWindowManager() = default;
        ~VulkanWindowManager() = default;
        void createWindow(const WindowProperties&  props);
        void destroyWindow(const int& windowID);
    private:
        friend class VulkanAPI; 
        std::vector<std::unique_ptr<VulkanWindow>> _windows;
        int _nextWindowID = 0;
    };
}