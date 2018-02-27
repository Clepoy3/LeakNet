//========= Copyright © 1996-2001, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
//=============================================================================

#ifndef GRENADE_HOPWIRE_H
#define GRENADE_HOPWIRE_H
#ifdef _WIN32
#pragma once
#endif

#include "basegrenade_shared.h"
#include "sprite.h"

// VXP
#include "rope.h"
#include "rope_shared.h"
#include "beam_shared.h"

#define MAX_HOOKS 8
#define TETHERHOOK_MODEL	"models/Weapons/w_hopwire.mdl"

class CGrenadeHopWire;

//-----------------------------------------------------------------------------
// Tether hook
//-----------------------------------------------------------------------------

class CTetherHook : public CBaseAnimating
{
	DECLARE_CLASS( CTetherHook, CBaseAnimating );
public:
	typedef CBaseAnimating BaseClass;

	bool	CreateVPhysics( void );
	void	Spawn( void );

	void	SetVelocity( const Vector &velocity, const AngularImpulse &angVelocity );
	void	StartTouch( CBaseEntity *pOther );

	static CTetherHook	*Create( const Vector &origin, const QAngle &angles, CGrenadeHopWire *pOwner );

	void	CreateRope( void );
	void	HookThink( void );

	void	KillHook( void );
	void	Detonate( void );

	DECLARE_DATADESC();

private:
	CHandle<CGrenadeHopWire>	m_hTetheredOwner;
	IPhysicsSpring				*m_pSpring;
	CRopeKeyframe				*m_pRope;
	CSprite						*m_pGlow;
	CBeam						*m_pBeam;
	bool						m_bAttached;
};

class CGrenadeHopWire : public CBaseGrenade
{
public:
	DECLARE_CLASS( CGrenadeHopWire, CBaseGrenade );
	DECLARE_DATADESC();

	void	Spawn( void );
	void	Precache( void );
	bool	CreateVPhysics( void );
	void	SetTimer( float timer );
	void	SetVelocity( const Vector &velocity, const AngularImpulse &angVelocity );
	void	Detonate( void );
	
	void	CombatThink( void );
	void	TetherThink( void );

	void	Event_Killed( const CTakeDamageInfo &info ); // VXP

protected:

	int			m_nHooksShot;
	CSprite		*m_pGlow;

	CUtlVector<CTetherHook *> m_hooksList;
};

extern CBaseGrenade *HopWire_Create( const Vector &position, const QAngle &angles, const Vector &velocity, const AngularImpulse &angVelocity, CBaseEntity *pOwner, float timer );

#endif // GRENADE_HOPWIRE_H
