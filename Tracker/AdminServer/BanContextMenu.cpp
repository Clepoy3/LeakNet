//========= Copyright © 1996-2001, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================

#include "BanContextMenu.h"

#include <vgui_controls/Controls.h>
#include <VGUI/IInput.h>
#include <VGUI/IPanel.h>
#include <VGUI/ISurface.h>
#include <KeyValues.h>

using namespace vgui;

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CBanContextMenu::CBanContextMenu(Panel *parent) : Menu(parent, "BanContextMenu")
{
	CBanContextMenu::parent=parent;
}

//-----------------------------------------------------------------------------
// Purpose: Destructor
//-----------------------------------------------------------------------------
CBanContextMenu::~CBanContextMenu()
{
}

//-----------------------------------------------------------------------------
// Purpose: Activates the menu
//-----------------------------------------------------------------------------
void CBanContextMenu::ShowMenu(Panel *target, unsigned int banID)
{
//	ClearMenu();
	DeleteAllItems();

	if(banID==-1) 
	{
		AddMenuItem("ban", "&Add Ban", new KeyValues("addban", "banID", banID), CBanContextMenu::parent);
	} 
	else
	{
		AddMenuItem("ban", "&Remove Ban", new KeyValues("removeban", "banID", banID), CBanContextMenu::parent);
		AddMenuItem("ban", "&Change Time", new KeyValues("changeban", "banID", banID), CBanContextMenu::parent);
	}

	MakePopup();
	int x, y, gx, gy;
	input()->GetCursorPos(x, y);
	ipanel()->GetPos(surface()->GetEmbeddedPanel(), gx, gy);
	SetPos(x - gx, y - gy);
	MoveToFront();
	RequestFocus();
	SetVisible(true);
}
