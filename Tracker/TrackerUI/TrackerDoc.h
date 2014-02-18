//=========== (C) Copyright 2000 Valve, L.L.C. All rights reserved. ===========
//
// The copyright to the contents herein is the property of Valve, L.L.C.
// The contents may be used and/or copied only with the written permission of
// Valve, L.L.C., or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: Base tracker user document containing all the info about a single user
//=============================================================================

#ifndef TRACKERDOC_H
#define TRACKERDOC_H
#pragma once

#include <VGUI/VGUI.h>
#include <KeyValues.h>

#include "UtlRBTree.h"

namespace vgui
{
class Panel;
};

class CBuddy;

//-----------------------------------------------------------------------------
// Purpose: Base tracker user document containing all the info about a single user
//-----------------------------------------------------------------------------
class CTrackerDoc
{
public:
	CTrackerDoc();
	~CTrackerDoc();

	// gets the base data registry
	KeyValues *Data() { return m_pDataRegistry; } // VXP: Crashed at this point when quitting game runned with -showplatform parameted

	// current user
	unsigned int GetUserID();
	const char *GetUserName();

	// buddies
	CBuddy *GetBuddy(unsigned int buddyID);				// returns a buddy from the "User/Buddies/" registry
	void AddNewBuddy(KeyValues *buddyData);		// adds a new buddy to the permenant buddy registry
	void ClearBuddyStatus();					// moves all the buddies offline
	void UpdateBuddyStatus(unsigned int buddyID, int status, unsigned int sessionID, unsigned int serverID, unsigned int *IP, unsigned int *port, unsigned int *gameIP, unsigned int *gamePort, const char *gameType);
	void RemoveBuddy(unsigned int userID);
	int GetNumberOfBuddies();
	int GetBuddyByIndex(int index);		// index is in the range [0, GetNumberOfBuddies).  Used to walk buddy list.

	// messages
	KeyValues *GetFirstMessage(int buddyID);
	void MoveMessageToHistory(int buddyID, KeyValues *message);
	
	// data serialization
	bool SaveAppData();
	bool LoadAppData();
	bool LoadUserData(unsigned int iUID);
	bool SaveUserData();

	// conversion functions for converting messages & files to data registries
	void WriteOut(KeyValues *dat, class ISendMessage *outFile);
	void ReadIn(KeyValues *dat, class IReceiveMessage *inFile);

	// windows
	void SaveDialogState(vgui::Panel *dialog, const char *dialogName, unsigned int buddyID = 0);
	void LoadDialogState(vgui::Panel *dialog, const char *dialogName, unsigned int buddyID = 0);
	void ShutdownAllDialogs();

private:
	KeyValues *m_pDataRegistry;

	// cached registry pointers
	KeyValues *m_pUserData;
	KeyValues *m_pAppData;
	KeyValues *m_pBuddyData;

	// buddies
	struct BuddyMapItem_t
	{
		unsigned int buddyID;
		CBuddy *pBuddy;
	};
	CUtlRBTree<BuddyMapItem_t, short> m_BuddyMap;
	static bool BuddyMapItemLessFunc(const BuddyMapItem_t &r1, const BuddyMapItem_t &r2);
};

// returns a pointer to the current active document
extern CTrackerDoc *GetDoc();

#endif // TRACKERDOC_H
