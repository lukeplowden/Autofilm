#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "Renderer/RenderAPI.h"
#include "Vulkan/VulkanUtils.h"

namespace Autofilm
{
    class AUTOFILM_API VulkanAPI : public RenderAPI
    {
    public:
        void init() override;
        void shutdown() override;
        
        void clearColor(const glm::vec4& color) override;
		void clear() override;
    
    private:
        VulkanUtils vulkanUtils;
        VkInstance _instance;
        VkDevice _device;
        VkPhysicalDevice _physicalDevice;
        VkQueue _graphicsQueue;
        VkQueue _presentQueue;

        void createInstance();
        void setupDebugMessenger();
        void createSurfaces();
        void pickPhysicalDevice();
        bool isDeviceSuitable(VkPhysicalDevice device);
        void createLogicalDevice();
        void createSwapchains();
        struct QueueFamilyIndices {
            std::optional<uint32_t> graphicsFamily;
            std::optional<uint32_t> computeFamily;
            std::optional<uint32_t> presentFamily;
            std::optional<uint32_t> graphicsAndComputeFamily;

            bool isComplete() {
               return (graphicsAndComputeFamily.has_value() 
                        && presentFamily.has_value());
            }
        };
        QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);

        bool checkDeviceExtensionsSupport(VkPhysicalDevice device);
        const std::vector<const char*> _deviceExtensions = {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME
        };
        struct SwapchainSupportDetails
        {
            VkSurfaceCapabilitiesKHR capabilities;
            std::vector<VkSurfaceFormatKHR> formats;
            std::vector<VkPresentModeKHR> presentModes;
        };
        std::vector<VulkanAPI::SwapchainSupportDetails> querySwapchainSupport(VkPhysicalDevice device);
        VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);

        // Validation Layers
        bool _enableValidationLayers = true;
        VkDebugUtilsMessengerEXT _debugMessenger;
        const std::vector<const char*> _validationLayers = {
            "VK_LAYER_KHRONOS_validation"
        };
        bool checkValidationLayerSupport();
        std::vector<const char*> getRequiredExtensions();
        void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
        static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
            VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
            VkDebugUtilsMessageTypeFlagsEXT messageType,
            const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
            void* pUserData
        );
        VkResult CreateDebugUtilsMessengerEXT(
            VkInstance instance, 
            const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
            const VkAllocationCallbacks* pAllocator, 
            VkDebugUtilsMessengerEXT* pDebugMessenger
        );
    };
}