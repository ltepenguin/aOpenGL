#include <aOpenGL.h>

class MyApp : public agl::App
{
public:
    void update() override
    {

    }

    void render() override
    {
        agl::Render::sphere()
            ->position(0, 0, 0)
            ->color(1, 1, 0)
            ->draw();
    }

    void key_callback(char key, int action) override
    {
        if(action != GLFW_PRESS)
            return;
        
        if(key == '1')
            this->capture(true);
        if(key == '2')
            this->capture(false);
    }
    
};

int main(int argc, char* argv[])
{
    MyApp app;
    agl::AppManager::start(&app);
    return 0;
} 