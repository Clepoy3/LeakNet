//========= Copyright © 1996-2001, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
//=============================================================================

#include "cbase.h"
#include "grenade_hopwire.h"
//#include "rope.h"
//#include "rope_shared.h"
//#include "beam_shared.h"
#include "physics.h"
#include "physics_saverestore.h"

// VXP
#include "explode.h"

//#define HOOK_JOKE


BEGIN_DATADESC( CTetherHook )
	DEFINE_FIELD( CTetherHook, m_hTetheredOwner, FIELD_EHANDLE ),
	DEFINE_PHYSPTR( CTetherHook, m_pSpring ),
	DEFINE_FIELD( CTetherHook, m_pRope, FIELD_CLASSPTR ),
	DEFINE_FIELD( CTetherHook, m_pGlow, FIELD_CLASSPTR ),
	DEFINE_FIELD( CTetherHook, m_pBeam, FIELD_CLASSPTR ),
	DEFINE_FIELD( CTetherHook, m_bAttached, FIELD_BOOLEAN ),
	DEFINE_FUNCTION( CTetherHook, HookThink ),
END_DATADESC()

LINK_ENTITY_TO_CLASS( tetherhook, CTetherHook );

//-----------------------------------------------------------------------------
// Purpose: 
// Output : Returns true on success, false on failure.
//-----------------------------------------------------------------------------
bool CTetherHook::CreateVPhysics()
{
	// Create the object in the physics system
	IPhysicsObject *pPhysicsObject = VPhysicsInitNormal( SOLID_BBOX, 0, false );
	
	// Make sure I get touch called for static geometry
	if ( pPhysicsObject )
	{
		int flags = pPhysicsObject->GetCallbackFlags();
		flags |= CALLBACK_GLOBAL_TOUCH_STATIC;
		pPhysicsObject->SetCallbackFlags(flags);
	}

	return true;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTetherHook::Spawn( void )
{
	m_bAttached = false;

	Precache();
	SetModel( TETHERHOOK_MODEL );

	UTIL_SetSize( this, vec3_origin, vec3_origin );

	CreateVPhysics();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTetherHook::CreateRope( void )
{
	//Make sure it's not already there
	if ( m_pRope == NULL )
	{
		//Creat it between ourselves and the owning grenade
		m_pRope = CRopeKeyframe::Create( this, m_hTetheredOwner, 0, 0 );
		
		if ( m_pRope != NULL )
		{
			m_pRope->m_Width		= 0.75;
			m_pRope->m_RopeLength	= 32;
			m_pRope->m_Slack		= 64;

			CPASAttenuationFilter filter( this,"TripwireGrenade.ShootRope" );
			EmitSound( filter, entindex(),"TripwireGrenade.ShootRope" );
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : *pOther - 
//-----------------------------------------------------------------------------
void CTetherHook::StartTouch( CBaseEntity *pOther )
{
	if ( m_bAttached == false )
	{
		m_bAttached = true;

		SetVelocity( vec3_origin, vec3_origin );
		SetMoveType( MOVETYPE_NONE );
		VPhysicsDestroyObject();

		EmitSound( "TripwireGrenade.Hook" );

		StopSound( entindex(),"TripwireGrenade.ShootRope" );

		//Make a physics constraint between us and the owner
		if ( m_pSpring == NULL )
		{
			springparams_t spring;

			//FIXME: Make these real
			spring.constant			= 150.0f;
			spring.damping			= 24.0f;
			spring.naturalLength	= 32;
			spring.relativeDamping	= 0.1f;
			spring.startPosition	= GetAbsOrigin();
			spring.endPosition		= m_hTetheredOwner->WorldSpaceCenter();
			spring.useLocalPositions= false;
			
			IPhysicsObject *pEnd	= m_hTetheredOwner->VPhysicsGetObject();

			m_pSpring = physenv->CreateSpring( g_PhysWorldObject, pEnd, &spring );
		}

		SetThink( &CTetherHook::HookThink );
		SetNextThink( gpGlobals->curtime + 0.1f );
		//UTIL_Remove(this);
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : &velocity - 
//			&angVelocity - 
//-----------------------------------------------------------------------------
void CTetherHook::SetVelocity( const Vector &velocity, const AngularImpulse &angVelocity )
{
	IPhysicsObject *pPhysicsObject = VPhysicsGetObject();

	if ( pPhysicsObject != NULL )
	{
		pPhysicsObject->AddVelocity( &velocity, &angVelocity );
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTetherHook::HookThink( void )
{
	if ( m_pBeam == NULL )
	{
		m_pBeam = CBeam::BeamCreate( "sprites/rollermine_shock.vmt", 1.0f );
		m_pBeam->EntsInit( this, m_hTetheredOwner );

		m_pBeam->SetNoise( 0.5f );
		m_pBeam->SetColor( 255, 255, 255 );
		m_pBeam->SetScrollRate( 25 );
		m_pBeam->SetBrightness( 128 );
		m_pBeam->SetWidth( 4.0f );
		m_pBeam->SetEndWidth( 1.0f );
	}

	if ( m_pGlow == NULL )
	{
		m_pGlow = CSprite::SpriteCreate( "sprites/blueflare1.vmt", GetLocalOrigin(), false );

		if ( m_pGlow != NULL )
		{
			m_pGlow->SetParent( this );
			m_pGlow->SetTransparency( kRenderTransAdd, 255, 255, 255, 255, kRenderFxNone );
			m_pGlow->SetBrightness( 128, 0.1f );
			m_pGlow->SetScale( 0.5f, 0.1f );
		}
	}

	if ( m_bAttached && m_hTetheredOwner != NULL )
	{
		trace_t tr;
		Vector m_vecEnd = m_hTetheredOwner->GetAbsOrigin();

		// NOT MASK_SHOT because we want only simple hit boxes
		UTIL_TraceLine( GetAbsOrigin(), m_vecEnd, MASK_SOLID, this, COLLISION_GROUP_NONE, &tr );
	//	Msg( "Fraction is: %f\n", tr.fraction );

		CBaseEntity *pEntity = tr.m_pEnt;
		if ( pEntity && pEntity != (CBaseEntity*)m_hTetheredOwner && tr.fraction != 1.0f )
		{
		//	CBaseCombatCharacter *pBCC  = ToBaseCombatCharacter( pEntity );
		//	if (pBCC)
#ifndef HOOK_JOKE
			if ( tr.fraction > 0.001f && tr.fraction < 0.9f ) // VXP: Works fine!
#else
			if ( false )
#endif // HOOK_JOKE
			{
				if ( m_hTetheredOwner )
				{
					m_hTetheredOwner->Event_Killed( CTakeDamageInfo( (CBaseEntity*)m_hTetheredOwner, this, 100, GIB_NORMAL ) );
#ifdef _DEBUG
				//	DevMsg("Something should just explode my owner, grenade_hopwire!\n");
#endif // _DEBUG

					return;
				}
			}
		}
	}

	SetNextThink( gpGlobals->curtime + 0.1f );
}

void CTetherHook::KillHook()
{
	// VXP: First of all Think hook should never be called anymore
	SetNextThink( TICK_NEVER_THINK );
	SetThink( NULL );

	// Then we remove all the effects
	if ( m_pRope )
	{
		UTIL_Remove( m_pRope );
		m_pRope = NULL;
	}
	if ( m_pGlow )
	{
		UTIL_Remove( m_pGlow );
		m_pGlow = NULL;
	}
	if ( m_pBeam )
	{
		UTIL_Remove( m_pBeam );
		m_pBeam = NULL;
	}

	// Stopping sounds
	StopSound( entindex(), "TripwireGrenade.Hook" );
	StopSound( entindex(), "TripwireGrenade.ShootRope" );

	// And remove itself
	UTIL_Remove( this );
}

void CTetherHook::Detonate()
{
	ExplosionCreate( GetAbsOrigin(), GetAbsAngles(), m_hTetheredOwner, 100, 250, true );
	UTIL_ScreenShake( GetAbsOrigin(), 25.0, 150.0, 1.0, 250, SHAKE_START );

	KillHook();
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : &origin - 
//			&angles - 
//			*pOwner - 
//-----------------------------------------------------------------------------
CTetherHook	*CTetherHook::Create( const Vector &origin, const QAngle &angles, CGrenadeHopWire *pOwner )
{
	CTetherHook *pHook = CREATE_ENTITY( CTetherHook, "tetherhook" );
	
	if ( pHook != NULL )
	{
		pHook->m_hTetheredOwner = pOwner;
		pHook->SetAbsOrigin( origin );
		pHook->SetAbsAngles( angles );
		pHook->SetOwnerEntity( (CBaseEntity *) pOwner );
		pHook->Spawn();
	}

	return pHook;
}

//-----------------------------------------------------------------------------

#define GRENADE_MODEL "models/Weapons/w_hopwire.mdl"

BEGIN_DATADESC( CGrenadeHopWire )
	DEFINE_FIELD( CGrenadeHopWire, m_nHooksShot, FIELD_INTEGER ),
	DEFINE_FIELD( CGrenadeHopWire, m_pGlow, FIELD_CLASSPTR ),

	DEFINE_FUNCTION( CGrenadeHopWire, TetherThink ),
	DEFINE_FUNCTION( CGrenadeHopWire, CombatThink ),
END_DATADESC()

LINK_ENTITY_TO_CLASS( npc_grenade_hopwire, CGrenadeHopWire );

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CGrenadeHopWire::Spawn( void )
{
	Precache();

	SetModel( GRENADE_MODEL );

	m_nHooksShot	= 0;
	m_pGlow			= NULL;

	CreateVPhysics();
}

//-----------------------------------------------------------------------------
// Purpose: 
// Output : Returns true on success, false on failure.
//-----------------------------------------------------------------------------
bool CGrenadeHopWire::CreateVPhysics()
{
	// Create the object in the physics system
	VPhysicsInitNormal( SOLID_BBOX, 0, false );
	return true;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CGrenadeHopWire::Precache( void )
{
	engine->PrecacheModel( GRENADE_MODEL );
	BaseClass::Precache();
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : timer - 
//-----------------------------------------------------------------------------
void CGrenadeHopWire::SetTimer( float timer )
{
	SetThink( &CBaseGrenade::PreDetonate );
	SetNextThink( gpGlobals->curtime + timer );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CGrenadeHopWire::CombatThink( void )
{
	if ( m_pGlow == NULL )
	{
		m_pGlow = CSprite::SpriteCreate( "sprites/blueflare1.vmt", GetLocalOrigin(), false );

		if ( m_pGlow != NULL )
		{
			m_pGlow->SetParent( this );
			m_pGlow->SetTransparency( kRenderTransAdd, 255, 255, 255, 255, kRenderFxNone );
			m_pGlow->SetBrightness( 128, 0.1f );
			m_pGlow->SetScale( 1.0f, 0.1f );
		}
	}

	//TODO: Go boom... or something	
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CGrenadeHopWire::TetherThink( void )
{
	CTetherHook *pHook = NULL;
	static Vector velocity = RandomVector( -1.0f, 1.0f );

	//Create a tether hook
	pHook = (CTetherHook *) CTetherHook::Create( GetLocalOrigin(), GetLocalAngles(), this );

	if ( pHook == NULL )
		return;

	pHook->CreateRope();

	if ( m_nHooksShot % 2 )
	{
		velocity.Negate();
	}
	else
	{
		velocity = RandomVector( -1.0f, 1.0f );
	}

	pHook->SetVelocity( velocity * 1500.0f, vec3_origin );

	m_nHooksShot++;

	m_hooksList.AddToTail( pHook ); // VXP

#ifndef HOOK_JOKE
	if ( m_nHooksShot == MAX_HOOKS )
#else
	if ( false )
#endif // HOOK_JOKE
	{
		//TODO: Play a startup noise
		SetThink( &CGrenadeHopWire::CombatThink );
		SetNextThink( gpGlobals->curtime + 0.1f );
	}
	else
	{
		SetThink( &CGrenadeHopWire::TetherThink );
#ifndef HOOK_JOKE
		SetNextThink( gpGlobals->curtime + random->RandomFloat( 0.1f, 0.3f ) );
#else
		SetNextThink( gpGlobals->curtime + 0.1f );

		Msg( "m_nHooksShot = %i\n", m_nHooksShot );
		if ( m_nHooksShot >= 180 )
			Event_Killed( CTakeDamageInfo( this, this, 100, GIB_NORMAL ) );
#endif // HOOK_JOKE
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : &velocity - 
//			&angVelocity - 
//-----------------------------------------------------------------------------
void CGrenadeHopWire::SetVelocity( const Vector &velocity, const AngularImpulse &angVelocity )
{
	IPhysicsObject *pPhysicsObject = VPhysicsGetObject();
	
	if ( pPhysicsObject != NULL )
	{
		pPhysicsObject->AddVelocity( &velocity, &angVelocity );
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CGrenadeHopWire::Detonate( void )
{
	Vector	hopVel = RandomVector( -8, 8 );
	hopVel[2] += 800.0f;

	AngularImpulse	hopAngle = RandomAngularImpulse( -300, 300 );

	//FIXME: We should find out how tall the ceiling is and always try to hop halfway

	//Add upwards velocity for the "hop"
	SetVelocity( hopVel, hopAngle );

	//Shoot 4-8 cords out to grasp the surroundings
	SetThink( &CGrenadeHopWire::TetherThink );
	SetNextThink( gpGlobals->curtime + 0.6f );
}

//-----------------------------------------------------------------------------
// Purpose:
// Input  :
// Output :
//-----------------------------------------------------------------------------
void CGrenadeHopWire::Event_Killed( const CTakeDamageInfo &info )
{
	m_takedamage		= DAMAGE_NO;

	// VXP: We don't need a delay
//	SetThink( DelayDeathThink );
//	SetNextThink( gpGlobals->curtime + random->RandomFloat( 0.1, 0.3 ) );

	if ( m_pGlow )
	{
		UTIL_Remove( m_pGlow );
		m_pGlow = NULL;
	}

	for ( int i = 0; i < m_hooksList.Count(); i++ )
	{
		m_hooksList[i]->Detonate();
	}

	trace_t tr;
	UTIL_TraceLine ( GetAbsOrigin() + RandomVector( -8, 8 ), GetAbsOrigin() - RandomVector( -64, 64 ),  MASK_SOLID, this, COLLISION_GROUP_NONE, & tr);
	UTIL_ScreenShake( GetAbsOrigin(), 25.0, 150.0, 1.0, 750, SHAKE_START );
	Explode( &tr, DMG_BLAST ); // VXP: TODO: Maybe use ExplosionCreate just like at tether hooks?

//	EmitSound( "TripmineGrenade.StopSound" );

	UTIL_Remove( this );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
CBaseGrenade *HopWire_Create( const Vector &position, const QAngle &angles, const Vector &velocity, const AngularImpulse &angVelocity, CBaseEntity *pOwner, float timer )
{
	CGrenadeHopWire *pGrenade = (CGrenadeHopWire *) CBaseEntity::Create( "npc_grenade_hopwire", position, angles, pOwner );
	
	pGrenade->SetTimer( timer );
	pGrenade->SetVelocity( velocity, angVelocity );
	pGrenade->SetOwner( ToBaseCombatCharacter( pOwner ) );

	return pGrenade;
}
