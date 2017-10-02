//========= Copyright © 1996-2001, Valve LLC, All rights reserved. ============
//
// Purpose:
//
// $NoKeywords: $
//=============================================================================

#ifndef	WEAPONIMMOLATOR_H
#define	WEAPONIMMOLATOR_H

#include "cbase.h"
#include "basehlcombatweapon.h"

enum IMMOLATOR_FIRESTATE
{
	FIRE_OFF,
	FIRE_STARTUP,
	FIRE_CHARGE
};

class CWeaponImmolator : public CBaseHLCombatWeapon
{
public:
	DECLARE_CLASS( CWeaponImmolator, CBaseHLCombatWeapon );

	DECLARE_SERVERCLASS();

	CWeaponImmolator( void );
	
	void Precache( void );
	void PrimaryAttack( void );
	void ItemPostFrame( void );

	int CapabilitiesGet( void ) {	return bits_CAP_WEAPON_RANGE_ATTACK1;	}

	void ImmolationDamage( const CTakeDamageInfo &info, const Vector &vecSrcIn, float flRadius, int iClassIgnore );
	virtual bool WeaponLOSCondition( const Vector &ownerPos, const Vector &targetPos, bool bSetConditions );	
	virtual int	WeaponRangeAttack1Condition( float flDot, float flDist );

	void Update();
	void UpdateThink();

	void StartImmolating();
	void StopImmolating();
//	bool IsImmolating() { return m_flBurnRadius != 0.0; } // VXP: Old style
	bool IsImmolating() { return m_flBurnRadius != 0.0; }

	bool Deploy( void );
	bool Holster( CBaseCombatWeapon *pSwitchingTo = NULL );
	void WeaponIdle( void );
//	bool HasAmmo( void );
	void UseAmmo( int count );

	float GetFireRate( void ) { return 1.0f; }

	DECLARE_ACTTABLE();
	DECLARE_DATADESC();

	int	m_beamIndex;

	float m_flBurnRadius;
	float m_flTimeLastUpdatedRadius;

	Vector  m_vecImmolatorTarget;

	IMMOLATOR_FIRESTATE		m_fireState;
	float				m_flAmmoUseTime;	// since we use < 1 point of ammo per update, we subtract ammo on a timer.
	float				m_flStartFireTime;
};

#endif	//WEAPONIMMOLATOR_H
