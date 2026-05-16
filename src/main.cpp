#include "core/Application.hpp"

int main()
{
    AppConfig config{};
    Application app(config);
    return app.Run();
}
