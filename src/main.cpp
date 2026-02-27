#include "Core/Application.h"

int main() {
    Core::Application application;
    if (!application.Init()) {
        return 1;
    }

    application.Run();
    application.Shutdown();
    return 0;
}
