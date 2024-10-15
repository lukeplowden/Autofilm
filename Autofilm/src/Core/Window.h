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

        WindowProperties(const std::string& title = "Autofilm", 
                        unsigned int width = 1280,
                        unsigned int height = 720)
            : title(title), width(width), height(height)
        {}
    };

    class AUTOFILM_API WindowManager
    {
    public:
        WindowManager();
        virtual ~WindowManager() {}
        
        virtual void createWindow(const WindowProperties&  props = WindowProperties());
        virtual void destroyWindow();
    private:
        std::vector<std::unique_ptr<Window>> _windows;
        // virtual void createWindow
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

        static Window* Create(const WindowProperties&  props = WindowProperties());

        private:
            int _id;
            friend class WindowManager;
    };
}