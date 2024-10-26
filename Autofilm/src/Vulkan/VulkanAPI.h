#pragma once
#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#define VMA_IMPLEMENTATION
#include <vk_mem_alloc.h>

#include "Renderer/RenderAPI.h"
#include "Vulkan/VulkanWindow.h"
#include "Core/ThreadPool.h"
#include "Events/Event.h"

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
        void onEvent(Event& event);

        VkInstance _instance;
        VkDevice _device;
        VkPhysicalDevice _physicalDevice;
        VkQueue _graphicsQueue;
        VkQueue _presentQueue;
        VkRenderPass _renderPass;
        VkPipelineLayout _pipelineLayout;
        VkPipeline _graphicsPipeline;
        VkCommandPool _mainCommandPool;
        VkCommandBuffer _mainCommandBuffer;
        VkSemaphore _imageAvailableSemaphore;
        VkSemaphore _renderFinishedSemaphore;
        const static int FRAMES_IN_FLIGHT { 2 };
        std::array<VkFence, FRAMES_IN_FLIGHT> _renderFences;
        std::vector<VkSemaphore> _imageAvailableSemaphores;
        std::vector<VkSemaphore> _renderFinishedSemaphores;

        double _lastTime { 0 };
        int _frameCount { 0 };
        void printFPS();
        int _currentFrame { 0 };

        void createInstance();
        void setupDebugMessenger();
        void createSurfaces();
        void pickPhysicalDevice();
        bool isDeviceSuitable(VkPhysicalDevice device);
        void createLogicalDevice();
        void createSwapchains();
        void createImageViews();
        void createRenderPass();
        void createGraphicsPipeline();
        void createFramebuffers();
        void createCommandPool();
        void createCommandBuffer();
        void createSemaphores();
        void createRenderFence();
        void drawFrame() override;

        void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex, const VulkanWindow::WindowData& windowData);
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

        // Swapchains
        struct SwapchainSupportDetails
        {
            VkSurfaceCapabilitiesKHR capabilities;
            std::vector<VkSurfaceFormatKHR> formats;
            std::vector<VkPresentModeKHR> presentModes;
        };
        VulkanAPI::SwapchainSupportDetails querySwapchainSupport(VkPhysicalDevice device, VkSurfaceKHR surface);
        VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
        VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
        VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, GLFWwindow* window);

        // Shader modules
        VkShaderModule createShaderModule(const std::vector<char>& code);
        
        // Multi threading stuff
        uint32_t _numThreads { 0 };
        uint32_t _maxNumThreads { 0 };
        struct ThreadData {
            std::array<VkCommandPool, FRAMES_IN_FLIGHT> commandPools;
            std::array<VkCommandBuffer, FRAMES_IN_FLIGHT> commandBuffers;
            std::array<VkSemaphore, FRAMES_IN_FLIGHT> renderSemaphores;
            std::array<VkSemaphore, FRAMES_IN_FLIGHT> frameSemaphores;
            VulkanWindow::WindowData* windowData;
        };
        std::vector<ThreadData> _threadData;
        ThreadPool _threadPool;
        void prepareThreads();
        void threadRenderCode(const ThreadData* thread, int currentFrame, uint32_t imageIndex);

        // Validation Layers
        bool _enableValidationLayers = false;
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