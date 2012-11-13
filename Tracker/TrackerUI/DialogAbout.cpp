//========= Copyright © 1996-2001, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================

#include "DialogAbout.h"
#include "TrackerDoc.h"

#include <vgui_controls/Button.h>
#include <vgui_controls/Controls.h>
#include <VGUI/ISystem.h>
#include <vgui_controls/TextEntry.h>

#include <stdio.h>

using namespace vgui;

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CDialogAbout::CDialogAbout() : Frame(NULL, "DialogAbout")
{
	m_pClose = new Button(this, "CloseButton", "#TrackerUI_Close");
	m_pText = new TextEntry(this, "AboutText");

//	m_pText->SetRichEdit(true);
	m_pText->SetMultiline(true);
	m_pText->SetVerticalScrollbar(true);

	SetTitle("#TrackerUI_FriendsAboutTitle", true);

	LoadControlSettings("Friends/DialogAbout.res");

	GetDoc()->LoadDialogState(this, "About");
}

//-----------------------------------------------------------------------------
// Purpose: Destructor
//-----------------------------------------------------------------------------
CDialogAbout::~CDialogAbout()
{
	if (GetDoc())
	{
		GetDoc()->SaveDialogState(this, "About");
	}
}

//-----------------------------------------------------------------------------
// Purpose: activates the dialog, brings it to the front
//-----------------------------------------------------------------------------
void CDialogAbout::Run()
{
	// bring us to the foreground
	m_pClose->RequestFocus();
	RequestFocus();
}

//-----------------------------------------------------------------------------
// Purpose: sets up the about text for drawing
//-----------------------------------------------------------------------------
void CDialogAbout::PerformLayout()
{
	BaseClass::PerformLayout();

	m_pText->SetText("");

	m_pText->InsertString("Friends - Copyright © 2001 Valve LLC\n\nInstalled version: ");

	char versionString[32];
	if (!system()->GetRegistryString("HKEY_CURRENT_USER\\Software\\Valve\\Tracker\\Version", versionString, sizeof(versionString)))
	{
		strcpy(versionString, "<unknown>");
	}

	m_pText->InsertString(versionString);

	m_pText->InsertString("\nBuild number: ");

	char buildString[32];
	extern int build_number( void );
	sprintf(buildString, "%d", build_number());

	m_pText->InsertString(buildString);
	m_pText->InsertString("\n\n");

	// beta
	m_pText->InsertString("This software is BETA and is not for public release.\n");
	m_pText->InsertString("Please report any bugs or issues to:\n  trackerbugs@valvesoftware.com\n");
}

//-----------------------------------------------------------------------------
// Purpose: dialog deletes itself on close
//-----------------------------------------------------------------------------
void CDialogAbout::OnClose()
{
	BaseClass::OnClose();
	MarkForDeletion();
}
