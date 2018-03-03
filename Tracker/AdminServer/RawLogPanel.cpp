//========= Copyright © 1996-2001, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================

#include <stdio.h>

#include "RawLogPanel.h"


#include <vgui_controls/Controls.h>
#include <VGUI/ISystem.h>
#include <VGUI/ISurface.h>
#include <VGUI/IVGui.h>
#include <KeyValues.h>
#include <vgui_controls/Label.h>
#include <vgui_controls/TextEntry.h>
#include <vgui_controls/Button.h>
#include <vgui_controls/ToggleButton.h>
#include <vgui_controls/RadioButton.h>
#include <vgui_controls/ListPanel.h>
#include <vgui_controls/ComboBox.h>
#include <vgui_controls/PHandle.h>
#include <vgui_controls/PropertySheet.h>

using namespace vgui;
//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CRawLogPanel::CRawLogPanel(vgui::Panel *parent, const char *name) : PropertyPage(parent, name)
{
	SetSize(200,100);
	m_pServerRawLogPanel = new TextEntry(this, "ServerChatText");

	m_pServerRawLogPanel->SetMultiline(true);
	m_pServerRawLogPanel->SetEnabled(true);
	m_pServerRawLogPanel->SetEditable(false);
	m_pServerRawLogPanel->SetVerticalScrollbar(true);
//	m_pServerRawLogPanel->SetRichEdit(false);
	m_pServerRawLogPanel->SetMaximumCharCount(8000); // 100 x 80 char wide lines...	
	m_pServerRawLogPanel->SetWrap(true);


	m_pEnterRconPanel = new TextEntry(this,"RconMessage");
//	m_pEnterRconPanel->SendNewLine(true);

	m_pSendRconButton = new Button(this, "SendRcon", "&Send");
	m_pSendRconButton->SetCommand(new KeyValues("SendRcon"));
	m_pSendRconButton->SetAsDefaultButton(true);
	
	m_pServerRawLogPanel->SetBounds(5,5,GetWide()-5,GetTall()-35);

	m_pEnterRconPanel->SetBounds(5,GetTall()-25,GetWide()-80,20);
	m_pSendRconButton->SetBounds(GetWide()-70,GetTall()-25,60,20);

}

//-----------------------------------------------------------------------------
// Purpose: Destructor
//-----------------------------------------------------------------------------
CRawLogPanel::~CRawLogPanel()
{

}

//-----------------------------------------------------------------------------
// Purpose: Activates the page
//-----------------------------------------------------------------------------
void CRawLogPanel::OnPageShow()
{
	m_pEnterRconPanel->RequestFocus();

}

//-----------------------------------------------------------------------------
// Purpose: Hides the page
//-----------------------------------------------------------------------------
void CRawLogPanel::OnPageHide()
{
}



//-----------------------------------------------------------------------------
// Purpose: Relayouts the data
//-----------------------------------------------------------------------------
void CRawLogPanel::PerformLayout()
{
	BaseClass::PerformLayout();

	// setup the layout of the panels
	m_pServerRawLogPanel->SetBounds(5,5,GetWide()-10,GetTall()-35);

	m_pEnterRconPanel->SetBounds(5,GetTall()-25,GetWide()-80,20);
	m_pSendRconButton->SetBounds(GetWide()-70,GetTall()-25,60,20);
}

//-----------------------------------------------------------------------------
// Purpose: inserts a new string into the main chat panel
//-----------------------------------------------------------------------------
void CRawLogPanel::DoInsertString(const char *str) 
{
	m_pServerRawLogPanel->InsertString(str);
}

//-----------------------------------------------------------------------------
// Purpose: passes the rcon class to use
//-----------------------------------------------------------------------------
void CRawLogPanel::SetRcon(CRcon *rcon) 
{
	m_pRcon=rcon;
}

//-----------------------------------------------------------------------------
// Purpose: run when the send button is pressed, send a rcon "say" to the server
//-----------------------------------------------------------------------------
void CRawLogPanel::OnSendRcon()
{
	if(m_pRcon)
	{
		char chat_text[512];

		m_pEnterRconPanel->GetText(chat_text,512);

		if(strlen(chat_text)>1) // check there is something in the text panel
		{
			m_pRcon->SendRcon(chat_text);

			// the message is sent, zero the text
			m_pEnterRconPanel->SetText("");
		}
	}
}


//-----------------------------------------------------------------------------
// Purpose: Message map
//-----------------------------------------------------------------------------
MessageMapItem_t CRawLogPanel::m_MessageMap[] =
{
	MAP_MESSAGE( CRawLogPanel, "SendRcon", OnSendRcon ),
	MAP_MESSAGE( CRawLogPanel, "TextNewLine", OnSendRcon ),
	MAP_MESSAGE( CRawLogPanel, "PageShow", OnPageShow ),

};

IMPLEMENT_PANELMAP( CRawLogPanel, Frame );