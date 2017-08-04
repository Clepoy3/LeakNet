//========= Copyright © 1996-2001, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
//=============================================================================

#include "cbase.h"

class CWeaponCubemap : public CBaseCombatWeapon
{
public:

	DECLARE_CLASS( CWeaponCubemap, CBaseCombatWeapon );

	DECLARE_SERVERCLASS();

	void	Precache( void );

	bool	HasAnyAmmo( void )	{ return true; }
};

IMPLEMENT_SERVERCLASS_ST( CWeaponCubemap, DT_WeaponCubemap )
END_SEND_TABLE()

LINK_ENTITY_TO_CLASS( weapon_cubemap, CWeaponCubemap );
PRECACHE_WEAPON_REGISTER( weapon_cubemap ); // VXP: Fix for precache

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CWeaponCubemap::Precache( void )
{
	BaseClass::Precache();
}
