#include "Keybinds.h"

#include "Settings.h"
#include  "Shared.h"

namespace Keybinds
{
    void Keybinds::Register()
    {
        Addon_API->InputBinds_RegisterWithString(Keybinds::KeyBindChartLabel, Handle, kbDefChart);
        Addon_API->InputBinds_RegisterWithString(Keybinds::KeyBindLogLabel, Handle, kbDefLog);
    }

    void Keybinds::Handle(const char* aIdentifier, bool aIsRelease)
    {
        if (strcmp(aIdentifier, KeyBindChartLabel) == 0 && !aIsRelease)
        {
            Settings::show_app_chart =  !Settings::show_app_chart;
            Settings::Settings[IS_MECHANI_CHART_VISIBLE] = Settings::show_app_chart;
            Settings::Save(SettingsPath);
        }
        else if (strcmp(aIdentifier, KeyBindLogLabel) == 0 && !aIsRelease)
        {
            Settings::show_app_log = !Settings::show_app_log;
            Settings::Settings[IS_MECHANI_LOG_VISIBLE] = Settings::show_app_log;
            Settings::Save(SettingsPath);
        }
    }
    
    
    void Cleanup()
    {
        Addon_API->InputBinds_Deregister(Keybinds::KeyBindChartLabel);
        Addon_API->InputBinds_Deregister(Keybinds::KeyBindChartLabel);
    }
}



