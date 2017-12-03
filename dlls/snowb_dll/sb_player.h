//========= Copyright © 2017, LeakNet, All rights reserved. ============
//
// Purpose:		Player for Snowball.
//
// $NoKeywords: $
//=============================================================================

#ifndef SB_PLAYER_H
#define SB_PLAYER_H
#pragma once

#include "player.h"


//=============================================================================
// >> Snowball player
//=============================================================================
class CSBPlayer : public CBasePlayer
{
	DECLARE_CLASS( CSBPlayer, CBasePlayer );
	DECLARE_SERVERCLASS();
public:

	CSBPlayer();
	~CSBPlayer();

	static CSBPlayer *CreatePlayer( const char *className, edict_t *ed );
	static CSBPlayer* Instance( int iEnt );

	virtual void		Precache();
};

#endif	//SB_PLAYER_H