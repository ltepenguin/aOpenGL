#include <aOpenGL.h>
#include <iostream>

#include <aOpenGL.h>

class MyApp : public agl::App
{
public:

    void render() override
    {
        // draw plane
        agl::Render::sphere()
            ->color(1.0, 0.7, 0.5)
            ->scale(0.5f)
            ->position(Vec3(0, 1, 0))
            ->draw();

        agl::Render::text("Render sphere\nRender text", 1.5f)
            ->scale(0.5f)
            ->position(Vec3(0.3f, 1, 0))
            ->draw();
    }
};

int main(int argc, char* argv[])
{
    MyApp app;
    agl::AppManager::start(&app);
    return 0;
}
