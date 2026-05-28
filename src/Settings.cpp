#include "Settings.h"
#include "mechanics.h"
#include "Shared.h"

const char* IS_MECHANI_CHART_VISIBLE = "IsMechanicChartVisible";
const char* IS_MECHANI_LOG_VISIBLE = "IsMechanicLogVisible";
const char* EXPORT_DIR = "";
const char* LOG_MAX_MECHANICS = "LogMaxMechanics";
const char* EXPORT_ON_CLOSE = "ExportOnClose";
const char* IS_SELF_STATS = "SelfStatistics";

namespace Settings
{
    std::mutex	Mutex;
    json		Settings = json::object();
    bool show_app_chart = false;
    bool show_app_log = false;
    int max_log_events = 300;
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
        
        for (const Boss* boss : bosses)
        {
            if (boss->name == "Generic" || boss->name == "Mursaat Overseer" || boss->name == "Skorvald the Shattered" || boss->name == "Artsariiv") { continue; }
            for (auto current_mechanic = getMechanics().begin(); current_mechanic != getMechanics().end(); ++current_mechanic)
            {
                if (current_mechanic->boss->name == boss->name)
                {
                    std::string_view tmp = current_mechanic->boss->name + ":" + current_mechanic->name;
                    if (!Settings[tmp].is_null())
                    {
                        Settings[tmp].get_to<int>(current_mechanic->verbosity);
                    }
                }
            }
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