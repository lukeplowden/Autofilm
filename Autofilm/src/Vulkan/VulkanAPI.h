#pragma once
#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#include <vk_mem_alloc.h>

#include "Renderer/RenderAPI.h"
#include "Vulkan/VulkanWindowManager.h"
#include "Core/ThreadPool.h"
#include "Events/AllEvents.h"

namespace Autofilm
{
    struct DeletionQueue
    {
        std::deque<std::function<void()>> deletors;

        void push_function(std::function<void()>&& function) {
            deletors.push_back(function);
        }

        void flush() {
            // reverse iterate the deletion queue to execute all the functions
            for (auto it = deletors.rbegin(); it != deletors.rend(); it++) {
                (*it)(); //call functors
            }

            deletors.clear();
        }
    };

    class AUTOFILM_API VulkanAPI : public RenderAPI
    {
    public:
        void init() override;
        void shutdown() override;
        
        void clearColor(const glm::vec4& color) override;
		void clear() override;

    private:
        void onEvent(Event& event);
        bool onFramebufferResize(WindowResizeEvent& event);
        const static int FRAMES_IN_FLIGHT { 2 };

        VkInstance _instance;
        VkDevice _device;
        VkPhysicalDevice _physicalDevice;
        VkQueue _graphicsQueue;
        VkQueue _presentQueue;
        VkRenderPass _renderPass;
        VkPipelineLayout _pipelineLayout;
        VkPipeline _graphicsPipeline;
        std::array<VkFence, FRAMES_IN_FLIGHT> _renderFences;
        struct AllocatedImage {
            VkImage image;
            VkImageView imageView;
            VmaAllocation allocation;
            VkExtent3D imageExtent;
            VkFormat imageFormat;
        };
        struct VulkanWindowResources {
            VkSurfaceKHR surface;
            VkSwapchainKHR swapchain;
            std::vector<VkImage> swapchainImages;
            VkFormat swapchainImageFormat;
            VkExtent2D swapchainExtent;
            std::vector<VkImageView> swapchainImageViews;
            std::vector<VkFramebuffer> swapchainFramebuffers;
            AllocatedImage drawImage;
            bool framebufferResized = false;
            bool imageAcquired = false;
        };
        struct FrameSubmitPresentInfo {
            std::vector<uint32_t> imageIndices;
            std::vector<VkSwapchainKHR> swapchains;
            std::vector<VkSemaphore> frameSemaphores;
            std::vector<VkSemaphore> renderSemaphores;
            std::vector<VkCommandBuffer> commandBuffers;
            void resize(int size)
            {
                imageIndices.resize(size);
                swapchains.resize(size);
                frameSemaphores.resize(size);
                renderSemaphores.resize(size);
                commandBuffers.resize(size); 
            }
        };
        FrameSubmitPresentInfo submitPresentInfo; 
        std::unordered_map<int, VulkanWindowResources> _windowResources;

        VmaAllocator _allocator;
        DeletionQueue _mainDeletionQueue;

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
        
        // Window data
        void createSurface(int windowID);
        void createSwapchain(int windowID);
        void createImageViews(int windowID);
        void createFramebuffers(int windowID);
        void recreateSwapchain(int windowID);
        void cleanupSwapchain(int windowID);

        void createRenderPass();
        void createGraphicsPipeline();
        void createRenderFence();
        void drawFrame() override;

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
            VK_KHR_SWAPCHAIN_EXTENSION_NAME,
            VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME
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
            int windowID;
            std::array<VkCommandPool, FRAMES_IN_FLIGHT> commandPools;
            std::array<VkCommandBuffer, FRAMES_IN_FLIGHT> commandBuffers;
            std::array<VkSemaphore, FRAMES_IN_FLIGHT> renderSemaphores;
            std::array<VkSemaphore, FRAMES_IN_FLIGHT> frameSemaphores;
            std::array<DeletionQueue, FRAMES_IN_FLIGHT> deletionQueue;
        };
        std::vector<ThreadData> _threadData;
        ThreadPool _threadPool;
        void prepareThreads();
        void threadRenderCode(ThreadData* thread, int currentFrame, uint32_t imageIndex);

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