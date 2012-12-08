//======== (C) Copyright 1999, 2000 Valve, L.L.C. All rights reserved. ========
//
// The copyright to the contents herein is the property of Valve, L.L.C.
// The contents may be used and/or copied only with the written permission of
// Valve, L.L.C., or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: 
//
// $Workfile:     $
// $NoKeywords: $
//=============================================================================

#if !defined( C_BASEHLPLAYER_H )
#define C_BASEHLPLAYER_H
#ifdef _WIN32
#pragma once
#endif


#include "c_baseplayer.h"
#include "c_hl2_playerlocaldata.h"


class C_BaseHLPlayer : public C_BasePlayer
{
public:
	DECLARE_CLASS( C_BaseHLPlayer, C_BasePlayer );
	DECLARE_CLIENTCLASS();

						C_BaseHLPlayer();

	virtual void		OnDataChanged( DataUpdateType_t updateType );

	void				Weapon_DropPrimary( void );
	void				UpdateFOV( void );
	
	void				Zoom( float FOVOffset, float time );
	float				GetZoom( void );

	bool				IsSprinting( void ) { return false; }

	virtual int			DrawModel( int flags );
	
	// ID Target
//	void				SetIDEnt( C_BaseEntity *pEntity );
	int					GetIDTarget( void ) const;
	void				UpdateIDTarget( void );
public:

	// Return this client's C_BaseHLPlayer pointer
	static C_BaseHLPlayer* GetLocalPlayer( void )
	{
		return ( static_cast< C_BaseHLPlayer * >( C_BasePlayer::GetLocalPlayer() ) );
	}
	
	C_HL2PlayerLocalData		m_HL2Local;


private:
	C_BaseHLPlayer( const C_BaseHLPlayer & ); // not defined, not accessible

	float				m_flZoomStart;
	float				m_flZoomEnd;
	float				m_flZoomRate;
	float				m_flZoomStartTime;
	
	// ID Target
	int					m_iIDEntIndex;
};


#endif
