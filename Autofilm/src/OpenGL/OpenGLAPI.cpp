#include "autofilmpch.h"
#include "OpenGL/OpenGLAPI.h"
#include "Core/Log.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace Autofilm
{
    void OpenGLAPI::init()
    {
        int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
        AF_CORE_ASSERT(status, "Failed to initialize Glad");
    }

    void OpenGLAPI::shutdown()
    {

    }

    void OpenGLAPI::clearColor(const glm::vec4& color)
    {
        glClearColor(color.r, color.g, color.b, color.a);
    }

    void OpenGLAPI::clear()
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }
}
