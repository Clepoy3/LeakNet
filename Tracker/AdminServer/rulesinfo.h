//========= Copyright © 1996-2002, Valve LLC, All rights reserved. ============
//
// Purpose: defines a RCon class used to send rcon commands to remote servers
//
// $NoKeywords: $
//=============================================================================

#ifndef RULESINFO_H
#define RULESINFO_H
#ifdef _WIN32
#pragma once
#endif

#include "server.h"
#include "netadr.h"

class CSocket;
class IResponse;

#include <vgui_controls/PropertyPage.h>
#include <vgui_controls/Frame.h>
#include <vgui_controls/ListPanel.h>
#include <KeyValues.h>


class CRulesInfo 
{

public:

	CRulesInfo(IResponse *target,serveritem_t &server);
	~CRulesInfo();

	// send an rcon command to a server
	void Query();

	void Refresh();

	bool IsRefreshing();
	serveritem_t &GetServer();
	void RunFrame();
	bool Refreshed();

	void UpdateServer(netadr_t *adr, CUtlVector<KeyValues *> *Rules);
	
	CUtlVector<KeyValues *> *Rules();

	int serverID;
	int received;

private:

	serveritem_t m_Server;
	CSocket	*m_pQuery;	// Game server query socket
	
	IResponse *m_pResponseTarget;

	bool m_bIsRefreshing;
	float m_fSendTime;
	bool m_bRefreshed;

	CUtlVector<KeyValues *> *m_vRules;

};


#endif // RULESINFO_H