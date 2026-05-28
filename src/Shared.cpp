#include "Shared.h"

AddonAPI_t* Addon_API;
std::filesystem::path SettingsPath;
std::filesystem::path AddonPath;
const char* channelName = "Mechanic Log";
NexusLinkData_t* NexusLink = nullptr;
RTAPI::RealTimeData* RTAPIData = nullptr;