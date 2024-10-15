#pragma once

#include "Core/Core.h"
#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"

namespace Autofilm
{
    class AUTOFILM_API Log
    {
    public:

        static void Init();

        inline static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
        inline static std::shared_ptr<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }
        inline static std::shared_ptr<spdlog::logger>& GetVulkanLogger() { return s_VulkanLogger; }
    
    private:
        static std::shared_ptr<spdlog::logger> s_CoreLogger;
        static std::shared_ptr<spdlog::logger> s_ClientLogger;
        static std::shared_ptr<spdlog::logger> s_VulkanLogger;
    };
}

#define AF_CORE_TRACE(...)  ::Autofilm::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define AF_CORE_INFO(...)   ::Autofilm::Log::GetCoreLogger()->info(__VA_ARGS__)
#define AF_CORE_WARN(...)   ::Autofilm::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define AF_CORE_ERROR(...)  ::Autofilm::Log::GetCoreLogger()->error(__VA_ARGS__)

#define AF_TRACE(...)       ::Autofilm::Log::GetClientLogger()->trace(__VA_ARGS__)
#define AF_INFO(...)        ::Autofilm::Log::GetClientLogger()->info(__VA_ARGS__)
#define AF_WARN(...)        ::Autofilm::Log::GetClientLogger()->warn(__VA_ARGS__)
#define AF_ERROR(...)       ::Autofilm::Log::GetClientLogger()->error(__VA_ARGS__)

#define AF_VK_TRACE(...)       ::Autofilm::Log::GetVulkanLogger()->trace(__VA_ARGS__)
#define AF_VK_INFO(...)        ::Autofilm::Log::GetVulkanLogger()->info(__VA_ARGS__)
#define AF_VK_WARN(...)        ::Autofilm::Log::GetVulkanLogger()->warn(__VA_ARGS__)
#define AF_VK_ERROR(...)       ::Autofilm::Log::GetVulkanLogger()->error(__VA_ARGS__)