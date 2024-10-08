#include "Autofilm.h"

class emptyExample : public Autofilm::App
{
    public:
        emptyExample(const Autofilm::AppProperties& props)
            : Autofilm::App(props) 
            {

            }
        ~emptyExample() {}
};

Autofilm::App* Autofilm::CreateApp()
{
    AppProperties props;
    props.multiWindow = false;
    return new emptyExample(props);
}  