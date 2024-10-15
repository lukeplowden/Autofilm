#include "Autofilm.h"

class emptyExample : public Autofilm::App
{
    public:
        emptyExample(const Autofilm::ProjectSettings& props)
            : Autofilm::App(props) 
            {
                setup();
            }
        ~emptyExample() {}
        virtual void setup() override;
};

void emptyExample::setup()
{
    AF_INFO("setup called confirmed");
};

Autofilm::App* Autofilm::CreateApp()
{
    ProjectSettings settings;
    settings.renderer = RenderAPIType::Vulkan;
    return new emptyExample(settings);
}  

