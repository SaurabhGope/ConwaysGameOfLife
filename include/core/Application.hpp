#pragma once

#include "core/AppConfig.hpp"

class Application
{
public:
    explicit Application(AppConfig config);
    int Run();

private:
    AppConfig m_config;
};
