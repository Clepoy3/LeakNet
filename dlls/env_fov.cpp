//========= Copyright © 1996-2001, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
//=============================================================================

#include "cbase.h"

class CEnvFOV : public CPointEntity
{
public:
	DECLARE_CLASS( CEnvFOV, CPointEntity );

	void	InputFOV( inputdata_t &inputdata );

private:

	float	m_flSpeed;
	int		m_nFOV;

	DECLARE_DATADESC();
};

LINK_ENTITY_TO_CLASS( env_fov, CEnvFOV );

BEGIN_DATADESC( CEnvFOV )

	DEFINE_KEYFIELD( CEnvFOV, m_nFOV, FIELD_INTEGER, "targetfov" ),

	DEFINE_INPUTFUNC( CEnvFOV, FIELD_VOID, "ChangeFOV", InputFOV ),

END_DATADESC()

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : &inputdata - 
//-----------------------------------------------------------------------------
void CEnvFOV::InputFOV( inputdata_t &inputdata )
{
	CBasePlayer *pPlayer = dynamic_cast<CBasePlayer *>(CBaseEntity::Instance( engine->PEntityOfEntIndex( 1 )));

	if ( pPlayer != NULL )
	{
		//Stuff the values
		pPlayer->SetFOV( m_nFOV, 0.0f );
	}
}
