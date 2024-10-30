#include "autofilmpch.h"

#define VMA_IMPLEMENTATION
#include "Vulkan/VulkanAPI.h"
#include "Vulkan/VulkanUtils.h"

#include "Core/Log.h"
#include "Core/File.h"
#include "Core/Window.h"
#include "Events/AllEvents.h"
namespace Autofilm
{
    void VulkanAPI::init()
    {
        createInstance();
        setupDebugMessenger();
        createSurfaces(); // For each window
        pickPhysicalDevice();
        createLogicalDevice();
        createRenderPass();
        createGraphicsPipeline();
        createRenderFence();
        prepareThreads();

        // initialize the memory allocator
        VmaAllocatorCreateInfo allocatorInfo = {};
        allocatorInfo.physicalDevice = _physicalDevice;
        allocatorInfo.device = _device;
        allocatorInfo.instance = _instance;
        allocatorInfo.flags = VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT;
        vmaCreateAllocator(&allocatorInfo, &_allocator);
        _mainDeletionQueue.push_function([&]() {
            vmaDestroyAllocator(_allocator);
        });

        auto& windows = WindowManager::getWindows();
        for (int i = 0; i < WindowManager::getWindows().size(); i++)
        {
            VulkanWindow* vulkanWindow = dynamic_cast<VulkanWindow*>(windows[i].get());
            auto ID = vulkanWindow->getID();
            
            vulkanWindow->setEventCallback(AF_BIND_EVENT_FN(VulkanAPI::onEvent));
            
            createSwapchain(ID);
            createImageViews(ID);
            createFramebuffers(ID);
        }
    }

    void VulkanAPI::onEvent(Event& event)
    {
        EventDispatcher dispatcher(event);
        dispatcher.Dispatch<WindowResizeEvent>(AF_BIND_EVENT_FN(onFramebufferResize));
        dispatcher.Dispatch<WindowCloseEvent>(AF_BIND_EVENT_FN(onWindowClose));
    }

    bool VulkanAPI::onWindowClose(WindowCloseEvent& event)
    {
        vkDeviceWaitIdle(_device);
        uint32_t ID = event.getID();

        auto windowResourceIt = _windowResources.find(ID);
        if (windowResourceIt != _windowResources.end()) {
            VulkanWindowResources& resources = windowResourceIt->second;
            for (VkFramebuffer framebuffer : resources.swapchainFramebuffers) {
                vkDestroyFramebuffer(_device, framebuffer, nullptr);
            }
            for (VkImageView imageView : resources.swapchainImageViews) {
                vkDestroyImageView(_device, imageView, nullptr);
            }

            if (resources.swapchain != VK_NULL_HANDLE) {
                vkDestroySwapchainKHR(_device, resources.swapchain, nullptr);
            }
            if (resources.surface != VK_NULL_HANDLE) {
                vkDestroySurfaceKHR(_instance, resources.surface, nullptr);
            }
            if (resources.drawImage.image != VK_NULL_HANDLE) {
                vkDestroyImage(_device, resources.drawImage.image, nullptr);
                // vkFreeMemory(_device, resources.drawImage.memory, nullptr);
                vkDestroyImageView(_device, resources.drawImage.imageView, nullptr);
            }
            _windowResources.erase(windowResourceIt);
        }

        auto threadIt = std::find_if(_threadData.begin(), _threadData.end(),
                                    [ID](const ThreadData& thread) { return thread.windowID == ID; });

        if (threadIt != _threadData.end()) {
            ThreadData& threadData = *threadIt;

            for (int j = 0; j < FRAMES_IN_FLIGHT; j++) {
                if (threadData.commandBuffers[j] != VK_NULL_HANDLE) {
                    vkFreeCommandBuffers(_device, threadData.commandPools[j], 1, &threadData.commandBuffers[j]);
                }
                if (threadData.commandPools[j] != VK_NULL_HANDLE) {
                    vkDestroyCommandPool(_device, threadData.commandPools[j], nullptr);
                }
                if (threadData.renderSemaphores[j] != VK_NULL_HANDLE) {
                    vkDestroySemaphore(_device, threadData.renderSemaphores[j], nullptr);
                }
                if (threadData.frameSemaphores[j] != VK_NULL_HANDLE) {
                    vkDestroySemaphore(_device, threadData.frameSemaphores[j], nullptr);
                }
                threadData.deletionQueue[j].flush();
            }

            _threadData.erase(threadIt);
            _numThreads = _threadData.size();
        }
        WindowManager::destroyWindow(event.getID());
        prepareThreads();
        return true;
    }

    bool VulkanAPI::onFramebufferResize(WindowResizeEvent& event)
    {
        VulkanWindowResources& resources = _windowResources[event.getID()];
        resources.framebufferResized = true;
        return true;
    }

    void VulkanAPI::shutdown()
    {
        _mainDeletionQueue.flush();

        vkDestroyFence(_device, _renderFences[_currentFrame], nullptr);
        vkDestroyPipelineLayout(_device, _pipelineLayout, nullptr);
        vkDestroyRenderPass(_device, _renderPass, nullptr);
        vkDestroyDevice(_device, nullptr);
        vkDestroyInstance(_instance, nullptr);
    }

    void VulkanAPI::prepareThreads()
    {
        _maxNumThreads = std::thread::hardware_concurrency();
        AF_CORE_ASSERT(_maxNumThreads > 0, "No threads are available");

        auto& windows = WindowManager::getWindows();
        int numWindows = windows.size();
        _threadData.clear();
        _threadPool.setThreadCount(numWindows);
        _threadData.resize(numWindows);
        _numThreads = numWindows;
        if (_numThreads > _maxNumThreads) {
            AF_CORE_WARN("More windows are open than threads available. Performance may be impacted.");
        }

        VkSemaphoreCreateInfo semaphoreInfo{};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        QueueFamilyIndices queueFamilyIndices = findQueueFamilies(_physicalDevice);

        VkCommandPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsAndComputeFamily.value();

        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount = 1;
        
        for (int i = 0; i < _numThreads; i++) {
            ThreadData* thread = &_threadData[i];
            VulkanWindow* vulkanWindow = dynamic_cast<VulkanWindow*>(windows[i].get());
            thread->windowID = vulkanWindow->getID();
            for (int j = 0; j < FRAMES_IN_FLIGHT; j++) {
                VkResult result = vkCreateCommandPool(_device, &poolInfo, nullptr, &thread->commandPools[j]);
                AF_VK_ASSERT(result == VK_SUCCESS, "Failed to create a command pool.");
                
                allocInfo.commandPool = thread->commandPools[j];

                result = vkAllocateCommandBuffers(_device, &allocInfo, &thread->commandBuffers[j]);
                AF_VK_ASSERT(result == VK_SUCCESS, "Failed to create a command pool.");

                vkCreateSemaphore(_device, &semaphoreInfo, nullptr, &thread->renderSemaphores[j]);
                vkCreateSemaphore(_device, &semaphoreInfo, nullptr, &thread->frameSemaphores[j]);
            }
        }
    }

    void VulkanAPI::threadRenderCode(ThreadData* thread, int currentFrame, uint32_t imageIndex)
    {
        VulkanWindowResources& resources = _windowResources[thread->windowID];
        thread->deletionQueue[currentFrame].flush();
        vkResetCommandPool(_device, thread->commandPools[currentFrame], 0);
                
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = 0;
        beginInfo.pInheritanceInfo = nullptr;

        auto& commandBuffer = thread->commandBuffers[currentFrame];
        VkResult result = vkBeginCommandBuffer(commandBuffer, &beginInfo);

        VkRenderPassBeginInfo renderPassBeginInfo{};
        renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassBeginInfo.renderPass = _renderPass;
        renderPassBeginInfo.framebuffer = resources.swapchainFramebuffers[imageIndex];
        renderPassBeginInfo.renderArea.offset = { 0, 0 };
        renderPassBeginInfo.renderArea.extent = resources.swapchainExtent;
        float col = ((float)std::sin(glfwGetTime()) * 0.5f) + 0.5f;
        VkClearValue clearColor = { { { col, 0.0f, 1.0f-col, 1.0f } } };
        renderPassBeginInfo.clearValueCount = 1;
        renderPassBeginInfo.pClearValues = &clearColor;
        
        vkCmdBeginRenderPass(commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, _graphicsPipeline);
        
        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<float>(resources.swapchainExtent.width);
        viewport.height = static_cast<float>(resources.swapchainExtent.height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

        VkRect2D scissor{};
        scissor.offset = {0, 0};
        scissor.extent = resources.swapchainExtent;
        vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

        vkCmdDraw(commandBuffer, 3, 1, 0, 0);

        vkCmdEndRenderPass(commandBuffer);
        result = vkEndCommandBuffer(commandBuffer);
    }

    void VulkanAPI::drawFrame()
    {
        vkWaitForFences(_device, 1, &_renderFences[_currentFrame], VK_TRUE, UINT64_MAX);

        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = 0;
        beginInfo.pInheritanceInfo = nullptr;

        uint32_t validWindows { 0 };
        std::vector<uint32_t> outOfDateWindowIDs;

        std::vector<uint32_t> imageIndices;
        std::vector<VkSwapchainKHR> swapchains;
        std::vector<VkSemaphore> frameSemaphores;
        std::vector<VkSemaphore> renderSemaphores;
        std::vector<VkCommandBuffer> commandBuffers;
        imageIndices.reserve(_numThreads);
        swapchains.reserve(_numThreads);
        frameSemaphores.reserve(_numThreads);
        renderSemaphores.reserve(_numThreads);
        commandBuffers.reserve(_numThreads);

        for (uint32_t t = 0; t < _numThreads; t++) {
            ThreadData* thread = &_threadData[t];
            VulkanWindowResources& resources = _windowResources[thread->windowID];
            uint32_t imageIndex;

            // Center coordinates of the circular path
            int screenCenterX = 1920*3/4; // Example screen center x coordinate
            int screenCenterY = 1080*3/4; // Example screen center y coordinate
            int radius = 700;        // Radius of the circular path

            float angle = glfwGetTime()/3.0f + (2.0f * 3.14f * t / _numThreads);
            int xPos = screenCenterX + static_cast<int>(radius * std::cos(angle));
            int yPos = screenCenterY + static_cast<int>(radius * std::sin(angle));

            VulkanWindow* vulkanWindow = dynamic_cast<VulkanWindow*>(WindowManager::getWindows()[t].get());
            glfwSetWindowPos(vulkanWindow->_window, xPos, yPos);

            VkResult result = vkAcquireNextImageKHR(_device, resources.swapchain, 1000000000, thread->frameSemaphores[_currentFrame], VK_NULL_HANDLE, &imageIndex);
            if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || resources.framebufferResized == true) {
                resources.framebufferResized = false;
                outOfDateWindowIDs.push_back(thread->windowID);
                frameSemaphores.push_back(thread->frameSemaphores[_currentFrame]);
                continue;
            }
            _threadPool._threads[t]->addJob([=] { threadRenderCode(thread, _currentFrame, imageIndex); });

            imageIndices.push_back(imageIndex);
            swapchains.push_back(resources.swapchain);
            frameSemaphores.push_back(thread->frameSemaphores[_currentFrame]);
            renderSemaphores.push_back(thread->renderSemaphores[_currentFrame]);
            commandBuffers.push_back(thread->commandBuffers[_currentFrame]);

            validWindows++;
        }
        vkResetFences(_device, 1, &_renderFences[_currentFrame]);

        _threadPool.wait();
        printFPS();

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, 
                                            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, 
                                            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                                             VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
        submitInfo.waitSemaphoreCount = frameSemaphores.size();
        submitInfo.pWaitSemaphores = frameSemaphores.data();
        submitInfo.pWaitDstStageMask = waitStages;
        submitInfo.commandBufferCount = validWindows;
        submitInfo.pCommandBuffers = commandBuffers.data();
        submitInfo.signalSemaphoreCount = validWindows;
        submitInfo.pSignalSemaphores = renderSemaphores.data();

        VkResult result = vkQueueSubmit(_graphicsQueue, 1, &submitInfo, _renderFences[_currentFrame]);
        AF_VK_ASSERT(result == VK_SUCCESS, "Failed to submit draw command buffer.");

        // This can be made on the fly
        VkPresentInfoKHR presentInfo{};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.waitSemaphoreCount = validWindows;
        presentInfo.pWaitSemaphores = renderSemaphores.data();
        presentInfo.swapchainCount = validWindows;
        presentInfo.pSwapchains = swapchains.data();
        presentInfo.pImageIndices = imageIndices.data();

        result = vkQueuePresentKHR(_presentQueue, &presentInfo);
        for (auto& ID : outOfDateWindowIDs) {
            recreateSwapchain(ID);
        }
        _currentFrame = (_currentFrame + 1) % FRAMES_IN_FLIGHT;
    }

    void VulkanAPI::clearColor(const glm::vec4& color)
    {
    }

    void VulkanAPI::clear()
    {

    }

    void VulkanAPI::printFPS()
    {
        double currentTime = glfwGetTime();
        _frameCount++;
        if (currentTime - _lastTime >= 1.0) {
            double fps = _frameCount / (currentTime - _lastTime);
            AF_CORE_TRACE("FPS: {0}", fps);
            _frameCount = 0;
            _lastTime = currentTime;
        }
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
        AF_VK_ASSERT(result == VK_SUCCESS, "Failed to create Vulkan instance.");
    }

    void VulkanAPI::createSurfaces()
    {
        for (const auto& window : WindowManager::getWindows()) {
            VulkanWindow* vulkanWindow = dynamic_cast<VulkanWindow*>(window.get());
            createSurface(vulkanWindow->getID());
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

        AF_VK_ASSERT(_physicalDevice != VK_NULL_HANDLE, "Failed to find a suitable GPU");
    }

    void VulkanAPI::createLogicalDevice()
    {
        QueueFamilyIndices indices = findQueueFamilies(_physicalDevice);
        
        std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
        std::set<uint32_t> uniqueQueueFamilies = { indices.graphicsAndComputeFamily.value(), indices.presentFamily.value() };

        float queuePriority = 1.0f;

        for (uint32_t queueFamily : uniqueQueueFamilies) {
            VkDeviceQueueCreateInfo queueCreateInfo{};
            queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueCreateInfo.queueFamilyIndex = queueFamily;
            queueCreateInfo.queueCount = 1;
            queueCreateInfo.pQueuePriorities = &queuePriority;
            queueCreateInfos.push_back(queueCreateInfo);
        }

        // VkPhysicalDeviceFeatures deviceFeatures{};
        // deviceFeatures.multiViewport = VK_TRUE;

        VkPhysicalDeviceVulkan12Features features12{ .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES };
        features12.bufferDeviceAddress = true;
        features12.descriptorIndexing = true;

        VkDeviceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
        createInfo.pQueueCreateInfos = queueCreateInfos.data();
        createInfo.pEnabledFeatures = nullptr;

        createInfo.enabledExtensionCount = static_cast<uint32_t>(_deviceExtensions.size());
        createInfo.ppEnabledExtensionNames = _deviceExtensions.data();
        createInfo.pNext = &features12;

        // For backwards compatability:
        if (_enableValidationLayers) {
            createInfo.enabledLayerCount = static_cast<uint32_t>(_validationLayers.size());
            createInfo.ppEnabledLayerNames = _validationLayers.data();
        } else {
            createInfo.enabledLayerCount = 0;
        }

        VkResult result = vkCreateDevice(_physicalDevice, &createInfo, nullptr, &_device);
        AF_VK_ASSERT(result == VK_SUCCESS, "Failed to create a logical device.");

        vkGetDeviceQueue(_device, indices.graphicsAndComputeFamily.value(), 0, &_graphicsQueue);
        vkGetDeviceQueue(_device, indices.presentFamily.value(), 0, &_presentQueue);
    }

    void VulkanAPI::createSurface(int windowID)
    {
        VulkanWindowResources& resources = _windowResources[windowID];
        GLFWwindow* wndPtr = dynamic_cast<VulkanWindow*>(WindowManager::getWindows()[windowID].get())->_window;
        VkResult result = glfwCreateWindowSurface(_instance, wndPtr, nullptr, &resources.surface);
        AF_VK_ASSERT(result == VK_SUCCESS, "Failed to create a surface");
    }

    void VulkanAPI::createSwapchain(int windowID)
    {
        VulkanWindowResources& resources = _windowResources[windowID];
        VkSurfaceKHR surface = resources.surface;
        GLFWwindow* wndPtr = dynamic_cast<VulkanWindow*>(WindowManager::getWindows()[windowID].get())->_window;

        SwapchainSupportDetails swapchainSupport = querySwapchainSupport(_physicalDevice, surface);

        VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapchainSupport.formats);
        VkPresentModeKHR presentMode = chooseSwapPresentMode(swapchainSupport.presentModes);
        VkExtent2D extent = chooseSwapExtent(swapchainSupport.capabilities, wndPtr);

        uint32_t imageCount = swapchainSupport.capabilities.minImageCount + 1;
        if (swapchainSupport.capabilities.maxImageCount > 0 && imageCount > swapchainSupport.capabilities.maxImageCount) {
            imageCount = swapchainSupport.capabilities.maxImageCount;
        }

        VkSwapchainCreateInfoKHR createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        createInfo.surface = surface;

        createInfo.minImageCount = imageCount;
        createInfo.imageFormat = surfaceFormat.format;
        createInfo.imageColorSpace = surfaceFormat.colorSpace;
        createInfo.imageExtent = extent;
        createInfo.imageArrayLayers = 1;
        createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

        QueueFamilyIndices indices = findQueueFamilies(_physicalDevice);
        uint32_t queueFamilyIndices[] = {indices.graphicsAndComputeFamily.value(), indices.presentFamily.value()};

        if (indices.graphicsFamily != indices.presentFamily) {
            createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            createInfo.queueFamilyIndexCount = 2;
            createInfo.pQueueFamilyIndices = queueFamilyIndices;
        } else {
            createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
            createInfo.queueFamilyIndexCount = 0; // Optional
            createInfo.pQueueFamilyIndices = nullptr; // Optional
        }

        createInfo.preTransform = swapchainSupport.capabilities.currentTransform;
        createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        
        createInfo.presentMode = presentMode;
        createInfo.clipped = VK_TRUE;
        createInfo.oldSwapchain = VK_NULL_HANDLE;
        
        VkResult result = vkCreateSwapchainKHR(_device, &createInfo, nullptr, &resources.swapchain);
        AF_VK_ASSERT(result == VK_SUCCESS, "Failed to create a Vulkan Swapchain.");
        vkGetSwapchainImagesKHR(_device, resources.swapchain, &imageCount, nullptr);
        resources.swapchainImages.resize(imageCount);
        vkGetSwapchainImagesKHR(_device, resources.swapchain, &imageCount, resources.swapchainImages.data());
        resources.swapchainImageFormat = surfaceFormat.format;
        resources.swapchainExtent = extent;

        VkExtent3D drawImageExtent = {
            extent.width,
            extent.height,
            1
        };

        resources.drawImage.imageFormat = VK_FORMAT_R16G16B16A16_SFLOAT;
        resources.drawImage.imageExtent = drawImageExtent;

        VkImageUsageFlags drawImageUsages{};
        drawImageUsages |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
        drawImageUsages |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
        drawImageUsages |= VK_IMAGE_USAGE_STORAGE_BIT;
        drawImageUsages |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

        VkImageCreateInfo imgCreateInfo = VulkanUtils::imageCreateInfo(resources.drawImage.imageFormat, drawImageUsages, drawImageExtent);

        VmaAllocationCreateInfo imgAllocInfo = {};
        imgAllocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
        imgAllocInfo.requiredFlags = VkMemoryPropertyFlags(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

        vmaCreateImage(_allocator, &imgCreateInfo, &imgAllocInfo, &resources.drawImage.image, &resources.drawImage.allocation, nullptr);

        VkImageViewCreateInfo viewCreateInfo = VulkanUtils::imageViewCreateInfo(resources.drawImage.imageFormat, resources.drawImage.image, VK_IMAGE_ASPECT_COLOR_BIT);

        result = vkCreateImageView(_device, &viewCreateInfo, nullptr, &resources.drawImage.imageView);

        _mainDeletionQueue.push_function([=]() {
            vkDestroyImageView(_device, resources.drawImage.imageView, nullptr);
            vmaDestroyImage(_allocator, resources.drawImage.image, resources.drawImage.allocation);
        });
    }

    void VulkanAPI::createImageViews(int windowID)
    {
        VulkanWindowResources& resources = _windowResources[windowID];
        resources.swapchainImageViews.resize(
            resources.swapchainImages.size()
        );
        for (size_t i = 0; i < resources.swapchainImages.size(); i++) {
            VkImageViewCreateInfo createInfo{};
            createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            createInfo.image = resources.swapchainImages[i];
            createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            createInfo.format = resources.swapchainImageFormat;
            createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            createInfo.subresourceRange.baseMipLevel = 0;
            createInfo.subresourceRange.levelCount = 1;
            createInfo.subresourceRange.baseArrayLayer = 0;
            createInfo.subresourceRange.layerCount = 1;
            VkResult result = vkCreateImageView(_device, &createInfo, nullptr, &resources.swapchainImageViews[i]);
            AF_VK_ASSERT(result == VK_SUCCESS, "Failed to create an image view");
        }
    }

    void VulkanAPI::createFramebuffers(int windowID)
    {
        VulkanWindowResources& resources = _windowResources[windowID];
        const auto imageViews = resources.swapchainImageViews;
        resources.swapchainFramebuffers.resize(imageViews.size());
        for (size_t i = 0; i < imageViews.size(); i++) {
            VkImageView attachments[] = {
                imageViews[i]
            };

            VkFramebufferCreateInfo framebufferInfo{};
            framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebufferInfo.renderPass = _renderPass;
            framebufferInfo.attachmentCount = 1;
            framebufferInfo.pAttachments = attachments;
            framebufferInfo.width = resources.swapchainExtent.width;
            framebufferInfo.height = resources.swapchainExtent.height;
            framebufferInfo.layers = 1;
            
            VkResult result = vkCreateFramebuffer(_device, &framebufferInfo, nullptr, &resources.swapchainFramebuffers[i]);
            AF_VK_ASSERT(result == VK_SUCCESS, "Failed to create a framebuffer.");       
        }
    }

    void VulkanAPI::cleanupSwapchain(int windowID)
    {
        VulkanWindowResources resources = _windowResources[windowID];

        for (size_t i = 0; i < resources.swapchainFramebuffers.size(); i++) {
            vkDestroyFramebuffer(_device, resources.swapchainFramebuffers[i], nullptr);
        }
        resources.swapchainFramebuffers.clear();
        for (size_t i = 0; i < resources.swapchainImageViews.size(); i++) {
            vkDestroyImageView(_device, resources.swapchainImageViews[i], nullptr);
        }
        resources.swapchainImageViews.clear();
        vkDestroySwapchainKHR(_device, resources.swapchain, nullptr);
        resources.swapchain = VK_NULL_HANDLE;
    }

    void VulkanAPI::recreateSwapchain(int windowID)
    {
        vkDeviceWaitIdle(_device);

        cleanupSwapchain(windowID);

        createSwapchain(windowID);
        createImageViews(windowID);
        createFramebuffers(windowID);
    }

    void VulkanAPI::createRenderPass()
    {
        VkAttachmentDescription colorAttachment{};
        colorAttachment.format = VK_FORMAT_B8G8R8A8_SRGB;
        colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR ;

        VkAttachmentReference colorAttachmentRef{};
        colorAttachmentRef.attachment = 0;
        colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkSubpassDescription subpass{};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &colorAttachmentRef;

        VkSubpassDependency dependency{};
        dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        dependency.dstSubpass = 0;
        dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.srcAccessMask = 0;
        dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

        VkRenderPassCreateInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassInfo.attachmentCount = 1;
        renderPassInfo.pAttachments = &colorAttachment;
        renderPassInfo.subpassCount = 1;
        renderPassInfo.pSubpasses = &subpass;
        renderPassInfo.dependencyCount = 1;
        renderPassInfo.pDependencies = &dependency;

        VkResult result = vkCreateRenderPass(_device, &renderPassInfo, nullptr, &_renderPass);
        AF_VK_ASSERT(result == VK_SUCCESS, "Failed to create a render pass.");
    }

    void VulkanAPI::createGraphicsPipeline()
    {
        auto vertShaderCode = File::readFile("C:/Users/iplow/Documents/code/Autofilm/Autofilm/src/Shaders/shaders/bin/vert.spv");
        auto fragShaderCode = File::readFile("C:/Users/iplow/Documents/code/Autofilm/Autofilm/src/Shaders/shaders/bin/frag.spv");

        VkShaderModule vertShaderModule = createShaderModule(vertShaderCode);
        VkShaderModule fragShaderModule = createShaderModule(fragShaderCode);

        VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
        vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
        vertShaderStageInfo.module = vertShaderModule;
        vertShaderStageInfo.pName = "main";
        VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
        fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        fragShaderStageInfo.module = fragShaderModule;
        fragShaderStageInfo.pName = "main";
        VkPipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo, fragShaderStageInfo};

        VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
        vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInputInfo.vertexBindingDescriptionCount = 0;
        vertexInputInfo.pVertexBindingDescriptions = nullptr; // Optional
        vertexInputInfo.vertexAttributeDescriptionCount = 0;
        vertexInputInfo.pVertexAttributeDescriptions = nullptr; // Optional

        VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
        inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        inputAssembly.primitiveRestartEnable = VK_FALSE;

        std::vector<VkDynamicState> dynamicStates = {
            VK_DYNAMIC_STATE_VIEWPORT,
            VK_DYNAMIC_STATE_SCISSOR
        };
        VkPipelineDynamicStateCreateInfo dynamicState{};
        dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
        dynamicState.pDynamicStates = dynamicStates.data();

        VkPipelineViewportStateCreateInfo viewportState{};
        viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportState.viewportCount = 1; //WindowManager::getWindows().size();
        viewportState.scissorCount = 1;// WindowManager::getWindows().size();
        // Nullptr because I have dynamic state. Important for multi window application
        viewportState.pViewports = nullptr;
        viewportState.pScissors = nullptr;

        VkPipelineRasterizationStateCreateInfo rasterizer{};
        rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterizer.depthClampEnable = VK_FALSE;
        rasterizer.rasterizerDiscardEnable = VK_FALSE;
        rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
        rasterizer.lineWidth = 1.0f;
        rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
        rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
        rasterizer.depthBiasEnable = VK_FALSE;
        rasterizer.depthBiasConstantFactor = 0.0f; // Optional
        rasterizer.depthBiasClamp = 0.0f; // Optional
        rasterizer.depthBiasSlopeFactor = 0.0f; // Optional

        // Multisampling is disabled for now.
        VkPipelineMultisampleStateCreateInfo multisampling{};
        multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampling.sampleShadingEnable = VK_FALSE;
        multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
        multisampling.minSampleShading = 1.0f; // Optional
        multisampling.pSampleMask = nullptr; // Optional
        multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
        multisampling.alphaToOneEnable = VK_FALSE; // Optional

        VkPipelineColorBlendAttachmentState colorBlendAttachment{};
        colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        colorBlendAttachment.blendEnable = VK_FALSE;
        colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
        colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
        colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD; // Optional
        colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
        colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
        colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD; // Optional

        VkPipelineColorBlendStateCreateInfo colorBlending{};
        colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorBlending.logicOpEnable = VK_FALSE;
        colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
        colorBlending.attachmentCount = 1;
        colorBlending.pAttachments = &colorBlendAttachment;
        colorBlending.blendConstants[0] = 0.0f; // Optional
        colorBlending.blendConstants[1] = 0.0f; // Optional
        colorBlending.blendConstants[2] = 0.0f; // Optional
        colorBlending.blendConstants[3] = 0.0f; // Optional

        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 0; // Optional
        pipelineLayoutInfo.pSetLayouts = nullptr; // Optional
        pipelineLayoutInfo.pushConstantRangeCount = 0; // Optional
        pipelineLayoutInfo.pPushConstantRanges = nullptr; // Optional
        VkResult result = vkCreatePipelineLayout(_device, &pipelineLayoutInfo, nullptr, &_pipelineLayout);
        AF_VK_ASSERT(result == VK_SUCCESS, "Failed to create the pipeline layout.");

        // Bringing it all together
        VkGraphicsPipelineCreateInfo pipelineInfo{};
        pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipelineInfo.stageCount = 2; // vertex and fragment!
        pipelineInfo.pStages = shaderStages;
        pipelineInfo.pVertexInputState = &vertexInputInfo;
        pipelineInfo.pInputAssemblyState = &inputAssembly;
        pipelineInfo.pViewportState = &viewportState;
        pipelineInfo.pRasterizationState = &rasterizer;
        pipelineInfo.pMultisampleState = &multisampling;
        pipelineInfo.pDepthStencilState = nullptr; // Optional
        pipelineInfo.pColorBlendState = &colorBlending;
        pipelineInfo.pDynamicState = &dynamicState;
        pipelineInfo.layout = _pipelineLayout;
        pipelineInfo.renderPass = _renderPass;
        pipelineInfo.subpass = 0;
        pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
        pipelineInfo.basePipelineIndex = -1; // Optional

        VkResult result2 = vkCreateGraphicsPipelines(_device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &_graphicsPipeline);
        AF_VK_ASSERT(result2 == VK_SUCCESS, "Failed to create a graphics pipeline.");
        vkDestroyShaderModule(_device, fragShaderModule, nullptr);
        vkDestroyShaderModule(_device, vertShaderModule, nullptr);
    }

    void VulkanAPI::createRenderFence()
    {
        VkFenceCreateInfo fenceInfo{};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
        for (int i = 0; i < FRAMES_IN_FLIGHT; i++) {
            VkResult result = vkCreateFence(_device, &fenceInfo, nullptr, &_renderFences[i]);
            AF_VK_ASSERT(result == VK_SUCCESS, "Failed to create semaphores.");
        }
    }

    VkShaderModule VulkanAPI::createShaderModule(const std::vector<char>& code) 
    {
        VkShaderModuleCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.codeSize = code.size();
        createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());
        VkShaderModule shaderModule;
        VkResult result = vkCreateShaderModule(_device, &createInfo, nullptr, &shaderModule);
        AF_VK_ASSERT(result == VK_SUCCESS, "Shader module failed");
        return shaderModule;
    }

    bool VulkanAPI::isDeviceSuitable(VkPhysicalDevice device)
    {
        QueueFamilyIndices indices = findQueueFamilies(device);

        bool swapchainAdequate = true;
        for (auto& window : WindowManager::getWindows()) {
            VulkanWindow* vulkanWindow = dynamic_cast<VulkanWindow*>(window.get());
            auto& surface = _windowResources[vulkanWindow->getID()].surface;
            SwapchainSupportDetails swapchainSupport = querySwapchainSupport(device, surface);
            if (swapchainSupport.formats.empty() && swapchainSupport.presentModes.empty()) {
                swapchainAdequate = false;
                break;
            }
        }
        bool extensionsSupported = checkDeviceExtensionsSupport(device);
        VkPhysicalDeviceProperties deviceProperties;
        vkGetPhysicalDeviceProperties(device, &deviceProperties);
        bool isDiscrete = (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU);
        bool result = indices.isComplete() && extensionsSupported && swapchainAdequate && isDiscrete;
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

    VulkanAPI::SwapchainSupportDetails VulkanAPI::querySwapchainSupport(VkPhysicalDevice device, VkSurfaceKHR surface)
    {
        SwapchainSupportDetails details;
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

        uint32_t formatCount;
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
        if (formatCount != 0) {
            details.formats.resize(formatCount);
            vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
        }

        uint32_t presentModeCount;
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data());

        if (presentModeCount != 0) {
            details.presentModes.resize(presentModeCount);
            vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data());
        }
        return details;
    }

    VkSurfaceFormatKHR VulkanAPI::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
    {
        for (const auto& availableFormat : availableFormats) {
            if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
                return availableFormat;
            }
        }
        return availableFormats[0];
    }

    VkPresentModeKHR VulkanAPI::chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) {
        for (const auto& availablePresentMode : availablePresentModes) {
            if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
                return availablePresentMode;
            }
        }

        return VK_PRESENT_MODE_FIFO_KHR;
    }

    VkExtent2D VulkanAPI::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, GLFWwindow* window) {
        if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
            return capabilities.currentExtent;
        } else {
            int width, height;
            glfwGetFramebufferSize(window, &width, &height);

            VkExtent2D actualExtent = {
                static_cast<uint32_t>(width),
                static_cast<uint32_t>(height)
            };

            actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
            actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

            return actualExtent;
        }
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
                auto& surface = _windowResources[vulkanWindow->getID()].surface;
                VkBool32 presentSupport = false;
                vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);
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
        AF_VK_ASSERT(result == VK_SUCCESS, "Failed to create the debug messenger.");
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