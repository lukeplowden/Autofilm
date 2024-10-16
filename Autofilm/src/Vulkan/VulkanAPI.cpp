#include "autofilmpch.h"
#include "Core/Log.h"
#include "Vulkan/VulkanAPI.h"
#include "Vulkan/VulkanWindow.h"
#include "Core/Window.h"
namespace Autofilm
{
    void VulkanAPI::init()
    {
        createInstance();
        setupDebugMessenger();
        createSurfaces();
        pickPhysicalDevice();
        createLogicalDevice();
    }

    void VulkanAPI::shutdown()
    {
        vkDestroyDevice(_device, nullptr);
        vkDestroyInstance(_instance, nullptr);
    }

    void VulkanAPI::clearColor(const glm::vec4& color)
    {
    }

    void VulkanAPI::clear()
    {

    }

    void VulkanAPI::createInstance()
    {

        VkApplicationInfo appInfo{};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = "Autofilm";
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = "No Engine";
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion = VK_API_VERSION_1_0;
        
        VkInstanceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;
   
        auto extensions = getRequiredExtensions();
        createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
        createInfo.ppEnabledExtensionNames = extensions.data();

        AF_VK_ASSERT(checkValidationLayerSupport(), "Validation layers not available!");
        VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
        if (_enableValidationLayers) {
            createInfo.enabledLayerCount = static_cast<uint32_t>(_validationLayers.size());
            createInfo.ppEnabledLayerNames = _validationLayers.data();

            populateDebugMessengerCreateInfo(debugCreateInfo);
            createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*) &debugCreateInfo;

        } else {
            createInfo.enabledLayerCount = 0;
            createInfo.pNext = nullptr;
        }
        
        VkResult result = vkCreateInstance(&createInfo, nullptr, &_instance);
        AF_VK_ASSERT_EQUAL(result, VK_SUCCESS, "Failed to create Vulkan instance.");
    }

    void VulkanAPI::createSurfaces()
    {
        for (auto& window : WindowManager::getWindows()) {
            VulkanWindow* vulkanWindow = dynamic_cast<VulkanWindow*>(window.get());
            AF_VK_ASSERT(vulkanWindow, "Failed to cast window to VulkanWindow. The window type does not match the renderer.");
            vulkanWindow->createSurface(_instance);
        }
    }

    void VulkanAPI::pickPhysicalDevice()
    {
        uint32_t deviceCount = 0;
        vkEnumeratePhysicalDevices(_instance, &deviceCount, nullptr);

        AF_VK_ASSERT(deviceCount, "There are no GPUs which support Vulkan available.");

        std::vector<VkPhysicalDevice> devices(deviceCount);
        vkEnumeratePhysicalDevices(_instance, &deviceCount, devices.data());

        for (const auto& device : devices) {
            if (isDeviceSuitable(device)) {
                _physicalDevice = device;
                break;
            }
        }

        AF_VK_ASSERT_NOT_EQUAL(_physicalDevice, VK_NULL_HANDLE, "Failed to find a suitable GPU");

    }

    void VulkanAPI::createLogicalDevice()
    {
        QueueFamilyIndices indices = findQueueFamilies(_physicalDevice);
        
        std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
        std::set<uint32_t> uniqueQueueFamilies = {indices.graphicsAndComputeFamily.value(), indices.presentFamily.value()};

        float queuePriority = 1.0f;

        for (uint32_t queueFamily : uniqueQueueFamilies) {
            VkDeviceQueueCreateInfo queueCreateInfo{};
            queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueCreateInfo.queueFamilyIndex = queueFamily;
            queueCreateInfo.queueCount = 1;
            queueCreateInfo.pQueuePriorities = &queuePriority;
            queueCreateInfos.push_back(queueCreateInfo);
        }

        VkPhysicalDeviceFeatures deviceFeatures{};
        deviceFeatures.multiViewport = VK_TRUE;

        VkDeviceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
        createInfo.pQueueCreateInfos = queueCreateInfos.data();
        createInfo.pEnabledFeatures = &deviceFeatures;
        

        createInfo.enabledExtensionCount = static_cast<uint32_t>(_deviceExtensions.size());
        createInfo.ppEnabledExtensionNames = _deviceExtensions.data();

        // For backwards compatability:
        if (_enableValidationLayers) {
            createInfo.enabledLayerCount = static_cast<uint32_t>(_validationLayers.size());
            createInfo.ppEnabledLayerNames = _validationLayers.data();
        } else {
            createInfo.enabledLayerCount = 0;
        }

        VkResult result = vkCreateDevice(_physicalDevice, &createInfo, nullptr, &_device);
        AF_VK_ASSERT_EQUAL(result, VK_SUCCESS, "Failed to create a logical device.")

        vkGetDeviceQueue(_device, indices.graphicsAndComputeFamily.value(), 0, &_graphicsQueue);
        vkGetDeviceQueue(_device, indices.presentFamily.value(), 0, &_presentQueue);
    }

    bool VulkanAPI::isDeviceSuitable(VkPhysicalDevice device)
    {
        QueueFamilyIndices indices = findQueueFamilies(device);

        bool swapChainAdequate = true;
        std::vector<SwapchainSupportDetails> swapChainSupportList = querySwapchainSupport(device);
        for (auto support : swapChainSupportList) {
            if (support.formats.empty() && support.presentModes.empty()){
                swapChainAdequate = false;
                break;
            }
        }
        bool extensionsSupported = checkDeviceExtensionsSupport(device);

        bool result = indices.isComplete() && extensionsSupported && swapChainAdequate;
        return result;
    }

    bool VulkanAPI::checkDeviceExtensionsSupport(VkPhysicalDevice device)
    {
        uint32_t extensionCount;
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

        std::vector<VkExtensionProperties> availableExtensions(extensionCount);
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

        std::set<std::string> requiredExtensions(_deviceExtensions.begin(), _deviceExtensions.end());

        for (const auto& extension : availableExtensions) {
            requiredExtensions.erase(extension.extensionName);
        }

        return requiredExtensions.empty();
    }

    std::vector<VulkanAPI::SwapchainSupportDetails> VulkanAPI::querySwapchainSupport(VkPhysicalDevice device)
    {
        std::vector<SwapchainSupportDetails> detailsList;
        for (auto& window : WindowManager::getWindows()) {
            SwapchainSupportDetails details;
            VulkanWindow* vulkanWindow = dynamic_cast<VulkanWindow*>(window.get());
            auto surface = vulkanWindow -> getSurface();

            vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);
            uint32_t formatCount;
            vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());

            uint32_t presentModeCount;
            vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data());

            if (presentModeCount != 0) {
                details.presentModes.resize(presentModeCount);
                vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data());
            }
            detailsList.push_back(details);
        }
        return detailsList;
    }


    VulkanAPI::QueueFamilyIndices VulkanAPI::findQueueFamilies(VkPhysicalDevice device)
    {
        QueueFamilyIndices indices;
        
        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
        
        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

        int i = 0;
        for (const auto& queueFamily : queueFamilies) {
            for (const auto& window : WindowManager::getWindows()) {
                VulkanWindow* vulkanWindow = dynamic_cast<VulkanWindow*>(window.get());
                VkBool32 presentSupport = false;
                vkGetPhysicalDeviceSurfaceSupportKHR(device, i, vulkanWindow->getSurface(), &presentSupport);
                if (presentSupport) {
                    indices.presentFamily = i;
                }
            }
            if ((queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) && (queueFamily.queueFlags & VK_QUEUE_COMPUTE_BIT)) {
                indices.graphicsAndComputeFamily = i;
            }
            i++;
        }
        return indices;
    }

    bool VulkanAPI::checkValidationLayerSupport()
    {
        uint32_t layerCount;
        vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

        std::vector<VkLayerProperties> availableLayers(layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

        for (const char* layerName : _validationLayers) {
            bool layerFound = false;
            for (const auto& layerProperties : availableLayers) {
                if (strcmp(layerName, layerProperties.layerName) == 0) {
                    layerFound = true;
                    break;
                }
            }
            if(!layerFound) {
                return false;
            }
        }
        return true;
    }

    std::vector<const char*> VulkanAPI::getRequiredExtensions() 
    {
        uint32_t glfwExtensionCount = 0;
        const char** glfwExtensions;
        glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

        if (_enableValidationLayers) {
            extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        }

        return extensions;
    }

    void VulkanAPI::populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo)
    {
        createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT 
                                    // | VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT
                                    | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT 
                                    | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT 
                                | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT 
                                | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        createInfo.pfnUserCallback = debugCallback;
    }
    void VulkanAPI::setupDebugMessenger() 
    {
        if (!_enableValidationLayers) { return; }
        VkDebugUtilsMessengerCreateInfoEXT createInfo;
        populateDebugMessengerCreateInfo(createInfo);
        VkResult result = CreateDebugUtilsMessengerEXT(_instance, &createInfo, nullptr, &_debugMessenger);
        AF_VK_ASSERT_EQUAL(result, VK_SUCCESS, "Failed to create the debug messenger.");
    }
     
    VKAPI_ATTR VkBool32 VKAPI_CALL VulkanAPI::debugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void* pUserData
    )
    {
        switch (messageSeverity)
        {
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
                AF_VK_TRACE(pCallbackData->pMessage);
                break;
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
                AF_VK_INFO(pCallbackData->pMessage);
                break;
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
                AF_VK_WARN(pCallbackData->pMessage);
                break;
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
                AF_VK_ERROR(pCallbackData->pMessage);
                break;
        }

        return VK_FALSE;
    };

    VkResult VulkanAPI::CreateDebugUtilsMessengerEXT(
        VkInstance instance, 
        const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
        const VkAllocationCallbacks* pAllocator, 
        VkDebugUtilsMessengerEXT* pDebugMessenger
    )
    {
        auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
        if (func != nullptr) {
            return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
        } else {
            return VK_ERROR_EXTENSION_NOT_PRESENT;
        }
    }
}