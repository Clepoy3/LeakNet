//========= Copyright © 1996-2001, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================

#ifndef RAWLOGPANEL_H
#define RAWLOGPANEL_H
#ifdef _WIN32
#pragma once
#endif

#include <vgui_controls/Frame.h>
#include <vgui_controls/PHandle.h>
#include <vgui_controls/ListPanel.h>
#include <KeyValues.h>
#include <vgui_controls/PropertyPage.h>

#include "rcon.h"

namespace vgui
{
class Button;
class ToggleButton;
class RadioButton;
class Label;
class TextEntry;
//class KeyValues;
class ListPanel;
//class KeyValues;
class MessageBox;
class ComboBox;
class Panel;
class PropertySheet;
};


//-----------------------------------------------------------------------------
// Purpose: Dialog for displaying information about a game server
//-----------------------------------------------------------------------------
class CRawLogPanel:public vgui::PropertyPage
{
public:
	CRawLogPanel(vgui::Panel *parent, const char *name);
	~CRawLogPanel();

	// property page handlers
	virtual void OnPageShow();
	virtual void OnPageHide();
	void DoInsertString(const char *str);

	void SetRcon(CRcon *rcon);


	virtual void PerformLayout();

private:

	void OnSendRcon();

	vgui::TextEntry *m_pServerRawLogPanel;
	vgui::TextEntry *m_pEnterRconPanel;
	vgui::Button *m_pSendRconButton;

	CRcon *m_pRcon;
		
	DECLARE_PANELMAP();
	typedef vgui::PropertyPage BaseClass;

};

#endif // RAWLOGPANEL_H