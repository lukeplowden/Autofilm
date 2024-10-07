#include "Autofilm.h"

class emptyExample : public Autofilm::App
{
    public:
        emptyExample() {}
        ~emptyExample() {}
};

Autofilm::App* Autofilm::CreateApp()
{
    return new emptyExample();
}