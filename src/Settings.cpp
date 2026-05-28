#include "Settings.h"
#include "mechanics.h"
#include "Shared.h"

const char* IS_MECHANI_CHART_VISIBLE = "IsMechanicChartVisible";
const char* IS_MECHANI_LOG_VISIBLE = "IsMechanicLogVisible";
const char* EXPORT_DIR = "";

namespace Settings
{
    std::mutex	Mutex;
    json		Settings = json::object();
    bool show_app_chart = false;
    bool show_app_log = false;
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
    }
    
    void Save(std::filesystem::path aPath)
    {
        Mutex.lock();
        {
            std::ofstream file(aPath);
            file << Settings.dump(1, '\t') << std::endl;
            file.close();
        }
        Mutex.unlock();
        Addon_API->Log(LOGL_INFO, channelName, aPath.string().c_str());
        
    }
    
}