#include "Settings.h"
#include "mechanics.h"
#include "Shared.h"
#include <fstream>

const char* IS_MECHANI_CHART_VISIBLE = "IsMechanicChartVisible";
const char* IS_MECHANI_LOG_VISIBLE = "IsMechanicLogVisible";
const char* EXPORT_DIR = "";
const char* LOG_MAX_MECHANICS = "LogMaxMechanics";
const char* EXPORT_ON_CLOSE = "ExportOnClose";
const char* IS_SELF_STATS = "SelfStatistics";
const char* MAX_NAME_DISPLAY_LENGTH = "MaxNameDisplayLength";

namespace Settings
{
    std::mutex	Mutex;
    json		Settings = json::object();
    bool show_app_chart = false;
    bool show_app_log = false;
    int max_log_events = 300;
    int max_name_display_length = 300;
    bool show_only_self = false;
    bool export_chart_on_close = false;
    std::string export_dir;
    
    void Load(std::filesystem::path aPath)
    {
        if (!std::filesystem::exists(aPath)) { return; }
        Mutex.lock();
        {
            try
            {
                std::ifstream file(aPath);
                Settings = json::parse(file);
                file.close();
            }
            catch (json::parse_error& ex)
            {
                Addon_API->Log(LOGL_WARNING, channelName, "Settings.json could not be parsed.");
                Addon_API->Log(LOGL_WARNING, channelName, ex.what());
            }
        }
        Mutex.unlock();
        
        if (!Settings[IS_MECHANI_CHART_VISIBLE].is_null())
        {
            Settings[IS_MECHANI_CHART_VISIBLE].get_to<bool>(show_app_chart);
        }
        
        if (!Settings[IS_MECHANI_LOG_VISIBLE].is_null())
        {
            Settings[IS_MECHANI_LOG_VISIBLE].get_to<bool>(show_app_log);
        }
        
        if (!Settings[LOG_MAX_MECHANICS].is_null())
        {
            Settings[LOG_MAX_MECHANICS].get_to<int>(max_log_events);
        }
        
        if (!Settings[MAX_NAME_DISPLAY_LENGTH].is_null())
        {
            Settings[MAX_NAME_DISPLAY_LENGTH].get_to<int>(max_name_display_length);
        }
        
        if (!Settings[EXPORT_ON_CLOSE].is_null())
        {
            Settings[EXPORT_ON_CLOSE].get_to<bool>(export_chart_on_close);
        }
        
        if (!Settings[IS_SELF_STATS].is_null())
        {
            Settings[IS_SELF_STATS].get_to<bool>(show_only_self);
        }
        
        //TODO LOAD RIGHT.
        for (auto& current_mechanic: getMechanics())
        {
            std::string tmp = current_mechanic.getIniName();
            
            if (!Settings[tmp].is_null())
            {
                Settings[tmp].get_to<int>(current_mechanic.verbosity);
            }
        }
    }
    
    void Save(std::filesystem::path aPath)
    {
        Mutex.lock();
        try
            {
                std::ofstream file(aPath);
                file << Settings.dump(1, '\t') << std::endl;
                file.close();
            }
            catch (json::exception ex)
            {
                Addon_API->Log(LOGL_WARNING, channelName, "Could not be saved.");
                Addon_API->Log(LOGL_WARNING, channelName, ex.what());
            }
        Mutex.unlock();
    }
    
    void SaveMechanicSettings(std::filesystem::path aPath)
    {
        for (auto current_mechanic: getMechanics())
        {
            std::string tmp = current_mechanic.getIniName();
            Settings[tmp] = current_mechanic.verbosity;
        }
    }
    
}