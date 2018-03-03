//========= Copyright © 1996-2001, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================

#include "DialogServerPassword.h"

#include <vgui_controls/Button.h>
#include <KeyValues.h>
#include <vgui_controls/Label.h>
#include <vgui_controls/TextEntry.h>

#include <vgui_controls/Controls.h>
#include <VGUI/ISurface.h>

using namespace vgui;

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CDialogServerPassword::CDialogServerPassword() : Frame(NULL, "DialogServerPassword")
{
	m_iServerID = -1;
	SetSize(320, 240);

	m_pInfoLabel = new Label(this, "InfoLabel", "This server requires a password to join.");
	m_pGameLabel = new Label(this, "GameLabel", "<game label>");
	m_pPasswordEntry = new TextEntry(this, "PasswordEntry");
	m_pConnectButton = new Button(this, "ConnectButton", "&Connect");
	m_pPasswordEntry->SetTextHidden(true);

	LoadControlSettings("Admin\\DialogServerPassword.res", "PLATFORM");

	SetTitle("Server Requires Password - Servers", true);

	// set our initial position in the middle of the workspace
	MoveToCenterOfScreen();
}

//-----------------------------------------------------------------------------
// Purpose: Destructor
//-----------------------------------------------------------------------------
CDialogServerPassword::~CDialogServerPassword()
{
}

//-----------------------------------------------------------------------------
// Purpose: initializes the dialog and brings it to the foreground
//-----------------------------------------------------------------------------
void CDialogServerPassword::Activate(const char *serverName, unsigned int serverID)
{
	m_pGameLabel->SetText(serverName);
	m_iServerID = serverID;

	m_pConnectButton->SetAsDefaultButton(true);
	MakePopup();
	MoveToFront();
	m_pPasswordEntry->RequestFocus();
	RequestFocus();
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : *command - 
//-----------------------------------------------------------------------------
void CDialogServerPassword::OnCommand(const char *command)
{
	bool bClose = false;

	if (!_stricmp(command, "Connect"))
	{
		KeyValues *msg = new KeyValues("JoinServerWithPassword");
		char buf[64];
		m_pPasswordEntry->GetText(buf, sizeof(buf)-1);
		msg->SetString("password", buf);
		msg->SetInt("serverID", m_iServerID);
		PostActionSignal(msg);

		bClose = true;
	}
	else if (!_stricmp(command, "Close"))
	{
		bClose = true;
	}
	else
	{
		BaseClass::OnCommand(command);
	}

	if (bClose)
	{
		PostMessage(this, new KeyValues("Close"));
		MarkForDeletion();
	}
}


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CDialogServerPassword::PerformLayout()
{
	BaseClass::PerformLayout();
}

//-----------------------------------------------------------------------------
// Purpose: deletes the dialog on close
//-----------------------------------------------------------------------------
void CDialogServerPassword::OnClose()
{
	BaseClass::OnClose();
	MarkForDeletion();
}




