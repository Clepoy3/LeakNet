//========= Copyright © 1996-2003, Valve LLC, All rights reserved. ============
//
// Purpose: The various ammo types for HL2	
//
//=============================================================================

#include "cbase.h"
#include "player.h"
#include "gamerules.h"
#include "items.h"

// VXP
#include "ammodef.h"
#include "npcevent.h"

// ========================================================================
//	>> BoxSRounds
// ========================================================================
#define SIZE_BOX_SROUNDS 20

class CItem_BoxSRounds : public CItem
{
public:
	DECLARE_CLASS( CItem_BoxSRounds, CItem );

	void Spawn( void )
	{ 
		Precache( );
		SetModel( "models/items/boxsrounds.mdl" );
		BaseClass::Spawn( );
	}
	void Precache( void )
	{
		engine->PrecacheModel ("models/items/boxsrounds.mdl");
	}
	bool MyTouch( CBasePlayer *pPlayer )
	{
		if (pPlayer->GiveAmmo( SIZE_BOX_SROUNDS, "SmallRound"))
		{
			UTIL_Remove(this);	
			return true;
		}
		return false;
	}
};
LINK_ENTITY_TO_CLASS(item_box_srounds, CItem_BoxSRounds);

// ========================================================================
//	>> LargeBoxSRounds
// ========================================================================
#define SIZE_LARGE_BOX_SROUNDS 100

class CItem_LargeBoxSRounds : public CItem
{
public:
	DECLARE_CLASS( CItem_LargeBoxSRounds, CItem );

	void Spawn( void )
	{ 
		Precache( );
		SetModel( "models/items/largeboxsrounds.mdl" );
		BaseClass::Spawn( );
	}
	void Precache( void )
	{
		engine->PrecacheModel ("models/items/largeboxsrounds.mdl");
	}
	bool MyTouch( CBasePlayer *pPlayer )
	{
		if (pPlayer->GiveAmmo( SIZE_LARGE_BOX_SROUNDS, "SmallRound"))
		{
			UTIL_Remove(this);	
			return true;
		}
		return false;
	}
};
LINK_ENTITY_TO_CLASS(item_large_box_srounds, CItem_LargeBoxSRounds);

// ========================================================================
//	>> BoxMRounds
// ========================================================================
#define SIZE_BOX_MROUNDS 20

class CItem_BoxMRounds : public CItem
{
public:
	DECLARE_CLASS( CItem_BoxMRounds, CItem );

	void Spawn( void )
	{ 
		Precache( );
		SetModel( "models/items/boxmrounds.mdl");
		BaseClass::Spawn( );
	}
	void Precache( void )
	{
		engine->PrecacheModel ("models/items/boxmrounds.mdl");
	}
	bool MyTouch( CBasePlayer *pPlayer )
	{
		if (pPlayer->GiveAmmo( SIZE_BOX_MROUNDS, "MediumRound"))
		{
			UTIL_Remove(this);	
			return true;
		}
		return false;
	}
};
LINK_ENTITY_TO_CLASS(item_box_mrounds, CItem_BoxMRounds);

// ========================================================================
//	>> LargeBoxMRounds
// ========================================================================
#define SIZE_LARGE_BOX_MROUNDS 100

class CItem_LargeBoxMRounds : public CItem
{
public:
	DECLARE_CLASS( CItem_LargeBoxMRounds, CItem );

	void Spawn( void )
	{ 
		Precache( );
		SetModel( "models/items/largeboxmrounds.mdl");
		BaseClass::Spawn( );
	}
	void Precache( void )
	{
		engine->PrecacheModel ("models/items/largeboxmrounds.mdl");
	}
	bool MyTouch( CBasePlayer *pPlayer )
	{
		if (pPlayer->GiveAmmo( SIZE_LARGE_BOX_MROUNDS, "MediumRound"))
		{
			UTIL_Remove(this);	
			return true;
		}
		return false;
	}
};
LINK_ENTITY_TO_CLASS(item_large_box_mrounds, CItem_LargeBoxMRounds);

// ========================================================================
//	>> BoxLRounds
// ========================================================================
#define SIZE_BOX_LROUNDS 20

class CItem_BoxLRounds : public CItem
{
public:
	DECLARE_CLASS( CItem_BoxLRounds, CItem );

	void Spawn( void )
	{ 
		Precache( );
		SetModel( "models/items/boxbrounds.mdl");
		BaseClass::Spawn( );
	}
	void Precache( void )
	{
		engine->PrecacheModel ("models/items/boxbrounds.mdl");
	}
	bool MyTouch( CBasePlayer *pPlayer )
	{
		if (pPlayer->GiveAmmo( SIZE_BOX_LROUNDS, "LargeRound"))
		{
			UTIL_Remove(this);	
			return true;
		}
		return false;
	}
};
LINK_ENTITY_TO_CLASS(item_box_lrounds, CItem_BoxLRounds);

// ========================================================================
//	>> LargeBoxLRounds
// ========================================================================
#define SIZE_LARGE_BOX_LROUNDS 100

class CItem_LargeBoxLRounds : public CItem
{
public:
	DECLARE_CLASS( CItem_LargeBoxLRounds, CItem );

	void Spawn( void )
	{ 
		Precache( );
		SetModel( "models/items/largeboxbrounds.mdl");
		BaseClass::Spawn( );
	}
	void Precache( void )
	{
		engine->PrecacheModel ("models/items/largeboxbrounds.mdl");
	}
	bool MyTouch( CBasePlayer *pPlayer )
	{
		if (pPlayer->GiveAmmo( SIZE_LARGE_BOX_LROUNDS, "LargeRound"))
		{
			UTIL_Remove(this);	
			return true;
		}
		return false;
	}
};
LINK_ENTITY_TO_CLASS(item_large_box_lrounds, CItem_LargeBoxLRounds);

// ========================================================================
//	>> FlareRound
// ========================================================================
class CItem_FlareRound : public CItem
{
public:
	DECLARE_CLASS( CItem_FlareRound, CItem );

	void Spawn( void )
	{ 
		Precache( );
		SetModel( "models/items/flare.mdl");
		BaseClass::Spawn( );
	}
	void Precache( void )
	{
		engine->PrecacheModel ("models/items/flare.mdl");
	}
	bool MyTouch( CBasePlayer *pPlayer )
	{
		if (pPlayer->GiveAmmo( 1, "FlareRound"))
		{
			UTIL_Remove(this);	
			return true;
		}
		return false;
	}
};
LINK_ENTITY_TO_CLASS(item_flare_round, CItem_FlareRound);

// ========================================================================
//	>> BoxFlareRounds
// ========================================================================
#define SIZE_BOX_FLARE_ROUNDS 5

class CItem_BoxFlareRounds : public CItem
{
public:
	DECLARE_CLASS( CItem_BoxFlareRounds, CItem );

	void Spawn( void )
	{ 
		Precache( );
		SetModel( "models/items/boxflares.mdl");
		BaseClass::Spawn( );
	}
	void Precache( void )
	{
		engine->PrecacheModel ("models/items/boxflares.mdl");
	}
	bool MyTouch( CBasePlayer *pPlayer )
	{
		if (pPlayer->GiveAmmo( SIZE_BOX_FLARE_ROUNDS, "FlareRound"))
		{
			UTIL_Remove(this);	
			return true;
		}
		return false;
	}
};
LINK_ENTITY_TO_CLASS(item_box_flare_rounds, CItem_BoxFlareRounds);

// ========================================================================
//	>> ML_Grenade
// ========================================================================
class CItem_ML_Grenade : public CItem
{
public:
	DECLARE_CLASS( CItem_ML_Grenade, CItem );

	void Spawn( void )
	{ 
		Precache( );
		SetModel( "models/items/ml_grenade.mdl");
		BaseClass::Spawn( );
	}
	void Precache( void )
	{
		engine->PrecacheModel ("models/items/ml_grenade.mdl");
	}
	bool MyTouch( CBasePlayer *pPlayer )
	{
		if (pPlayer->GiveAmmo( 1, "ML_Grenade"))
		{
			UTIL_Remove(this);	
			return true;
		}
		return false;
	}
};
LINK_ENTITY_TO_CLASS(item_ml_grenade, CItem_ML_Grenade);
LINK_ENTITY_TO_CLASS(item_rpg_round, CItem_ML_Grenade);

// ========================================================================
//	>> AR2_Grenade
// ========================================================================
class CItem_AR2_Grenade : public CItem
{
public:
	DECLARE_CLASS( CItem_AR2_Grenade, CItem );

	void Spawn( void )
	{ 
		Precache( );
		SetModel( "models/items/ar2_grenade.mdl");
		BaseClass::Spawn( );
	}
	void Precache( void )
	{
		engine->PrecacheModel ("models/items/ar2_grenade.mdl");
	}
	bool MyTouch( CBasePlayer *pPlayer )
	{
		if (pPlayer->GiveAmmo( 1, "AR2_Grenade"))
		{
			UTIL_Remove(this);	
			return true;
		}
		return false;
	}
};
LINK_ENTITY_TO_CLASS(item_ar2_grenade, CItem_AR2_Grenade);

// ========================================================================
//	>> BoxSniperRounds
// ========================================================================
#define SIZE_BOX_SNIPER_ROUNDS 10

class CItem_BoxSniperRounds : public CItem
{
public:
	DECLARE_CLASS( CItem_BoxSniperRounds, CItem );

	void Spawn( void )
	{ 
		Precache( );
		SetModel( "models/items/boxsniperrounds.mdl");
		BaseClass::Spawn( );
	}
	void Precache( void )
	{
		engine->PrecacheModel ("models/items/boxsniperrounds.mdl");
	}
	bool MyTouch( CBasePlayer *pPlayer )
	{
		if (pPlayer->GiveAmmo( SIZE_BOX_SNIPER_ROUNDS, "SniperRound"))
		{
			UTIL_Remove(this);	
			return true;
		}
		return false;
	}
};
LINK_ENTITY_TO_CLASS(item_box_sniper_rounds, CItem_BoxSniperRounds);


// ========================================================================
//	>> BoxBuckshot
// ========================================================================
#define SIZE_BOX_BUCKSHOT 20

class CItem_BoxBuckshot : public CItem
{
public:
	DECLARE_CLASS( CItem_BoxBuckshot, CItem );

	void Spawn( void )
	{ 
		Precache( );
		SetModel( "models/items/boxbuckshot.mdl");
		BaseClass::Spawn( );
	}
	void Precache( void )
	{
		engine->PrecacheModel ("models/items/boxbuckshot.mdl");
	}
	bool MyTouch( CBasePlayer *pPlayer )
	{
		if (pPlayer->GiveAmmo( SIZE_BOX_BUCKSHOT, "Buckshot"))
		{
			UTIL_Remove(this);	
			return true;
		}
		return false;
	}
};
LINK_ENTITY_TO_CLASS(item_box_buckshot, CItem_BoxBuckshot);

// ==================================================================
// Ammo crate which will supply infinite ammo of the specified type
// ==================================================================

// Ammo types
enum
{
	AMMOCRATE_SMALL_ROUNDS,
	AMMOCRATE_MEDIUM_ROUNDS,
	AMMOCRATE_LARGE_ROUNDS,
	AMMOCRATE_RPG_ROUNDS,
	AMMOCRATE_BUCKSHOT,

	NUM_AMMO_CRATE_TYPES,
};

// Ammo crate

class CItem_AmmoCrate : public CBaseAnimating
{
public:
	DECLARE_CLASS( CItem_AmmoCrate, CBaseAnimating );

	void	Spawn( void );
	void	Precache( void );
	bool	CreateVPhysics( void );

	void	GiveAmmo(); // VXP
	virtual void HandleAnimEvent( animevent_t *pEvent );

	void	SetupCrate( void );
	void	OnRestore( void );

	//FIXME: May not want to have this used in a radius
	int		ObjectCaps( void ) { return (BaseClass::ObjectCaps() | (FCAP_IMPULSE_USE|FCAP_USE_IN_RADIUS)); };
	void	Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );

	void	InputKill( inputdata_t &data );
	void	CrateThink( void );
	
	virtual int OnTakeDamage( const CTakeDamageInfo &info );

protected:

	int		m_nAmmoType;
	int		m_nAmmoIndex;

	static const char *m_lpzModelNames[NUM_AMMO_CRATE_TYPES];
	static const char *m_lpzAmmoNames[NUM_AMMO_CRATE_TYPES];
	static int m_nAmmoAmounts[NUM_AMMO_CRATE_TYPES];
	static const char *m_pGiveWeapon[NUM_AMMO_CRATE_TYPES];

	float	m_flCloseTime;
	COutputEvent	m_OnUsed;
	CHandle< CBasePlayer > m_hActivator;

	DECLARE_DATADESC();
};

LINK_ENTITY_TO_CLASS( item_ammo_crate, CItem_AmmoCrate );

BEGIN_DATADESC( CItem_AmmoCrate )

	DEFINE_KEYFIELD( CItem_AmmoCrate, m_nAmmoType,	FIELD_INTEGER, "AmmoType" ),	

	DEFINE_FIELD( CItem_AmmoCrate, m_flCloseTime, FIELD_FLOAT ),
	DEFINE_FIELD( CItem_AmmoCrate, m_hActivator, FIELD_EHANDLE ),

	// These can be recreated
	//DEFINE_FIELD( CItem_AmmoCrate, m_nAmmoIndex,		FIELD_INTEGER ),
	//DEFINE_FIELD( CItem_AmmoCrate, m_lpzModelNames,	FIELD_ ),
	//DEFINE_FIELD( CItem_AmmoCrate, m_lpzAmmoNames,	FIELD_ ),
	//DEFINE_FIELD( CItem_AmmoCrate, m_nAmmoAmounts,	FIELD_INTEGER ),

	DEFINE_OUTPUT( CItem_AmmoCrate, m_OnUsed, "OnUsed" ),

	DEFINE_INPUTFUNC( CItem_AmmoCrate, FIELD_VOID, "Kill", InputKill ),

	DEFINE_THINKFUNC( CItem_AmmoCrate, CrateThink ),

END_DATADESC()

//-----------------------------------------------------------------------------
// Animation events.
//-----------------------------------------------------------------------------

// Models names
const char *CItem_AmmoCrate::m_lpzModelNames[NUM_AMMO_CRATE_TYPES] =
{
	"models/items/ammocrate_bullets.mdl",	// Small rounds
	"models/items/ammocrate_bullets.mdl",		// Medium rounds
	"models/items/ammocrate_bullets.mdl",		// Large rounds
	"models/items/ammocrate_rockets.mdl",	// RPG rounds
	"models/items/ammocrate_bullets.mdl",	// Buckshot
};

// Ammo type names
const char *CItem_AmmoCrate::m_lpzAmmoNames[NUM_AMMO_CRATE_TYPES] =
{
	/*
	"Pistol",		
	"SMG1",			
	"AR2",			
	"RPG_Round",	
	"Buckshot",		
	*/

	"SmallRound",		
	"MediumRound",			
	"LargeRound",			
	"ML_Grenade",	
	"Buckshot",		
};

// Ammo amount given per +use
int CItem_AmmoCrate::m_nAmmoAmounts[NUM_AMMO_CRATE_TYPES] =
{
	/* // VXP: Source 2007/EP1
	300,	// Pistol
	300,	// SMG1
	300,	// AR2
	3,		// RPG rounds
	120,	// Buckshot
	*/

	30,		// Pistol
	30,		// SMG1/AR2
	30,		// HMG1
	3,		// RPG rounds
	12,		// Buckshot
};

const char *CItem_AmmoCrate::m_pGiveWeapon[NUM_AMMO_CRATE_TYPES] =
{
	NULL,	// Pistol
	NULL,	// SMG1
	NULL,	// AR2
	NULL,		// RPG rounds
	NULL,	// Buckshot
};

//#define	AMMO_CRATE_CLOSE_DELAY	1.5f
#define	AMMO_CRATE_CLOSE_DELAY	2.0f

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CItem_AmmoCrate::Spawn( void )
{
	Precache();

	BaseClass::Spawn();

	SetModel( STRING( GetModelName() ) );
	SetMoveType( MOVETYPE_NONE );
	SetSolid( SOLID_VPHYSICS );
	CreateVPhysics();

	ResetSequence( LookupSequence( "Idle" ) );
	SetBodygroup( 1, true );

	m_flCloseTime = gpGlobals->curtime;
	m_flAnimTime = gpGlobals->curtime;
	m_flPlaybackRate = 0.0;
//	SetCycle( 0 );
	m_flCycle = 0;

	m_takedamage = DAMAGE_EVENTS_ONLY;

}

//------------------------------------------------------------------------------
// Purpose:
//------------------------------------------------------------------------------
bool CItem_AmmoCrate::CreateVPhysics( void )
{
	return ( VPhysicsInitStatic() != NULL );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CItem_AmmoCrate::Precache( void )
{
	SetupCrate();
	PrecacheModel( STRING( GetModelName() ) );

	PrecacheScriptSound( "AmmoCrate.Open" );
	PrecacheScriptSound( "AmmoCrate.Close" );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CItem_AmmoCrate::SetupCrate( void )
{
	SetModelName( AllocPooledString( m_lpzModelNames[m_nAmmoType] ) );
	
	m_nAmmoIndex = GetAmmoDef()->Index( m_lpzAmmoNames[m_nAmmoType] );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CItem_AmmoCrate::OnRestore( void )
{
	BaseClass::OnRestore();

	// Restore our internal state
	SetupCrate();
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : *pActivator - 
//			*pCaller - 
//			useType - 
//			value - 
//-----------------------------------------------------------------------------
void CItem_AmmoCrate::Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	CBasePlayer *pPlayer = ToBasePlayer( pActivator );

	if ( pPlayer == NULL )
		return;

	m_hActivator = pPlayer;

	m_OnUsed.FireOutput( pActivator, this );

	GiveAmmo(); // VXP: Like it was intended at original beta build

	int iSequence = LookupSequence( "Open" );

	// See if we're not opening already
	if ( GetSequence() != iSequence )
	{
		Vector mins, maxs;
		trace_t tr;

	//	CollisionProp()->WorldSpaceAABB( &mins, &maxs );
		WorldSpaceAABB( &mins, &maxs );

		Vector vOrigin = GetAbsOrigin();
		vOrigin.z += ( maxs.z - mins.z );
		mins = (mins - GetAbsOrigin()) * 0.2f;
		maxs = (maxs - GetAbsOrigin()) * 0.2f;
		mins.z = ( GetAbsOrigin().z - vOrigin.z );  
		
		UTIL_TraceHull( vOrigin, vOrigin, mins, maxs, MASK_SOLID, this, COLLISION_GROUP_NONE, &tr );

		if ( tr.startsolid || tr.allsolid )
			 return;
			
	//	m_hActivator = pPlayer;

		// Animate!
		ResetSequence( iSequence );

		// Make sound
		CPASAttenuationFilter sndFilter( this, "AmmoCrate.Open" );
		EmitSound( sndFilter, entindex(), "AmmoCrate.Open" );

		// Start thinking to make it return
		SetThink( &CItem_AmmoCrate::CrateThink );
		SetNextThink( gpGlobals->curtime + 0.1f );
	}

	// Don't close again for two seconds
	m_flCloseTime = gpGlobals->curtime + AMMO_CRATE_CLOSE_DELAY;
}

//-----------------------------------------------------------------------------
// Purpose: allows the crate to open up when hit by a crowbar
//-----------------------------------------------------------------------------
int CItem_AmmoCrate::OnTakeDamage( const CTakeDamageInfo &info )
{
	// if it's the player hitting us with a crowbar, open up
	CBasePlayer *player = ToBasePlayer(info.GetAttacker());
	if (player)
	{
		CBaseCombatWeapon *weapon = player->GetActiveWeapon();

		if (weapon && !stricmp(weapon->GetName(), "weapon_crowbar"))
		{
			// play the normal use sound
			player->EmitSound( "HL2Player.Use" );
			// open the crate
			Use(info.GetAttacker(), info.GetAttacker(), USE_TOGGLE, 0.0f);
		}
	}

	// don't actually take any damage
	return 0;
}


void CItem_AmmoCrate::GiveAmmo()
{
	if ( m_hActivator == NULL )
	{
		return;
	}

	if ( m_pGiveWeapon[m_nAmmoType] && !m_hActivator->Weapon_OwnsThisType( m_pGiveWeapon[m_nAmmoType] ) )
	{
		CBaseEntity *pEntity = CreateEntityByName( m_pGiveWeapon[m_nAmmoType] );
		CBaseCombatWeapon *pWeapon = dynamic_cast<CBaseCombatWeapon*>(pEntity);
		if ( pWeapon )
		{
			pWeapon->SetAbsOrigin( m_hActivator->GetAbsOrigin() );
			pWeapon->m_iPrimaryAmmoType = 0;
			pWeapon->m_iSecondaryAmmoType = 0;
			pWeapon->Spawn();
			if ( !m_hActivator->BumpWeapon( pWeapon ) )
			{
				UTIL_Remove( pEntity );
			}
			else
			{
				SetBodygroup( 1, false );
			}
		}
	}

	if ( m_hActivator->GiveAmmo( m_nAmmoAmounts[m_nAmmoType], m_nAmmoIndex ) != 0 )
	{
		SetBodygroup( 1, false );
	}
	m_hActivator = NULL;
}


//-----------------------------------------------------------------------------
// Purpose: Catches the monster-specific messages that occur when tagged
//			animation frames are played.
// Input  : *pEvent - 
//-----------------------------------------------------------------------------
void CItem_AmmoCrate::HandleAnimEvent( animevent_t *pEvent )
{
	/*
	if ( pEvent->event == AE_AMMOCRATE_PICKUP_AMMO )
	{
		GiveAmmo();
		return;
	}
	*/

	BaseClass::HandleAnimEvent( pEvent );
}

	
//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CItem_AmmoCrate::CrateThink( void )
{
	StudioFrameAdvance();
	DispatchAnimEvents( this );

	SetNextThink( gpGlobals->curtime + 0.1f );

	// Start closing if we're not already
	if ( GetSequence() != LookupSequence( "Close" ) )
	{
		// Not ready to close?
		if ( m_flCloseTime <= gpGlobals->curtime )
		{
			m_hActivator = NULL;

			ResetSequence( LookupSequence( "Close" ) );
		}
	}
	else
	{
		// See if we're fully closed
		if ( IsSequenceFinished() )
		{
			// Stop thinking
			SetThink( NULL );
			CPASAttenuationFilter sndFilter( this, "AmmoCrate.Close" );
			EmitSound( sndFilter, entindex(), "AmmoCrate.Close" );

			// FIXME: We're resetting the sequence here
			// but setting Think to NULL will cause this to never have
			// StudioFrameAdvance called. What are the consequences of that?
			ResetSequence( LookupSequence( "Idle" ) );
			SetBodygroup( 1, true );
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : &data - 
//-----------------------------------------------------------------------------
void CItem_AmmoCrate::InputKill( inputdata_t &data )
{
	UTIL_Remove( this );
}
