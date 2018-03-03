//========= Copyright © 1996-2001, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================

#include "vinternetdlg.h"
#include "Browser.h"

#include <vgui_controls/Controls.h>
#include <VGUI/ISystem.h>
#include <VGUI/IPanel.h>
#include <VGUI/IVGui.h>
#include <KeyValues.h>


CBrowser g_ServerSingleton;
// expose the browser
EXPOSE_SINGLE_INTERFACE_GLOBALVAR(CBrowser, IVGuiModule, "VGuiModuleBrowser001", g_ServerSingleton);

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CBrowser::CBrowser()
{
}

//-----------------------------------------------------------------------------
// Purpose: Destructor
//-----------------------------------------------------------------------------
CBrowser::~CBrowser()
{
}

//-----------------------------------------------------------------------------
// Purpose: makes the browser dialog frame, by default its hidden
//-----------------------------------------------------------------------------
void CBrowser::CreateDialog()
{
	if (!m_hInternetDlg.Get())
	{
		m_hInternetDlg = new VInternetDlg;
		m_hInternetDlg->Initialize();
	}
}

//-----------------------------------------------------------------------------
// Purpose: links to vgui and engine interfaces
//-----------------------------------------------------------------------------
bool CBrowser::Initialize(CreateInterfaceFn *factorylist, int factoryCount)
{
	// load the vgui interfaces
	return vgui::VGui_InitInterfacesList("Browser", factorylist, factoryCount);
}

//-----------------------------------------------------------------------------
// Purpose: links to other modules interfaces 
//-----------------------------------------------------------------------------
bool CBrowser::PostInitialize(CreateInterfaceFn *modules, int factoryCount)
{
	CreateDialog();
	m_hInternetDlg->SetVisible(false);

	return true;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
bool CBrowser::IsValid()
{
	return true;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
bool CBrowser::Activate()
{
	Open();
	return true;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CBrowser::Deactivate()
{
	if (m_hInternetDlg.Get())
	{
		vgui::ivgui()->PostMessage(m_hInternetDlg->GetVPanel(), new KeyValues("Close"), NULL);
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CBrowser::Reactivate()
{
}

//-----------------------------------------------------------------------------
// Purpose: opens the browser window and sets it visible
//-----------------------------------------------------------------------------
void CBrowser::Open()
{
	m_hInternetDlg->Open();
}

//-----------------------------------------------------------------------------
// Purpose: returns direct handle to main server browser dialog
//-----------------------------------------------------------------------------
//vgui::VPanel *CBrowser::GetPanel()
vgui::VPANEL CBrowser::GetPanel()
{
	return m_hInternetDlg.Get() ? m_hInternetDlg->GetVPanel() : NULL;
}


//-----------------------------------------------------------------------------
// Purpose: Closes down the server browser for good
//-----------------------------------------------------------------------------
void CBrowser::Shutdown()
{
	if (m_hInternetDlg.Get())
	{
		vgui::ivgui()->PostMessage(m_hInternetDlg->GetVPanel(), new KeyValues("Close"), NULL);
		m_hInternetDlg->MarkForDeletion();
	}
}

//-----------------------------------------------------------------------------
// Purpose: sets the parent panel of the main module panel
//-----------------------------------------------------------------------------
void CBrowser::SetParent(vgui::VPANEL parent)
{
	if(m_hInternetDlg.Get())
	{
		m_hInternetDlg->SetParent(parent);
	}
}

