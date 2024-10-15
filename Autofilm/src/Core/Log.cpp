#include "autofilmpch.h"
#include "Core/Log.h"

namespace Autofilm
{
    std::shared_ptr<spdlog::logger> Log::s_CoreLogger;
    std::shared_ptr<spdlog::logger> Log::s_ClientLogger;
    std::shared_ptr<spdlog::logger> Log::s_VulkanLogger;
    
    void Log::Init()
    {
        spdlog::set_pattern("%^[%T] %n: %v%$");
        s_CoreLogger = spdlog::stdout_color_mt("Autofilm");
        s_CoreLogger->set_level(spdlog::level::trace);
        
        s_ClientLogger = spdlog::stdout_color_mt("App");
        s_ClientLogger->set_level(spdlog::level::trace);

        s_VulkanLogger = spdlog::stdout_color_mt("Vulkan");
        s_VulkanLogger->set_level(spdlog::level::trace);
    }
}