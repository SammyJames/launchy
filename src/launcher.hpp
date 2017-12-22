#pragma once

#include "types.hpp"

struct LauncherInfo
{
    string m_address;
    u32 m_port;
    u32 m_password_mode;
    u32 m_version;
};

LauncherInfo RequestLauncherInfo();