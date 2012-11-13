//========= Copyright © 1996-2003, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================

#ifndef ITRACKERUSER_H
#define ITRACKERUSER_H
#ifdef _WIN32
#pragma once
#endif

#include "interface.h"

//-----------------------------------------------------------------------------
// Purpose: Interface to accessing information about Tracker Users
//-----------------------------------------------------------------------------
class ITrackerUser : public IBaseInterface
{
public:
	// returns true if the interface is ready for use
	virtual bool IsValid() = 0;

	// returns the tracker userID of the current user
	virtual int GetTrackerID() = 0;

	// returns information about a user
	virtual const char *GetUserName(int userID) = 0;
	virtual const char *GetFirstName(int userID) = 0;
	virtual const char *GetLastName(int userID) = 0;
	virtual const char *GetEmail(int userID) = 0;

	// returns true if friendID is a friend of the current user 
	// ie. the current is authorized to see when the friend is online
	virtual bool IsFriend(int friendID) = 0;

	// requests authorization from a user
	virtual void RequestAuthorizationFromUser(int potentialFriendID) = 0;

	// returns the status of the user, > 0 is online, 4 is ingame
	virtual int GetUserStatus(int friendID) = 0;

	// gets the IP address of the server the user is on, returns false if couldn't get
	virtual bool GetUserGameAddress(int friendID, int *ip, int *port) = 0;

	// returns the number of friends, so we can iterate
	virtual int GetNumberOfFriends() = 0;

	// returns the userID of a friend - friendIndex is valid in the range [0, GetNumberOfFriends)
	virtual int GetFriendTrackerID(int friendIndex) = 0;

	// sets whether or not the user can receive messages at this time
	// messages will be queued until this is set to true
	virtual void SetCanReceiveMessages(bool state) = 0;
};

#define TRACKERUSER_INTERFACE_VERSION "TrackerUser001"


#endif // IFRIENDSUSER_H
