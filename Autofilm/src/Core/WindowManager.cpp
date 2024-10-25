#include "autofilmpch.h"
#include "Core/Core.h"
#include "Core/Window.h"
#include "Renderer/RenderAPI.h"
#include "Vulkan/VulkanWindow.h"

namespace Autofilm
{
    std::vector<std::unique_ptr<Window>> WindowManager::_windows;
    RenderAPIType WindowManager::_type = RenderAPIType::None;
    std::function<std::unique_ptr<Window>(const WindowProperties&, unsigned int ID)> WindowManager::_createWindowFunc;
    unsigned int WindowManager::nextID = 0;
    
    void WindowManager::Init(RenderAPIType type)
    {
        _type = type;
        switch (_type) {
            case RenderAPIType::Vulkan:
                _createWindowFunc = [](const WindowProperties& props, unsigned int ID) {
                    return std::make_unique<VulkanWindow>(props, ID);
                };
                break;
            default:
                AF_CORE_ERROR("Invalid renderer type selected or the platform is not supported.");
                _createWindowFunc = nullptr;
                break;
        }
        nextID = 0;
    }

    // I should change this to have less heap allocations ...
    void WindowManager::createWindow(const WindowProperties&  props)
    {
        if (_createWindowFunc) {
            _windows.push_back(_createWindowFunc(props, nextID++));
        } else {
            AF_CORE_ERROR("No valid renderer type initialized.");
        }
        // _windows.back().get()->setEventCallback(AF_BIND_EVENT_FN(App::onEvent));
    }

    const std::vector<std::unique_ptr<Window>>& WindowManager::getWindows()
    {
        return _windows;
    }

    void WindowManager::destroyWindow(int ID)
    {
        auto window = _windows[ID].get();
        window->shutdown();
    }
}