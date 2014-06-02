//========= Copyright © 1996-2002, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================

#include "cbase.h"
#include "convar.h"
#include "tier0/dbg.h"
#include "player.h"
#include "world.h"


void Test_CreateEntity()
{
	if ( engine->Cmd_Argc() < 2 )
	{
	//	Error( "Test_CreateEntity: requires entity classname argument." );
		Warning( "Test_CreateEntity: requires entity classname argument.\n" );
		return;
	}

	const char *pClassName = engine->Cmd_Argv( 1 );

	if ( !CreateEntityByName( pClassName ) )
	{
	//	Error( "Test_CreateEntity( %s ) failed.", pClassName );
		Warning( "Test_CreateEntity( %s ) failed.\n", pClassName );
		return;
	}
}


void Test_RandomPlayerPosition()
{
	CBaseEntity *pEnt = CBaseEntity::Instance( engine->PEntityOfEntIndex( 1 ) );
	CWorld *pWorld = GetWorldEntity();
	if ( !pEnt || !dynamic_cast< CBasePlayer* >( pEnt ) )
	{
	//	Error( "Test_RandomPlayerPosition: no local player entity." );
		Warning( "Test_RandomPlayerPosition: no local player entity.\n" );
		return;
	}
	else if ( !pWorld )
	{
	//	Error( "Test_RandomPlayerPosition: no world entity." );
		Warning( "Test_RandomPlayerPosition: no world entity.\n" );
		return;
	}

	CBasePlayer *pPlayer = dynamic_cast< CBasePlayer* >( pEnt );
	
	Vector vMin, vMax;
	pWorld->GetWorldBounds( vMin, vMax );

	Vector vecOrigin;
	vecOrigin.x = RandomFloat( vMin.x, vMax.x );
	vecOrigin.y = RandomFloat( vMin.y, vMax.y );
	vecOrigin.z = RandomFloat( vMin.z, vMax.z );
	pPlayer->ForceOrigin( vecOrigin );
}


ConCommand cc_Test_CreateEntity( "Test_CreateEntity", Test_CreateEntity, 0, FCVAR_CHEAT );
ConCommand cc_Test_RandomPlayerPosition( "Test_RandomPlayerPosition", Test_RandomPlayerPosition, 0, FCVAR_CHEAT );


