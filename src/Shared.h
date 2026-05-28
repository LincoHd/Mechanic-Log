#pragma once

#ifndef SHARED_H
#define SHARED_H

#include <filesystem>
#include <string>

#include "nexus/Nexus.h"
#include "rtapi/RTAPI.hpp"
#include "imgui/imgui.h"

extern AddonAPI_t* Addon_API;
extern const char* channelName;
extern std::filesystem::path SettingsPath;
extern std::filesystem::path AddonPath;

extern NexusLinkData_t* NexusLink;
extern RTAPI::RealTimeData* RTAPIData;

#endif
