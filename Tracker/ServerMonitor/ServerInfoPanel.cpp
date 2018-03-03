//========= Copyright © 1996-2001, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================

#include "ServerInfoPanel.h"
#include "ServerList.h"

#include <vgui_controls/Controls.h>
#include <VGUI/IScheme.h>
//#include <vgui_controls/TextEntry.h>
#include <vgui_controls/RichText.h>

#include <stdio.h>

using namespace vgui;

// server list
extern CServerList *g_pServerList;

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CServerInfoPanel::CServerInfoPanel(Panel *parent, const char *name) : EditablePanel(parent, name)
{
//	m_pText = new TextEntry(this, "ServerText");
	m_pText = new RichText(this, "ServerText");
//	m_pText->SetMultiline(true);
//	m_pText->SetRichEdit(true);
//	m_pText->SetEditable(false);
	m_pText->SetVerticalScrollbar(true);
	m_iServerID = 0;
}

//-----------------------------------------------------------------------------
// Purpose: Destructor
//-----------------------------------------------------------------------------
CServerInfoPanel::~CServerInfoPanel()
{
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : serverID - 
//-----------------------------------------------------------------------------
void CServerInfoPanel::SetServerID(int serverID)
{
	m_iServerID = serverID;
	InvalidateLayout();
}

//-----------------------------------------------------------------------------
// Purpose: Lays out controls
//-----------------------------------------------------------------------------
void CServerInfoPanel::PerformLayout()
{
	BaseClass::PerformLayout();

	Color col1 = GetSchemeColor("DimBaseText", scheme()->GetIScheme(scheme()->GetDefaultScheme()));
	Color col2 = GetSchemeColor("BaseText", scheme()->GetIScheme(scheme()->GetDefaultScheme()));
	Color col3 = GetSchemeColor("BrightBaseText", scheme()->GetIScheme(scheme()->GetDefaultScheme()));

//	SetBorder(scheme()->GetBorder(scheme()->GetDefaultScheme(), "BaseBorder"));
	SetBorder(scheme()->GetIScheme(scheme()->GetDefaultScheme())->GetBorder("BaseBorder"));

	int wide, tall;
	GetSize(wide, tall);

	m_pText->SetPos(5, 5);
	m_pText->SetSize(wide - 10, tall - 10);

	// reset text
	m_pText->SetText("");

	// rebuild text string from server info
	server_t &server = g_pServerList->GetServer(m_iServerID);

	// server name
	m_pText->InsertColorChange(col1);
	m_pText->InsertString("Server: ");
	m_pText->InsertColorChange(col2);
	m_pText->InsertString(server.name);
	m_pText->InsertString("\n");

	// server status
	m_pText->InsertColorChange(col1);
	m_pText->InsertString("Status: ");

	switch (server.state)
	{
	case SERVER_ACTIVE:
		if (server.underHeavyLoad)
		{
			m_pText->InsertColorChange(Color(0, 255, 0, 0));
			m_pText->InsertString("Under Heavy Load");
		}
		else
		{
			m_pText->InsertColorChange(col2);
			m_pText->InsertString("Active");
		}
		break;

	case SERVER_DOWN:
		m_pText->InsertColorChange(col3);
		m_pText->InsertString("Inactive");
		break;

	case SERVER_NOTRESPONDING:
		m_pText->InsertColorChange(Color(255, 100, 100, 0));
		m_pText->InsertString("NOT RESPONDING");
		break;

	case SERVER_SHUTTINGDOWN:
		m_pText->InsertColorChange(col3);
		m_pText->InsertString("Shutting down");
		break;

	case SERVER_UNKNOWN:
	default:
		m_pText->InsertColorChange(Color(255, 100, 100, 0));
		m_pText->InsertString("UNKNOWN");
		break;
	};

	m_pText->InsertString("\n");

	// only continue drawing if we have an active server
	if (server.state != SERVER_ACTIVE && server.state != SERVER_SHUTTINGDOWN)
		return;

	// primary
	m_pText->InsertColorChange(col1);
	m_pText->InsertString("Primary: ");
	if (server.primary)
	{
		m_pText->InsertColorChange(col3);
		m_pText->InsertString("true");
	}
	else
	{
		m_pText->InsertColorChange(col2);
		m_pText->InsertString("false");
	}
	m_pText->InsertString("\n");

	// user count
	m_pText->InsertColorChange(col1);
	m_pText->InsertString("Users: ");
	m_pText->InsertColorChange(col2);
	char buf[64];
	sprintf(buf, "%d / %d", server.users, server.maxUsers);
	m_pText->InsertString(buf);
	m_pText->InsertString("\n");

	// fps
	m_pText->InsertColorChange(col1);
	m_pText->InsertString("Fps: ");
	m_pText->InsertColorChange(col2);
	if (server.fps > 0)
	{
		sprintf(buf, "%d", server.fps);
	}
	else
	{
		strcpy(buf, "sleeping");
	}
	m_pText->InsertString(buf);
	m_pText->InsertString("\n");


	// network buffers
	m_pText->InsertColorChange(col1);
	m_pText->InsertString("Open network buffers: ");
	m_pText->InsertColorChange(col2);
	sprintf(buf, "%d    ", server.networkBuffers);
	m_pText->InsertString(buf);
	m_pText->InsertColorChange(col1);
	m_pText->InsertString("Peak: ");
	m_pText->InsertColorChange(col2);
	sprintf(buf, "%d", server.peakNetworkBuffers);
	m_pText->InsertString(buf);
	m_pText->InsertString("\n");

	// sqldb queries
	m_pText->InsertColorChange(col1);
	m_pText->InsertString("Open sql queries: ");
	m_pText->InsertColorChange(col2);
	sprintf(buf, "%d    ", server.dbOutBufs);
	m_pText->InsertString(buf);
	m_pText->InsertColorChange(col1);
	m_pText->InsertString("finished but unproccesed: ");
	m_pText->InsertColorChange(col2);
	sprintf(buf, "%d", server.dbInBufs);
	m_pText->InsertString(buf);
	m_pText->InsertString("\n");

	// bandwidth
	m_pText->InsertColorChange(col1);
	m_pText->InsertString("Bandwidth usage:  receiving ");
	m_pText->InsertColorChange(col2);
	sprintf(buf, "%d bytes/second    ", server.bytesReceivedPerSecond);
	m_pText->InsertString(buf);
	m_pText->InsertColorChange(col1);
	m_pText->InsertString("sending ");
	m_pText->InsertColorChange(col2);
	sprintf(buf, "%d bytes/second", server.bytesSentPerSecond);
	m_pText->InsertString(buf);
	m_pText->InsertString("\n");
}
