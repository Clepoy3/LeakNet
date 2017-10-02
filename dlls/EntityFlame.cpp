//========= Copyright © 1996-2003, Valve LLC, All rights reserved. ============
//
// Purpose: Flame entity to be attached to target entity. Serves two purposes:
//
//			1) An entity that can be placed by a level designer and triggered
//			   to ignite a target entity.
//
//			2) An entity that can be created at runtime to ignite a target entity.
//
//=============================================================================

#include "cbase.h"
#include "EntityFlame.h"
#include "fire.h"


BEGIN_DATADESC( CEntityFlame )

	DEFINE_KEYFIELD( CEntityFlame, m_flLifetime, FIELD_FLOAT, "lifetime" ),

	DEFINE_FIELD( CEntityFlame, m_flSize, FIELD_FLOAT ),
	DEFINE_FIELD( CEntityFlame, m_hEntAttached, FIELD_EHANDLE ),

	DEFINE_FIELD( CEntityFlame, m_bUsePlasma, FIELD_BOOLEAN ),
	DEFINE_FIELD( CEntityFlame, m_flDamageTime, FIELD_FLOAT ),
	
	DEFINE_FUNCTION( CEntityFlame, FlameThink ),

	DEFINE_INPUTFUNC( CEntityFlame, FIELD_VOID, "Ignite", InputIgnite ),
	DEFINE_INPUTFUNC( CEntityFlame, FIELD_VOID, "IgnitePlasma", InputIgnitePlasma ),

END_DATADESC()


IMPLEMENT_SERVERCLASS_ST( CEntityFlame, DT_EntityFlame )
	SendPropFloat( SENDINFO( m_flSize ), 16, SPROP_NOSCALE ),
	SendPropEHandle( SENDINFO( m_hEntAttached ) ),
	SendPropBool( SENDINFO( m_bUsePlasma ) ),
END_SEND_TABLE()

LINK_ENTITY_TO_CLASS( entityflame, CEntityFlame );
LINK_ENTITY_TO_CLASS( env_entity_igniter, CEntityFlame );


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
CEntityFlame::CEntityFlame( void )
{
	m_flSize		= 0.0f;
	m_flLifetime	= 0.0f;

	m_bUsePlasma	= false;
	m_flDamageTime	= gpGlobals->curtime;
}


//-----------------------------------------------------------------------------
// Purpose: 
// Input  : inputdata - 
//-----------------------------------------------------------------------------
void CEntityFlame::DoIgnite( inputdata_t &inputdata, bool bUsePlasma )
{
	if (m_target != NULL_STRING)
	{
		CBaseEntity *pTarget = NULL;
		while ((pTarget = gEntList.FindEntityGeneric(pTarget, STRING(m_target), this, inputdata.pActivator)) != NULL)
		{
			// Combat characters know how to catch themselves on fire.
			CBaseCombatCharacter *pBCC = pTarget->MyCombatCharacterPointer();
			if (pBCC)
			{
				// DVS TODO: consider promoting Ignite to CBaseEntity and doing everything here
				pBCC->Ignite(m_flLifetime, true, 0.0f, false, true);
			}
			// Everything else, we handle here.
			else
			{
				CEntityFlame *pFlame = CEntityFlame::Create(pTarget, bUsePlasma);
				if (pFlame)
				{
					pFlame->SetLifetime(m_flLifetime);
					pTarget->AddFlag( FL_ONFIRE );
				}
			}
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : inputdata - 
//-----------------------------------------------------------------------------
void CEntityFlame::InputIgnite( inputdata_t &inputdata )
{
	DoIgnite( inputdata, false );
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : inputdata - 
//-----------------------------------------------------------------------------
void CEntityFlame::InputIgnitePlasma( inputdata_t &inputdata )
{
	DoIgnite( inputdata, true );
}


//-----------------------------------------------------------------------------
// Purpose: Creates a flame and attaches it to a target entity.
// Input  : pTarget - 
//-----------------------------------------------------------------------------
CEntityFlame *CEntityFlame::Create( CBaseEntity *pTarget, bool bUsePlasma )
{
	CEntityFlame *pFlame = (CEntityFlame *) CreateEntityByName( "entityflame" );

	if ( pFlame == NULL )
		return NULL;

	/*IPhysicsObject *pPhysicsObject = pTarget->VPhysicsGetObject();

	if ( pPhysicsObject )
	{
		pPhysicsObject->
	}*/

	float size = ( pTarget->WorldAlignMaxs().y - pTarget->WorldAlignMins().y );

	if ( size < 8 )
		size = 8;

	UTIL_SetOrigin( pFlame, pTarget->GetAbsOrigin() );

	pFlame->m_flSize = size;
	pFlame->SetThink( &CEntityFlame::FlameThink );
	pFlame->SetNextThink( gpGlobals->curtime + 0.1f );

	pFlame->AttachToEntity( pTarget );
	pFlame->SetLifetime( 2.0f );
	pFlame->m_bUsePlasma = bUsePlasma;

	//Send to the client even though we don't have a model
	pFlame->AddEFlags( EFL_FORCE_CHECK_TRANSMIT );

	return pFlame;
}


//-----------------------------------------------------------------------------
// Purpose: Attaches the flame to an entity and moves with it
// Input  : pTarget - target entity to attach to
//-----------------------------------------------------------------------------
void CEntityFlame::AttachToEntity( CBaseEntity *pTarget )
{
	// For networking to the client.
	m_hEntAttached = pTarget;

	// So our heat emitter follows the entity around on the server.
	SetParent( pTarget );
	Relink();
}


//-----------------------------------------------------------------------------
// Purpose: 
// Input  : lifetime - 
//-----------------------------------------------------------------------------
void CEntityFlame::SetLifetime( float lifetime )
{
	m_flLifetime = gpGlobals->curtime + lifetime;
}


//-----------------------------------------------------------------------------
// Purpose: Burn targets around us
//-----------------------------------------------------------------------------
void CEntityFlame::FlameThink( void )
{
	// VXP: See if we're done burning, or our attached ent has vanished
	if ( m_flLifetime < gpGlobals->curtime || m_hEntAttached == NULL )
	{
		SetThink( &CEntityFlame::SUB_Remove );
		SetNextThink( gpGlobals->curtime + 0.1f );

		// Notify anything we're attached to
		if ( m_hEntAttached )
		{
			CBaseCombatCharacter *pAttachedCC = m_hEntAttached->MyCombatCharacterPointer();

			if( pAttachedCC )
			{
				// Notify the NPC that it's no longer burning!
				pAttachedCC->Extinguish();
			}
		}

		return;
	}

	int bitsDamageType = m_bUsePlasma ? DMG_PLASMA : DMG_BURN;
#if 1
	if ( m_hEntAttached && gpGlobals->curtime>= m_flDamageTime )
	{
		// Do radius damage ignoring the entity I'm attached to. This will harm things around me.
		RadiusDamage( CTakeDamageInfo( this, this, 4.0f, bitsDamageType ), GetAbsOrigin(), m_flSize/2, CLASS_NONE, m_hEntAttached );

		// Directly harm the entity I'm attached to. This is so we can precisely control how much damage the entity
		// that is on fire takes without worrying about the flame's position relative to the bodytarget (which is the
		// distance that the radius damage code uses to determine how much damage to inflict)
		
		// FLAME_DIRECT_DAMAGE = FLAME_DIRECT_DAMAGE_PER_SEC * FLAME_DAMAGE_INTERVAL
		// 5.0f * 0.2f = 1.0f

		m_hEntAttached->TakeDamage( CTakeDamageInfo( this, this, 1.0f, bitsDamageType /*| DMG_DIRECT*/ ) );

		m_flDamageTime = gpGlobals->curtime + 0.7f; // VXP: Fix for repeated citizen pain sounds
	}
	else
	{
		// VXP: FLAME_RADIUS_DAMAGE = FLAME_RADIUS_DAMAGE_PER_SEC * FLAME_DAMAGE_INTERVAL
		// 4.0f * 0.2f = 0.8f
		RadiusDamage( CTakeDamageInfo( this, this, 0.8f, bitsDamageType ), GetAbsOrigin(), m_flSize/2, CLASS_NONE, NULL );
	}
#else
	RadiusDamage( CTakeDamageInfo( this, this, 4.0f, bitsDamageType ), GetAbsOrigin(), m_flSize/2, CLASS_NONE, NULL );
#endif // 0
	FireSystem_AddHeatInRadius( GetAbsOrigin(), m_flSize/2, 2.0f );

	SetNextThink( gpGlobals->curtime + FLAME_DAMAGE_INTERVAL );
}


//-----------------------------------------------------------------------------
// Purpose: 
// Input  : pEnt -	
//-----------------------------------------------------------------------------
void CreateEntityFlame(CBaseEntity *pEnt)
{
	CEntityFlame::Create( pEnt );
}
