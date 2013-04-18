//========= Copyright © 1996-2001, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================

#include "RulesContextMenu.h"

#include <vgui_controls/Controls.h>
#include <VGUI/IInput.h>
#include <VGUI/IPanel.h>
#include <VGUI/ISurface.h>
#include <KeyValues.h>

using namespace vgui;

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CRulesContextMenu::CRulesContextMenu(Panel *parent) : Menu(parent, "RulesContextMenu")
{
	CRulesContextMenu::parent=parent;
}

//-----------------------------------------------------------------------------
// Purpose: Destructor
//-----------------------------------------------------------------------------
CRulesContextMenu::~CRulesContextMenu()
{
}

//-----------------------------------------------------------------------------
// Purpose: Activates the menu
//-----------------------------------------------------------------------------
void CRulesContextMenu::ShowMenu(Panel *target, unsigned int cvarID)
{
	DeleteAllItems();
		
	AddMenuItem("cvar", "&Change Value", new KeyValues("cvar", "cvarID", cvarID), CRulesContextMenu::parent);


	MakePopup();
	int x, y, gx, gy;
	input()->GetCursorPos(x, y);
	ipanel()->GetPos(surface()->GetEmbeddedPanel(), gx, gy);
	SetPos(x - gx, y - gy);
	MoveToFront();
	RequestFocus();
	SetVisible(true);
}
