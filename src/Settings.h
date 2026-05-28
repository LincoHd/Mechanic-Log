#pragma once

#include <mutex>

#include "nlohmann/json.hpp"
using json = nlohmann::json;
#include <filesystem>
#include <fstream>

#include <mutex>

extern const char* IS_MECHANI_CHART_VISIBLE;
extern const char* IS_MECHANI_LOG_VISIBLE;
extern const char* EXPORT_DIR;
extern const char* 

namespace Settings
{
    extern std::mutex	Mutex;
    extern json			Settings;
    /* Loads the settings. */
    void Load(std::filesystem::path aPath);
    /* Saves the settings. */
    void Save(std::filesystem::path aPath);
    
    extern bool show_app_chart;
    extern bool show_app_log;
    extern std::string export_dir;
}