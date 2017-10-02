//========= Copyright © 1996-2003, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
//=============================================================================

#ifndef ENTITYFLAME_H
#define ENTITYFLAME_H
#ifdef _WIN32
#pragma once
#endif

#define FLAME_DAMAGE_INTERVAL			0.2f // How often to deal damage. VXP: Was 0.1f, but it's too fast

class CEntityFlame : public CBaseEntity 
{
public:
	DECLARE_SERVERCLASS();
	DECLARE_CLASS( CEntityFlame, CBaseEntity );

	CEntityFlame( void );

	static CEntityFlame	*Create( CBaseEntity *pTarget, bool bUsePlasma = false );

	void	AttachToEntity( CBaseEntity *pTarget );
	void	SetLifetime( float lifetime );

	void	SetSize( float size ) { m_flSize = size; }

	DECLARE_DATADESC();

protected:

	void	DoIgnite( inputdata_t &inputdata, bool bUsePlasma );
	void	InputIgnite( inputdata_t &inputdata );
	void	InputIgnitePlasma( inputdata_t &inputdata );

	void	FlameThink( void );

	CNetworkHandle( CBaseEntity, m_hEntAttached );		// The entity that we are burning (attached to).

	CNetworkVar( float, m_flSize );
	float	m_flLifetime;
	CNetworkVar( bool, m_bUsePlasma );
	float	m_flDamageTime;
};

#endif // ENTITYFLAME_H
