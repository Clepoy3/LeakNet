//========= Copyright © 2017, LeakNet, All rights reserved. ============
//
// Purpose:		Player for Snowball Fight.
//
// $NoKeywords: $
//=============================================================================

#include "cbase.h"
#include "sb_player.h"
#include "sb_gamerules.h"
#include "trains.h"
#include "vcollide_parse.h"
#include "in_buttons.h"
#include "igamemovement.h"
#include "ai_hull.h"
#include "hl2_shareddefs.h"
#include "ndebugoverlay.h"
#include "decals.h"
#include "ammodef.h"
#include "IEffects.h"
#include "sb_client.h"
#include "client.h"
//#include "sb_shareddefs.h"
#include "shake.h"
#include "team.h"


#pragma warning( disable : 4355 )

// -------------------------------------------------------------------------------- //
// Tables.
// -------------------------------------------------------------------------------- //

LINK_ENTITY_TO_CLASS( player, CSBPlayer );
PRECACHE_REGISTER(player);

IMPLEMENT_SERVERCLASS_ST( CSBPlayer, DT_SBPlayer )
END_SEND_TABLE()

// -------------------------------------------------------------------------------- //
// CCSPlayer implementation.
// -------------------------------------------------------------------------------- //
CSBPlayer::CSBPlayer()
{
}


CSBPlayer::~CSBPlayer()
{
}


CSBPlayer *CSBPlayer::CreatePlayer( const char *className, edict_t *ed )
{
	CSBPlayer::s_PlayerEdict = ed;
	return (CSBPlayer*)CreateEntityByName( className );
}


void CSBPlayer::Precache()
{
	BaseClass::Precache();
}