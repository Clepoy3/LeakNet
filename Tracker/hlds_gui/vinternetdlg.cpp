//======== (C) Copyright 1999, 2000 Valve, L.L.C. All rights reserved. ========
//
// The copyright to the contents herein is the property of Valve, L.L.C.
// The contents may be used and/or copied only with the written permission of
// Valve, L.L.C., or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: 
//
// $Workfile:     $
// $Date:         $
//
//-----------------------------------------------------------------------------
// $Log: $
//
// $NoKeywords: $
//=============================================================================
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <assert.h>

/*#include "..\common\winlite.h"
// base vgui interfaces
#include <vgui_controls\Controls.h>
#include <VGUI\IInput.h>
#include <VGUI\ISurface.h>
#include <VGUI\IScheme.h>
#include <VGUI\IVGui.h>
#include <VGUI\MouseCode.h>
#include "FileSystem.h"*/

#include <VGUI\MouseCode.h>
#include <VGUI\KeyCode.h>
#include "interface.h"
#include "..\common\winlite.h"
#include <vgui_controls\Controls.h>
#include <vgui_controls\Panel.h>
#include <VGUI\IInput.h>
#include <VGUI\IScheme.h>
#include <VGUI\ISurface.h>
#include <VGUI\ILocalize.h>
#include <VGUI\IVGui.h>
#include "filesystem.h"
//#include "CControlCatalog.h"
#include <stdio.h>

// vgui controls
#include <vgui_controls\Button.h>
#include <vgui_controls\CheckButton.h>
#include <vgui_controls\ComboBox.h>
#include <vgui_controls\FocusNavGroup.h>
#include <vgui_controls\Frame.h>
#include <KeyValues.h>
#include <vgui_controls\ListPanel.h>
#include <vgui_controls\MessageBox.h>
#include <vgui_controls\Panel.h>
#include <vgui_controls\TextEntry.h>
#include <vgui_controls\PropertySheet.h>
#include <vgui_controls\QueryBox.h>



#include "vinternetdlg.h"
#include "hlds.h"

using namespace vgui;

static VInternetDlg *s_InternetDlg = NULL;


//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
VInternetDlg::VInternetDlg( void ) : Frame(NULL, "VInternetDlg")
{
	MakePopup();
	s_InternetDlg=this;

//	start=false;

	SetMinimumSize(480, 400);
	SetSize(480, 400);

	m_pDetailsSheet = new PropertySheet(this, "ServerTabs");
	m_pGameServer = new CGameServer(this, "Server");
	m_pConfigPage = new CCreateMultiplayerGameServerPage(this,"Config");

	LoadControlSettings("Server/dlg.res");

	m_pDetailsSheet->AddPage(m_pConfigPage,"Config");
	m_pDetailsSheet->AddPage(m_pGameServer,"Server");
	m_pDetailsSheet->DisablePage("Server");


}

//-----------------------------------------------------------------------------
// Purpose: Destructor
//-----------------------------------------------------------------------------
VInternetDlg::~VInternetDlg()
{
	m_pGameServer->Stop();
}

//-----------------------------------------------------------------------------
// Purpose: Called once to set up
//-----------------------------------------------------------------------------
void VInternetDlg::Initialize()
{
	SetTitle("Servers", true);
	SetVisible(false);
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void VInternetDlg::Open( void )
{	
//	m_pTabPanel->RequestFocus();
	
	surface()->SetMinimized(GetVPanel(), false);
	SetVisible(true);
	RequestFocus();
	MoveToFront();
}

//-----------------------------------------------------------------------------
// Purpose: relayouts the dialogs controls
//-----------------------------------------------------------------------------
void VInternetDlg::PerformLayout()
{
	BaseClass::PerformLayout();

	int x, y, wide, tall;
	GetClientArea(x, y, wide, tall);
	
	// game list in middle
	m_pDetailsSheet->SetBounds(8, y + 8, GetWide() - 16, tall - (28));
	m_pConfigPage->SetBounds(2,2,wide-30,tall-50);

	// status text along bottom
	Repaint();

}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void VInternetDlg::OnClose()
{
	BaseClass::OnClose();
}

//-----------------------------------------------------------------------------
// Purpose: returns a pointer to a static instance of this dialog
// Output : VInternetDlg
//-----------------------------------------------------------------------------
VInternetDlg *VInternetDlg::GetInstance()
{
	return s_InternetDlg;
}

void VInternetDlg::ConsoleText(const char *text)
{
	m_pGameServer->ConsoleText(text);
}

void VInternetDlg::UpdateStatus(float fps,const char *map,int maxplayers,int numplayers)
{
	m_pGameServer->UpdateStatus(fps,map,maxplayers,numplayers);
}

void VInternetDlg::StartServer(const char *cmdline,const char *cvars)
{
	m_pDetailsSheet->EnablePage("Server");
	m_pDetailsSheet->DisablePage("Config");
	m_pDetailsSheet->SetActivePage(m_pGameServer);

	m_pGameServer->Start(cmdline,cvars);
}
void VInternetDlg::StopServer()
{
	m_pDetailsSheet->EnablePage("Config");
	m_pDetailsSheet->DisablePage("Server");
	m_pDetailsSheet->SetActivePage(m_pConfigPage);
}