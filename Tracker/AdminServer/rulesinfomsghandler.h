//========= Copyright © 1996-2001, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================

#ifndef RULESINFOMSGHANDLERDETAILS_H
#define RULESINFOMSGHANDLERDETAILS_H
#ifdef _WIN32
#pragma once
#endif

#include "Socket.h"
#include "UtlVector.h"

class CRulesInfo;

#include <vgui_controls/PropertyPage.h>
#include <vgui_controls/Frame.h>
#include <vgui_controls/ListPanel.h>
#include <KeyValues.h>


//-----------------------------------------------------------------------------
// Purpose: Socket handler for pinging internet servers
//-----------------------------------------------------------------------------
class CRulesInfoMsgHandlerDetails : public CMsgHandler
{
public:
	CRulesInfoMsgHandlerDetails(CRulesInfo *baseobject, HANDLERTYPE type, void *typeinfo = NULL);

	virtual bool Process(netadr_t *from, CMsgBuffer *msg);

private:
	// the parent class that we push info back to
	CRulesInfo *m_pRulesInfo;
	CUtlVector<KeyValues *> *m_vRules;
};




#endif // RULESINFOMSGHANDLERDETAILS_H
