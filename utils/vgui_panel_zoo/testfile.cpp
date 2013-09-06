/*
#include "interface.h"
#include "..\..\tracker\common\winlite.h"
#include <VGUI_Controls.h>
#include <VGUI_Panel.h>
#include <VGUI_IScheme.h>
#include <VGUI_ISurface.h>
#include <VGUI_ILocalize.h>
#include <VGUI_IVGui.h>
#include "filesystem.h"

#include "CControlCatalog.h"

#include <stdio.h>
*/
#include <VGUI\IScheme.h>
#include "interface.h"
#include "..\..\tracker\common\winlite.h"
#include <vgui_controls\Controls.h>
#include <VGUI\MouseCode.h>
#include <VGUI\KeyCode.h>
#include <VGUI\IVGui.h>
#include <VGUI\ISurface.h>
#include <VGUI\ILocalize.h>
#include <vgui_controls\Panel.h>
#include "utlbuffer.h"
#include "filesystem.h"

#include "CControlCatalog.h"

#include <stdio.h>

//-----------------------------------------------------------------------------
// Purpose: Entry point
//			loads interfaces and initializes dialog
//-----------------------------------------------------------------------------
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	// Load vgui
	CSysModule *vguiModule = Sys_LoadModule("bin/vgui2.dll");
	if (!vguiModule)
	{
		vguiModule = Sys_LoadModule("vgui2.dll");
	}

	CreateInterfaceFn vguiFactory = Sys_GetFactory(vguiModule);
	if (!vguiFactory)
	{
		printf("Fatal error: Could not load vgui2.dll\n");
		return 2;
	}	  

	CSysModule *filesystemModule = Sys_LoadModule("bin/filesystem_stdio.dll");
	if (!filesystemModule)
	{
		filesystemModule = Sys_LoadModule("filesystem_stdio.dll");
	}

	CreateInterfaceFn filesystemFactory = Sys_GetFactory(filesystemModule);
	if (!filesystemFactory)
	{
		printf("Fatal error: Could not load bin/filesystem_stdio.dll\n");
		return 2;
	}	  

	// Initialize interfaces
	CreateInterfaceFn factories[2];
	factories[0] = vguiFactory;
	factories[1] = filesystemFactory;

	if (!vgui::VGui_InitInterfacesList("Tracker", factories, 2))
	{
		printf("Fatal error: Could not initalize vgui2.dll\n");
		return 3;
	}
	
	// In order to load resource files the file must be in your vgui filesystem path.
//	vgui::filesystem()->AddSearchPath("../", "resources");
//	vgui::filesystem()->AddSearchPath("../", "");
	vgui::filesystem()->AddSearchPath("../platform/", ""); // VXP: From default bin path

	// Init the surface
	vgui::surface()->Init();

	// Load the scheme
	if (!vgui::scheme()->LoadSchemeFromFile("Resource/TrackerScheme.res", "Tracker"))
		return 1;

	// localization
	vgui::localize()->AddFile(vgui::filesystem(), "Resource/platform_english.txt");
	vgui::localize()->AddFile(vgui::filesystem(), "Resource/vgui_english.txt");

	// Make a embedded panel
	vgui::Panel *panel = new vgui::Panel(NULL, "TopPanel");
	vgui::surface()->SetEmbeddedPanel( panel->GetVPanel() );

	// Start vgui
	vgui::ivgui()->Start();

	// Add our main window
	CControlCatalog *panelZoo = new CControlCatalog();
	panelZoo->Activate();

	// Run app frame loop
	while (vgui::ivgui()->IsRunning())
	{
		vgui::ivgui()->RunFrame();
	}

	// Shutdown
	vgui::surface()->Shutdown();

	delete panelZoo;
//	delete panel;

	Sys_UnloadModule(vguiModule);
	return 1;
}






