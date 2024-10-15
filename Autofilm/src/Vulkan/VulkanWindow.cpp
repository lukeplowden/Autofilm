#include "autofilmpch.h"
#include "Vulkan/VulkanWindow.h"
#include "Core/Log.h"

namespace Autofilm
{
    static bool s_GLFWInitialised = false;

    // Window* Window::Create(const WindowProperties& props)
    // {
    //     return new VulkanWindow(props);
    // }
 
    VulkanWindow::VulkanWindow(const WindowProperties& props)
    {
        init(props);
    }

    VulkanWindow::~VulkanWindow()
    {
        shutdown();
    }

    void VulkanWindow::init(const WindowProperties& props)
    {

    }

    void VulkanWindow::shutdown()
    {
    }

    void VulkanWindow::onUpdate()
    {
    }

    void VulkanWindow::setVSync(bool enabled)
    {
        if (enabled){
            glfwSwapInterval(1);
        } else {
            glfwSwapInterval(0);
        }
        m_data.VSync = enabled;
    }

    bool VulkanWindow::isVSync() const
    {
        return m_data.VSync;
    }

    void VulkanWindow::setFullscreen(bool fullscreen)
    {
        return;
    }

    bool VulkanWindow::isFullscreen() const
    {
        return m_data.fullscreen;
    }
}
