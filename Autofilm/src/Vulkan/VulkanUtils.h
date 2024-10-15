#pragma once
#include "Core/Core.h"
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>


namespace Autofilm
{
    class AUTOFILM_API VulkanUtils
    {
    public:
        
    private:
        VkInstance _instance;
        VkDevice _device;
        VkPhysicalDevice _physicalDevice;
        VkQueue _graphicsQueue;
        VkQueue _presentQueue;
    };
}