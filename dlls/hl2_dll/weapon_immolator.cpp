//========= Copyright © 1996-2001, Valve LLC, All rights reserved. ============
//
// Purpose:
//
// $NoKeywords: $
//=============================================================================

#include "cbase.h"
#include "fire.h"
#include "basehlcombatweapon.h"
#include "npcevent.h"
#include "basecombatcharacter.h"
#include "player.h"
#include "soundent.h"
#include "te_particlesystem.h"
#include "ndebugoverlay.h"
#include "in_buttons.h"
#include "ai_basenpc.h"
#include "ai_memory.h"
#include "beam_shared.h"
#include "EntityFlame.h"

#include "weapon_immolator.h"


#define MAX_BURN_RADIUS		256
#define RADIUS_GROW_RATE	50.0	// units/sec 

#define IMMOLATOR_TARGET_INVALID Vector( FLT_MAX, FLT_MAX, FLT_MAX )

IMPLEMENT_SERVERCLASS_ST(CWeaponImmolator, DT_WeaponImmolator)
END_SEND_TABLE()

LINK_ENTITY_TO_CLASS( info_target_immolator, CPointEntity );
LINK_ENTITY_TO_CLASS( weapon_immolator, CWeaponImmolator );
PRECACHE_WEAPON_REGISTER( weapon_immolator );

BEGIN_DATADESC( CWeaponImmolator )

	DEFINE_FIELD( CWeaponImmolator, m_beamIndex, FIELD_INTEGER ),
	DEFINE_FIELD( CWeaponImmolator, m_flBurnRadius, FIELD_FLOAT ),
	DEFINE_FIELD( CWeaponImmolator, m_flTimeLastUpdatedRadius, FIELD_TIME ),
	DEFINE_FIELD( CWeaponImmolator, m_vecImmolatorTarget, FIELD_VECTOR ),

	DEFINE_FIELD( CWeaponImmolator, m_fireState, FIELD_INTEGER ),
	DEFINE_FIELD( CWeaponImmolator, m_flAmmoUseTime, FIELD_TIME ),
	DEFINE_FIELD( CWeaponImmolator, m_flStartFireTime, FIELD_TIME ),

	DEFINE_ENTITYFUNC( CWeaponImmolator, UpdateThink ),
END_DATADESC()


//-----------------------------------------------------------------------------
// Maps base activities to weapons-specific ones so our characters do the right things.
//-----------------------------------------------------------------------------
acttable_t CWeaponImmolator::m_acttable[] = 
{
	{	ACT_RANGE_ATTACK1, ACT_RANGE_ATTACK_SNIPER_RIFLE, true }
};

IMPLEMENT_ACTTABLE( CWeaponImmolator );

//-----------------------------------------------------------------------------
// Constructor
//-----------------------------------------------------------------------------
CWeaponImmolator::CWeaponImmolator( void )
{
	m_fMaxRange1 = 4096;
	StopImmolating();
}

bool CWeaponImmolator::Deploy( void )
{
	m_fireState = FIRE_OFF;

	return BaseClass::Deploy();
}

bool CWeaponImmolator::Holster( CBaseCombatWeapon *pSwitchingTo )
{
    StopImmolating();

	return BaseClass::Holster( pSwitchingTo );
}

/*
bool CWeaponImmolator::HasAmmo( void )
{
	CBasePlayer *pOwner = ToBasePlayer( GetOwner() );
	if ( pOwner == NULL )
		return false;

	if ( pOwner->GetAmmoCount( m_iPrimaryAmmoType ) <= 0 )
		return false;

	return true;
}
*/

void CWeaponImmolator::UseAmmo( int count )
{
	CBasePlayer *pOwner = ToBasePlayer( GetOwner() );
	if ( pOwner == NULL )
		return;

	if ( pOwner->GetAmmoCount( m_iPrimaryAmmoType ) >= count )
		pOwner->RemoveAmmo( count, m_iPrimaryAmmoType );
	else
		pOwner->RemoveAmmo( pOwner->GetAmmoCount( m_iPrimaryAmmoType ), m_iPrimaryAmmoType );
}

void CWeaponImmolator::StartImmolating()
{
	// Start the radius really tiny because we use radius == 0.0 to 
	// determine whether the immolator is operating or not.
	m_flBurnRadius = 0.1;
	m_flTimeLastUpdatedRadius = gpGlobals->curtime;
	SetThink( UpdateThink );
	SetNextThink( gpGlobals->curtime );
}

void CWeaponImmolator::StopImmolating()
{
	m_flBurnRadius = 0.0;
	SetThink( NULL );
	m_vecImmolatorTarget= IMMOLATOR_TARGET_INVALID;
	m_flNextPrimaryAttack = gpGlobals->curtime + 2.0;

	StopSound( "Weapon_Immolator.Run" );
	
	if ( m_fireState != FIRE_OFF )
	{
		 EmitSound( "Weapon_Immolator.Off" );
	}

	SetWeaponIdleTime( gpGlobals->curtime + 2.0 );
	m_flNextPrimaryAttack = gpGlobals->curtime + 0.5;

	m_fireState = FIRE_OFF;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CWeaponImmolator::Precache( void )
{
	m_beamIndex = engine->PrecacheModel( "sprites/bluelaser1.vmt" );

	BaseClass::Precache();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CWeaponImmolator::PrimaryAttack( void )
{
/*
	CBasePlayer *pOwner = ToBasePlayer( GetOwner() );
	if ( pOwner == NULL )
		return;

	WeaponSound( SINGLE );

	if( !IsImmolating() )
	{
		StartImmolating();
	} 

	// VXP
	pOwner->m_fEffects |= EF_MUZZLEFLASH;
	m_flNextPrimaryAttack = gpGlobals->curtime + GetFireRate();
*/

	switch( m_fireState )
	{
		case FIRE_OFF:
		{
			if ( !HasAmmo() )
			{
				Msg( "No ammo!\n" );
				m_flNextPrimaryAttack = gpGlobals->curtime + 0.25;
				WeaponSound( EMPTY );
				return;
			}

			m_flAmmoUseTime = gpGlobals->curtime;// start using ammo ASAP.
			
			EmitSound( "Weapon_Immolator.Start" ); // VXP: Play startup sound here
			
			SendWeaponAnim( ACT_VM_PRIMARYATTACK );
			
			m_flStartFireTime = gpGlobals->curtime;

			SetWeaponIdleTime( gpGlobals->curtime + 0.1 );
			
			m_fireState = FIRE_STARTUP;
		}
		break;

		case FIRE_STARTUP:
		{
		//	StartImmolating();

			if ( gpGlobals->curtime >= ( m_flStartFireTime + 0.3 ) )
			{
				EmitSound( "Weapon_Immolator.Run" );

				m_fireState = FIRE_CHARGE;
			}

			if ( !HasAmmo() )
			{
				StopImmolating();
				m_flNextPrimaryAttack = gpGlobals->curtime + 1.0;
			}
		}
		break;

		case FIRE_CHARGE:
		{
			if( !IsImmolating() )
			{
				StartImmolating();
			}

			if ( !HasAmmo() )
			{
				StopImmolating();
				m_flNextPrimaryAttack = gpGlobals->curtime + 1.0;
			}
		}
		break;
	}
}

//-----------------------------------------------------------------------------
// This weapon is said to have Line of Sight when it CAN'T see the target, but
// can see a place near the target than can.
//-----------------------------------------------------------------------------
bool CWeaponImmolator::WeaponLOSCondition( const Vector &ownerPos, const Vector &targetPos, bool bSetConditions )
{
	CAI_BaseNPC* npcOwner = GetOwner()->MyNPCPointer();

	if( !npcOwner )
	{
		return false;
	}

	if( IsImmolating() )
	{
		// Don't update while Immolating. This is a committed attack.
		return false;
	}

	// Assume we won't find a target.
//	m_vecImmolatorTarget = IMMOLATOR_TARGET_INVALID;
	m_vecImmolatorTarget = targetPos;

	if( npcOwner->FInViewCone( targetPos ) && npcOwner->FVisible( targetPos ) )
	{
		// We can see the target (usually the enemy). Don't fire the immolator directly at them.
		return false;
	}

	// Find a nearby immolator target that CAN see targetPOS
	float flNearest = FLT_MAX;
	CBaseEntity *pNearest = NULL;

	CBaseEntity *pEnt = gEntList.FindEntityByClassname( NULL, "info_target_immolator" );

	while( pEnt )
	{
		float flDist = ( pEnt->GetAbsOrigin() - targetPos ).Length();

		if( flDist < flNearest )
		{
			trace_t tr;

			UTIL_TraceLine( targetPos, pEnt->GetAbsOrigin(), MASK_SOLID_BRUSHONLY, NULL, COLLISION_GROUP_NONE, &tr );

			if( tr.fraction == 1.0 )
			{
				pNearest = pEnt;
				flNearest = flDist;
			}
		}

		pEnt = gEntList.FindEntityByClassname( pEnt, "info_target_immolator" );
	}

	if( pNearest )
	{
		m_vecImmolatorTarget = pNearest->GetAbsOrigin();
		return true;
	}

	return false;
}


//-----------------------------------------------------------------------------
// Purpose: Weapon firing conditions
//-----------------------------------------------------------------------------
int CWeaponImmolator::WeaponRangeAttack1Condition( float flDot, float flDist )
{
	if( m_flNextPrimaryAttack > gpGlobals->curtime )
	{
		// Too soon to attack!
		return COND_NONE;
	}

	if( IsImmolating() )
	{
		// Once is enough!
		return COND_NONE;
	}

	if(	m_vecImmolatorTarget == IMMOLATOR_TARGET_INVALID )
	{
		// No target!
		return COND_NONE;
	}

	if ( flDist > m_fMaxRange1 )
	{
		return COND_TOO_FAR_TO_ATTACK;
	}
	else if ( flDot < 0.5f )	// UNDONE: Why check this here? Isn't the AI checking this already?
	{
		return COND_NOT_FACING_ATTACK;
	}

	// !!!HACKHACK this makes the strider work! 
	// It's the Immolator's RangeAttack1, but it's the Strider's Range Attack 2.
	return COND_CAN_RANGE_ATTACK2;
}

void CWeaponImmolator::UpdateThink( void )
{
	CBaseCombatCharacter *pOwner = GetOwner();

	if( pOwner && !pOwner->IsAlive() )
	{
		StopImmolating();
		return;
	}

	Update();
	SetNextThink( gpGlobals->curtime + 0.05 );
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CWeaponImmolator::Update()
{
	float flDuration = gpGlobals->curtime - m_flTimeLastUpdatedRadius;
	if( flDuration != 0.0 )
	{
		m_flBurnRadius += RADIUS_GROW_RATE * flDuration;
	}

	// Clamp
	m_flBurnRadius = min( m_flBurnRadius, MAX_BURN_RADIUS );

	CBasePlayer *pOwner = ToBasePlayer( GetOwner() );

	Vector vecSrc;
	Vector vecStart;
	Vector vecAiming;
	QAngle dummy;

	if( pOwner )
	{
		vecSrc	 = pOwner->Weapon_ShootPosition( );

		CBaseViewModel *pViewModel = pOwner->GetViewModel();
		pViewModel->GetAttachment( pViewModel->LookupAttachment( "muzzle" ), vecStart, dummy );

		vecAiming = pOwner->GetAutoaimVector(AUTOAIM_2DEGREES);
	}
	else
	{
		CBaseCombatCharacter *pOwner = GetOwner();

		vecSrc = pOwner->Weapon_ShootPosition( );
		vecAiming = m_vecImmolatorTarget - vecSrc;
		VectorNormalize( vecAiming );

		vecStart = vecSrc;
	}

	trace_t	tr;
	UTIL_TraceLine( vecSrc, vecSrc + vecAiming * MAX_TRACE_LENGTH, MASK_SHOT, pOwner, COLLISION_GROUP_NONE, &tr );

	CSoundEnt::InsertSound( SOUND_DANGER, m_vecImmolatorTarget, 256, 5.0, GetOwner() );

	if ( gpGlobals->curtime >= m_flAmmoUseTime )
	{
		UseAmmo( 1 );
		ImmolationDamage( CTakeDamageInfo( this, this, 1, DMG_PLASMA ), tr.endpos, m_flBurnRadius, CLASS_NONE );
		
		if ( !HasAmmo() )
		{
			StopImmolating();
		}
		
		m_flAmmoUseTime = gpGlobals->curtime + 0.1;
	}

	int brightness;
	brightness = 255 * (m_flBurnRadius/MAX_BURN_RADIUS);
/*
	UTIL_Beam(  vecSrc,
				tr.endpos,
				m_beamIndex,
				0,		//halo index
				0,		//frame start
				2.0f,	//framerate
				0.1f,	//life
				20,		// width
				1,		// endwidth
				0,		// fadelength,
				1,		// noise

				0,		// red
				255,	// green
				0,		// blue,

				brightness, // bright
				100  // speed
				);
*/
	CBeam *pBeam = CBeam::BeamCreate( "sprites/bluelaser1.vmt", 20 );
	pBeam->PointEntInit( vecStart, this );
	pBeam->SetAbsStartPos( tr.endpos );
	pBeam->SetEndAttachment( 1 );
	pBeam->SetScrollRate( 2.0f );		//framerate
	pBeam->LiveForTime( 0.1f );			//life
	pBeam->SetEndWidth( 1 );			// endwidth
	pBeam->SetFadeLength( 0 );			// fadelength,
	pBeam->SetNoise( 1 );				// noise
//	pBeam->SetColor( 0, 255, 0 );		// red, green, blue,
//	pBeam->SetColor( 0, 165, 255 );		// red, green, blue,
	pBeam->SetBrightness( brightness );	// bright

	if( true || tr.DidHitWorld() )
	{
		int beams;

		for( beams = 0 ; beams < 5 ; beams++ )
		{
			Vector vecDest;

			// Random unit vector
			vecDest.x = random->RandomFloat( -1, 1 );
			vecDest.y = random->RandomFloat( -1, 1 );
			vecDest.z = random->RandomFloat( 0, 1 );

			// Push out to radius dist.
			vecDest = tr.endpos + vecDest * m_flBurnRadius;

			UTIL_Beam(  tr.endpos,
						vecDest,
						m_beamIndex,
						0,		//halo index
						0,		//frame start
						2.0f,	//framerate
						0.15f,	//life
						20,		// width
						1.75,	// endwidth
						0.75,	// fadelength,
						15,		// noise

					//	0,		// red
					//	255,	// green
					//	0,		// blue,

						0,		// red
						165,	// green
						255,	// blue,

						128, // bright
						100  // speed
						);
		}

	//	ImmolationDamage( CTakeDamageInfo( this, this, 1, DMG_BURN ), tr.endpos, m_flBurnRadius, CLASS_NONE );
	//	ImmolationDamage( CTakeDamageInfo( this, this, 1, DMG_PLASMA ), tr.endpos, m_flBurnRadius, CLASS_NONE );
	}
	else
	{
		// The attack beam struck some kind of entity directly.
	}

	m_flTimeLastUpdatedRadius = gpGlobals->curtime;

/*	if( m_flBurnRadius >= MAX_BURN_RADIUS )
	{
		StopImmolating();
	}*/
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CWeaponImmolator::ItemPostFrame( void )
{
	BaseClass::ItemPostFrame();
}



void CWeaponImmolator::ImmolationDamage( const CTakeDamageInfo &info, const Vector &vecSrcIn, float flRadius, int iClassIgnore )
{
	CBaseEntity *pEntity = NULL;
	trace_t		tr;
	Vector		vecSpot;

	Vector vecSrc = vecSrcIn;

	// iterate on all entities in the vicinity.
	for ( CEntitySphereQuery sphere( vecSrc, flRadius ); pEntity = sphere.GetCurrentEntity(); sphere.NextEntity() )
	{
	/* VXP: Old behavior
		if ( pEntity->m_takedamage != DAMAGE_NO )
		{
			// UNDONE: this should check a damage mask, not an ignore
			if ( iClassIgnore != CLASS_NONE && pEntity->Classify() == iClassIgnore )
			{
				continue;
			}

			pEntity->TakeDamage( info );
		}
	*/
		CBaseCombatCharacter *pBCC = pEntity->MyCombatCharacterPointer();
		if ( pBCC && pBCC->IsAlive() && !pBCC->IsOnFire() )
		{
			// UNDONE: this should check a damage mask, not an ignore
			if ( iClassIgnore != CLASS_NONE && pEntity->Classify() == iClassIgnore )
			{
				continue;
			}

		//	if ( pEntity->m_takedamage != DAMAGE_NO )
		//	{
		//		pEntity->TakeDamage( info );
		//	}

		//	if( pEntity == GetOwner() )
		//	{
		//		continue;
		//	}

		//	pBCC->Ignite( random->RandomFloat( 15, 20 ) );
		//	pBCC->Ignite( 2 );

		//	CPlasma	*plasma = (CPlasma *) CreateEntityByName( "_plasma" );
		//	plasma->EnableSmoke( true );
		//	plasma->AttachToEntity( pBCC );

		//	CPlasma	*plasma = CPlasma::Create( pBCC );
		//	plasma->EnableSmoke( true );
		//	plasma->SetLifetime( 5 );
		
			float lifetime = random->RandomFloat( 15, 20 );

			// VXP: Commented since I fixed repeated citizen pain sounds with a timer in EntityFlame.cpp
		//	if ( pEntity == GetOwner() ) // VXP: Slightly less burn lifetime for player
		//	{
		//		lifetime = random->RandomFloat( 4, 10 );
		//	}

			CEntityFlame *pFlame = CEntityFlame::Create( pBCC, true );
			if (pFlame)
			{
				pFlame->SetLifetime( lifetime );
				pBCC->AddFlag( FL_ONFIRE );
			}
		}
	}
}

void CWeaponImmolator::WeaponIdle( void )
{
	if ( !HasWeaponIdleTimeElapsed() )
		return;

	if ( m_fireState != FIRE_OFF )
		 StopImmolating();

	SendWeaponAnim( ACT_VM_IDLE );
}
