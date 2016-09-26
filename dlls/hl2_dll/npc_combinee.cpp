//=========== (C) Copyright 2000 Valve, L.L.C. All rights reserved. ===========
//
// The copyright to the contents herein is the property of Valve, L.L.C.
// The contents may be used and/or copied only with the written permission of
// Valve, L.L.C., or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: This is the camo version of the combine
//			
//
// $Workfile:     $
// $Date:         $
//
//-----------------------------------------------------------------------------
// $Log: $
//
// $NoKeywords: $
//=============================================================================

#include "cbase.h"
#include "ai_hull.h"
#include "npc_talker.h"
#include "npc_CombineE.h"

ConVar	sk_combine_elite_health( "sk_combine_elite_health","0");
ConVar	sk_combine_elite_kick( "sk_combine_elite_kick","0");

LINK_ENTITY_TO_CLASS( npc_combineelite, CNPC_CombineE );

//-----------------------------------------------------------------------------
// Purpose: 
//
//
//-----------------------------------------------------------------------------
void CNPC_CombineE::Spawn( void )
{
	Precache();
	SetModel( "models/Combine_Elite.mdl" );
	m_iHealth = sk_combine_elite_health.GetFloat();
	m_nKickDamage = sk_combine_elite_kick.GetFloat();

	BaseClass::Spawn();
}


//-----------------------------------------------------------------------------
// Purpose:
// Input  :
// Output :
//-----------------------------------------------------------------------------
void CNPC_CombineE::Precache()
{
	engine->PrecacheModel("models/combine_elite.mdl");

	BaseClass::Precache();
}

//-----------------------------------------------------------------------------
// Purpose: Combine needs to check ammo
// Input  :
// Output :
//-----------------------------------------------------------------------------
/*void CNPC_CombineE::CheckAmmo ( void ) // VXP
{
	if (GetActiveWeapon())
	{
		if (GetActiveWeapon()->UsesPrimaryAmmo())
		{
			GetActiveWeapon()->m_iClip1 = GetActiveWeapon()->GetMaxClip1(); 
			GetActiveWeapon()->m_iClip2 = GetActiveWeapon()->GetMaxClip2();  

			if (!GetActiveWeapon()->HasPrimaryAmmo() )
			{
				SetCondition(COND_NO_PRIMARY_AMMO);
			}
			else if (GetActiveWeapon()->UsesClipsForAmmo1() && GetActiveWeapon()->Clip1() < (GetActiveWeapon()->GetMaxClip1() / 4 + 1))
			{
				// don't check for low ammo if you're near the max range of the weapon

				SetCondition(COND_LOW_PRIMARY_AMMO);
			}
		}
		if (!GetActiveWeapon()->HasSecondaryAmmo() )
		{
			if ( GetActiveWeapon()->UsesClipsForAmmo2() )
				SetCondition(COND_NO_SECONDARY_AMMO);
		}
	}
}*/
