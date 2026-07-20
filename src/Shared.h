#pragma once

#ifndef SHARED_H
#define SHARED_H

#include <filesystem>

#include "nexus/Nexus.h"
#include "rtapi/RTAPI.hpp"
#include "mumble/mumble.h"

extern AddonAPI_t* Addon_API;
extern const char* channelName;
extern std::filesystem::path SettingsPath;
extern std::filesystem::path AddonPath;
extern NexusLinkData_t* NexusLink;
extern RTAPI::RealTimeData* RTAPIData;
extern Mumble::Data* MumbleLink;

#endif
