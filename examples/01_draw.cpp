#include <aOpenGL.h>

class MyApp : public agl::App
{
public:

    void render() override
    {
        // draw plane
        agl::Render::plane()
            ->scale(10.0f)
            ->floor_grid(true)
            ->draw();

        // draw cubes
        for(int i = 0; i < 10; ++i)
        {
            agl::Render::cube()
                ->position(i - 5.0f, 0.5f, 0.0f)
                ->scale(0.5f)
                ->color(i * 0.1, 0, 0)
                ->draw();
        }
    }
};

int main(int argc, char* argv[])
{
    MyApp app;
    agl::AppManager::start(&app);
    return 0;
}