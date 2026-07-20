#pragma once

#include <mutex>

#include "nlohmann/json.hpp"
using json = nlohmann::json;
#include <filesystem>

extern const char* IS_MECHANI_CHART_VISIBLE;
extern const char* IS_MECHANI_LOG_VISIBLE;
extern const char* EXPORT_DIR;
extern const char* LOG_MAX_MECHANICS;
extern const char* EXPORT_ON_CLOSE;
extern const char* IS_SELF_STATS;
extern const char* MAX_NAME_DISPLAY_LENGTH;

namespace Settings
{
    extern std::mutex	Mutex;
    extern json			Settings;
    /* Loads the settings. */
    void Load(std::filesystem::path aPath);
    /* Saves the settings. */
    void Save(std::filesystem::path aPath);
    void SaveMechanicSettings(std::filesystem::path aPath);
    
    extern bool show_app_chart;
    extern bool show_app_log;
    extern int max_log_events;
    extern bool show_only_self;
    extern bool export_chart_on_close;
    extern std::string export_dir;
    extern int max_name_display_length;
}
