#pragma once


namespace Keybinds
{
    __declspec(selectany)  const char* KeyBindChartLabel = "Mechanic Chart Toggle";
    __declspec(selectany)  const char* KeyBindLogLabel = "Mechanic Log Toggle ";
    __declspec(selectany)  const char* kbDefChart = "(null)";
    __declspec(selectany)  const char* kbDefLog = "(null)";
    
    void Register();
    void Cleanup();
    void Handle(const char*, bool);
}
