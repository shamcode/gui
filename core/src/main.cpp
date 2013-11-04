#include "core/application.h"
#include "core/defaultscene.h"

int main(int argc, char *argv[])
{
    using namespace core;

    CApplication *app = CApplication::getInstance();
    app->initialize(argc, argv);
    app->setWindowTitle("Test");
    app->setFullScreen(false);
    app->setSize(glm::ivec2(800, 600));
    app->setColorDepth(EColorDepth::_32);

    DefaultScene scene;
    app->setScene(&scene);

    if (app->init())
        app->exec();
    app->clear();

    return 0;
}
