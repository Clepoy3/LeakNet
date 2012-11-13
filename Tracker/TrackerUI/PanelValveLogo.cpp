//========= Copyright © 1996-2001, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================

#include <vgui_controls/BuildGroup.h>
#include <vgui_controls/Controls.h>
#include <VGUI/Cursor.h>
#include <VGUI/IScheme.h>
#include <VGUI/ISystem.h>
#include <VGUI/MouseCode.h>

#include "PanelValveLogo.h"
#include "Tracker.h"

using namespace vgui;

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CPanelValveLogo::CPanelValveLogo() : ImagePanel(NULL, NULL)
{
}

//-----------------------------------------------------------------------------
// Purpose: Destructor
//-----------------------------------------------------------------------------
CPanelValveLogo::~CPanelValveLogo()
{
}

//-----------------------------------------------------------------------------
// Purpose: Activate the valve web page
//-----------------------------------------------------------------------------
void CPanelValveLogo::OnMousePressed(MouseCode code)
{
	// disable the valve logo button if we are in game
	if(Tracker_StandaloneMode())
	{
		system()->ShellExecute("open", "http://www.valvesoftware.com");
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : *inResourceData - 
//-----------------------------------------------------------------------------
void CPanelValveLogo::ApplySchemeSettings(vgui::IScheme *pScheme)
{
	SetImage(scheme()->GetImage("Resource/valve_logo", false));
	SetCursor(dc_hand);

	ImagePanel::ApplySchemeSettings(pScheme);
}

