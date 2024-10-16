#include "autofilmpch.h"
#include "Core/Core.h"
#include "Core/Window.h"
#include "Renderer/RenderAPI.h"
#include "OpenGL/OpenGLWindow.h"
#include "Vulkan/VulkanWindow.h"

namespace Autofilm
{
    std::vector<std::unique_ptr<Window>> WindowManager::_windows;
    RenderAPIType WindowManager::_type = RenderAPIType::None;
    std::function<std::unique_ptr<Window>(const WindowProperties&)> WindowManager::_createWindowFunc;
    
    void WindowManager::Init(RenderAPIType type)
    {
        _type = type;
        switch (_type) {
            case RenderAPIType::Vulkan:
                _createWindowFunc = [](const WindowProperties& props) {
                    return std::make_unique<VulkanWindow>(props);
                };
                break;
            case RenderAPIType::OpenGL:
                _createWindowFunc = [](const WindowProperties& props) {
                    return std::make_unique<OpenGLWindow>(props);
                };
                break;
            default:
                AF_CORE_ERROR("Invalid renderer type selected or the platform is not supported.");
                _createWindowFunc = nullptr;
                break;
        }
    }

    void WindowManager::createWindow(const WindowProperties&  props)
    {
        if (_createWindowFunc) {
            _windows.push_back(_createWindowFunc(props));
        } else {
            AF_CORE_ERROR("No valid renderer type initialized.");
        }
    }

    const std::vector<std::unique_ptr<Window>>& WindowManager::getWindows()
    {
        return _windows;
    }
}