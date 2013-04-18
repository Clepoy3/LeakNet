//========= Copyright © 1996-2001, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================

#ifndef SERVERCONFIGPANEL_H
#define SERVERCONFIGPANEL_H
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
class KeyValues;
class ListPanel;
class KeyValues;
class MessageBox;
class ComboBox;
class Panel;
class PropertySheet;
};


//-----------------------------------------------------------------------------
// Purpose: Dialog for displaying information about a game server
//-----------------------------------------------------------------------------
class CServerConfigPanel:public vgui::PropertyPage
{
public:
	CServerConfigPanel(vgui::Panel *parent, const char *name,const char *mod);
	~CServerConfigPanel();

	// property page handlers
	virtual void OnPageShow();
	virtual void OnPageHide();

	void SetRcon(CRcon *rcon);

	virtual void PerformLayout();

	// set the rules for the panel to use
	void SetRules(CUtlVector<vgui::KeyValues *> *rules);
	void SetHostName(const char *name);


private:

	void OnSendConfig();

	//vgui::TextEntry *m_pServerServerConfigPanel;
	//vgui::TextEntry *m_pEnterServerConfigPanel;
	vgui::Button *m_pSendConfigButton;

	CRcon *m_pRcon;
	CUtlVector<vgui::KeyValues *> *m_Rules; // the servers rules, so we can parse them
	char m_sHostName[512]; // the hostname of the server, this isn't in a rule...	

	DECLARE_PANELMAP();
	typedef vgui::PropertyPage BaseClass;
};

#endif // SERVERCONFIGPANEL_H