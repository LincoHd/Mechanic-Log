#pragma once
#include <stdint.h>
#include <stdio.h>
#include <Windows.h>

#include "imgui_panels.h"
#include "Shared.h"
#include "Settings.h"
#include "skill_ids.h"
#include "Keybinds.h"

/* proto/globals */
HMODULE hSelf;
uint32_t cbtcount = 0;
AddonDefinition_t AddonDef{};
void AddonUnload();
void AddonOptions();
UINT mod_wnd(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void mod_combat_local(void*);
void mod_combat_squad(void*);
void mod_combat(bool, void*);
void ShowMechanicsChart(bool*);
void ShowMechanicsLog(bool*);
void AddonRender();
void AddonKeybindHandle(const char*, bool);

AppLog log_ui;
AppChart chart_ui;
AppOptions options_ui;
bool chartOpen = false;
bool logOpen = false;

Tracker tracker;

/* arcdps exports */
void* filelog;
void* arclog;

/* dll main -- winapi */
BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH: hSelf = hModule; break;
	case DLL_PROCESS_DETACH: break;
	case DLL_THREAD_ATTACH: break;
	case DLL_THREAD_DETACH: break;
	}
	return TRUE;
}

void OnAddonLoaded(void* aSignature)
{
	if (!aSignature) { return ;  }
  
	if ((int)aSignature == RTAPI_SIG)
	{
		RTAPIData = (RTAPI::RealTimeData*)Addon_API->DataLink_Get(DL_RTAPI);
	}
}


void AddonLoad(AddonAPI_t* aApi)
{
	Addon_API = aApi;
	ImGui::SetCurrentContext((ImGuiContext*)Addon_API->ImguiContext);
	ImGui::SetAllocatorFunctions((void* (*)(size_t, void*))Addon_API->ImguiMalloc, (void(*)(void*, void*))Addon_API->ImguiFree); // on imgui 1.80+
	RTAPIData = (RTAPI::RealTimeData*)Addon_API->DataLink_Get(DL_RTAPI);

	Addon_API->Events_Subscribe("EV_ARCDPS_COMBATEVENT_LOCAL_RAW", mod_combat_local);
	Addon_API->Events_Subscribe("EV_ARCDPS_COMBATEVENT_SQUAD_RAW", mod_combat_squad);
	Addon_API->Events_Subscribe(EV_ADDON_LOADED, OnAddonLoaded);
	
	Keybinds::Register();
	
	Addon_API->GUI_Register(RT_OptionsRender, AddonOptions);
	Addon_API->GUI_Register(RT_Render, AddonRender);
	
	AddonPath = Addon_API->Paths_GetAddonDirectory("Mechanic-Log");
	SettingsPath = Addon_API->Paths_GetAddonDirectory("Mechanic-Log\\settings.json");
	std::filesystem::create_directory(AddonPath);
	Settings::Load(SettingsPath);
	
	Addon_API->Log(LOGL_INFO, channelName, "done mod_init");
}

extern "C" __declspec(dllexport) AddonDefinition_t* GetAddonDef()
{
	AddonDef.Signature = -0xa84d7206;
	AddonDef.APIVersion = NEXUS_API_VERSION;
	AddonDef.Name = "Mechanic Log";
	AddonDef.Version.Major = 1;
	AddonDef.Version.Minor = 0;
	AddonDef.Version.Build = 0;
	AddonDef.Version.Revision = 1;
	AddonDef.Author = "Linco";
	AddonDef.Description = "Shows successful and failed mechanics. Special Thanks towards Knoxfighter and MarsEdge for the original arcdps mechanic log.";
	AddonDef.Load = &AddonLoad;
	AddonDef.Unload = &AddonUnload;
	AddonDef.Flags = EAddonFlags::AF_None;

	return &AddonDef;
}

void OnAddonUnloaded(void* aSignature)
{
	if (!aSignature) { return; }

	if ((int)aSignature == RTAPI_SIG)
	{
		RTAPIData = nullptr;
	}
}

void AddonUnload()
{
	Addon_API->GUI_Deregister(AddonOptions);
	Addon_API->GUI_Deregister(AddonRender);
	
	Keybinds::Cleanup();
	
	Addon_API->Events_Unsubscribe("EV_ARCDPS_COMBATEVENT_LOCAL_RAW", mod_combat_local);
	Addon_API->Events_Unsubscribe("EV_ARCDPS_COMBATEVENT_SQUAD_RAW", mod_combat_squad);
	Addon_API->Events_Unsubscribe(EV_ADDON_UNLOADED, OnAddonUnloaded);
	if(Settings::export_chart_on_close) chart_ui.writeToDisk(&tracker);
	tracker.resetAllPlayerStats();
	Settings::Save(SettingsPath);
}

/* wrapper functions simply to log to different nexus channels */
void mod_combat_local(void* aEventArgs)
{
	mod_combat(true, aEventArgs);
}
void mod_combat_squad(void* aEventArgs)
{
	mod_combat(true, aEventArgs);
}

/* combat callback -- may be called asynchronously, use id param to keep track of order, first event id will be 2. return ignored */
/* at least one participant will be party/squad or minion of, or a buff applied by squad in the case of buff remove. not all statechanges present, see evtc statechange enum */
void mod_combat(bool aIsLocal, void* aEventArgs)
{
	EvCombatData* cbtEv = (EvCombatData*)aEventArgs;
	PlayerEntry* current_entry = nullptr;

	/* ev is null. dst will only be valid on tracking add. skillname will also be null */
	if (!cbtEv->ev)
	{
		if (!cbtEv->src->elite)
		{
	
			/* notify tracking change */
			if (isPlayer(cbtEv->src) && !cbtEv->src->elite)
			{
				/* add */
				if (cbtEv->dst && cbtEv->src->prof)
				{
					tracker.addPlayer(cbtEv->src,cbtEv->dst);
				}

				/* remove */
				else
				{
					tracker.removePlayer(cbtEv->src);
				}
			}
		}
	}

	/* combat event. skillname may be null. non-null skillname will remain static until module is unloaded. refer to evtc notes for complete detail */
	else
	{

		/* common */

		/* statechange */
		if (cbtEv->ev->is_statechange)
		{
			switch(cbtEv->ev->is_statechange)
			{
			case CBTS_ENTERCOMBAT:
				tracker.processCombatEnter(cbtEv->ev, cbtEv->src);
				break;
			case CBTS_EXITCOMBAT:
				tracker.processCombatExit(cbtEv->ev, cbtEv->src);
				break;
			case CBTS_CHANGEUP:
				//TODO: make these into process functions in tracker.cpp
				if((current_entry = tracker.getPlayerEntry(cbtEv->src)))
				{
					current_entry->rally();
				}
				break;
			case CBTS_CHANGEDEAD:
				if((current_entry = tracker.getPlayerEntry(cbtEv->src)))
				{
					current_entry->dead();
				}
				break;
			case CBTS_CHANGEDOWN:
				if((current_entry = tracker.getPlayerEntry(cbtEv->src)))
				{
					current_entry->down();
				}
				break;
			case CBTS_LOGNPCUPDATE:
				tracker.processLogNpcUpdate(cbtEv->ev->src_agent);
				break;
			case CBTS_BUFFAPPLY:
				if (cbtEv->ev->skillid==BUFF_STABILITY)//if it's stability
				{
					if(current_entry = tracker.getPlayerEntry(cbtEv->dst))
					{
						current_entry->setStabTime(cbtEv->ev->time+cbtEv->ev->value+ms_per_tick);//add prediction of when new stab will end
					}
				}
				break;
			case CBTS_BUFFREMOVE_SINGLE:
				if (cbtEv->ev->skillid==BUFF_STABILITY)//if it's stability
				{
					if(current_entry = tracker.getPlayerEntry(cbtEv->dst))
					{
					

						current_entry->setStabTime(cbtEv->ev->time+ms_per_tick);//cut the ending time of stab early
					}
				}
				else if (cbtEv->ev->skillid==BUFF_VAPOR_FORM//vapor form manual case
						 || cbtEv->ev->skillid==BUFF_ILLUSION_OF_LIFE//Illusion of Life manual case
						 )
				{
					if(current_entry = tracker.getPlayerEntry(cbtEv->dst))
					{
						current_entry->fixDoubleDown();
					}
				}
				break;
			}
		}

		/* activation */
		else if (cbtEv->ev->is_activation)
		{

		}

		/* buff remove */
		else if (cbtEv->ev->is_buffremove)
		{

		}

		/* buff */
		else if (cbtEv->ev->buff)
		{

		}

		if(cbtEv->ev->result != CBTR_INTERRUPT && cbtEv->ev->result != CBTR_BLIND)
		{
			int64_t value = 0;
			current_entry = tracker.getPlayerEntry(cbtEv->src);
			PlayerEntry* other_entry = tracker.getPlayerEntry(cbtEv->dst);
			for(uint16_t index=0;index<getMechanics().size();index++)
			{
				if(value = getMechanics()[index].isValidHit(cbtEv->ev, cbtEv->src, cbtEv->dst,
					(current_entry ? current_entry->player : nullptr), //check for null before getting player object
					(other_entry ? other_entry->player: nullptr)))
				{
					if (getMechanics()[index].is_combat_buff && cbtEv->ev->is_statechange == CBTS_BUFFAPPLY)
					{
						getMechanics()[index].is_combat_buff = false;
					}
					tracker.processMechanic(cbtEv->ev, current_entry, other_entry, &getMechanics()[index], value);
					log_ui.scroll_to_bottom = true;
				}
			}
		}
	}
}

void AddonOptions()
{
	options_ui.draw(&tracker);
}

void ShowMechanicsChart(bool* p_open)
{
	if (Settings::show_app_chart)
	{
		chart_ui.draw(&tracker, "Mechanics Chart", p_open, ImGuiWindowFlags_NoCollapse); //TODO:  ImGuiWindowFlags_NoCollapse | (!canMoveWindows() ? ImGuiWindowFlags_NoMove : 0 
	}
}

void ShowMechanicsLog(bool* p_open)
{
	if(Settings::show_app_log) log_ui.draw("Mechanics Log", p_open, ImGuiWindowFlags_NoCollapse, &tracker); //TODO:  ImGuiWindowFlags_NoCollapse | (!canMoveWindows() ? ImGuiWindowFlags_NoMove : 0 
}

void AddonRender()
{
	if (Settings::show_app_chart)
	{
		chartOpen = true;
		ShowMechanicsChart( &chartOpen);
	}
	if (Settings::show_app_log)
	{
		logOpen = true;
		ShowMechanicsLog( &logOpen);
	}
	
	if (!chartOpen && Settings::show_app_chart)
	{
		Settings::show_app_chart = !Settings::show_app_chart;
		Settings::Settings[IS_MECHANI_CHART_VISIBLE] = Settings::show_app_chart;
		Settings::Save(SettingsPath);
	}
	
	if (!logOpen && Settings::show_app_log)
	{
		Settings::show_app_log = !Settings::show_app_log;
		Settings::Settings[IS_MECHANI_LOG_VISIBLE] = Settings::show_app_log;
		Settings::Save(SettingsPath);
	}
}

