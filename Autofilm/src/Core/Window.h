#pragma once

#include "autofilmpch.h"
#include "Core/Core.h"

namespace Autofilm
{
    struct WindowProperties
    {
        std::string title;
        unsigned int width;
        unsigned int height;

        WindowProperties(
                        const std::string& title = "Autofilm", 
                        unsigned int width = 1280,
                        unsigned int height = 720
                    )
            : title(title), width(width), height(height)
        {}
    };

    class AUTOFILM_API Window
    {
    public:
        // using eventCallbackFn = std::function<void(Event&)>;

        virtual ~Window() {}

        virtual void onUpdate() = 0;

        virtual unsigned int getWidth() const = 0;
        virtual unsigned int getHeight() const = 0;
    
        // Window attributes
        // virtual void setEventCallback(const eventCallbackFn& callback) = 0;
        virtual void setVSync(bool enabled) = 0;
        virtual bool isVSync() const = 0;
        virtual void setFullscreen(bool fullscreen) = 0;
        virtual bool isFullscreen() const = 0;

        private:
            int _id;
    };

    enum class RenderAPIType;
    class AUTOFILM_API WindowManager
    {
    public:
        static void Init(RenderAPIType type);
        static void createWindow(const WindowProperties&  props = WindowProperties());
        static void destroyWindow(){};

        static const std::vector<std::unique_ptr<Window>>& getWindows();
    private:
        static std::vector<std::unique_ptr<Window>> _windows;
        static RenderAPIType _type;
        static std::function<std::unique_ptr<Window>(const WindowProperties&)> _createWindowFunc;
    };
}