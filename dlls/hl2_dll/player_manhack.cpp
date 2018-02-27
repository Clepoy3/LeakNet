//========= Copyright © 1996-2002, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================

#include "cbase.h"

//#if 0
#include "ai_basenpc.h"
#include "basecombatweapon.h"	// For SpawnBlood
#include "soundent.h"
#include "func_break.h"		// For materials
#include "hl2_player.h"
#include "ai_hull.h"
#include "decals.h"
#include "shake.h"
#include "ndebugoverlay.h"
#include "player_control.h"
#include "IEffects.h"
#include "engine/IEngineSound.h"


#define PMANHACK_OFFSET		Vector(0,0,-18)
#define PMANHACK_HULL_MINS	Vector(-22,-22,-22)
#define PMANHACK_HULL_MAXS	Vector(22,22,22)
#define PMANHACK_MAX_SPEED	400
#define PMANHACK_DECAY		6


class CPlayer_Manhack : public CPlayer_Control
{
public:
	DECLARE_CLASS( CPlayer_Manhack, CPlayer_Control );

	void	Precache(void);
	void	Spawn(void);
	bool	CreateVPhysics( void );

	// Inputs
	void InputActivate( inputdata_t &inputdata );
	void InputDeactivate( inputdata_t &inputdata );
	void InputSetThrust( inputdata_t &inputdata );
	void InputSetSideThrust( inputdata_t &inputdata );

	void	FlyThink(void);
	void	CheckBladeTrace(trace_t &tr);

	DECLARE_DATADESC();
	
private:
	bool		m_bFlying;
	bool		m_bSideFlying;
	float		m_flFlyThrust;
	float		m_flSideFlyThrust;
};


BEGIN_DATADESC( CPlayer_Manhack )

	DEFINE_FIELD( CPlayer_Manhack, m_bFlying, FIELD_BOOLEAN ),
	DEFINE_FIELD( CPlayer_Manhack, m_bSideFlying, FIELD_BOOLEAN ),
	DEFINE_FIELD( CPlayer_Manhack,	m_flFlyThrust,		FIELD_FLOAT),
	DEFINE_FIELD( CPlayer_Manhack,	m_flSideFlyThrust,		FIELD_FLOAT),

	// Function Pointers
	DEFINE_FUNCTION( CPlayer_Manhack, FlyThink ),

	// Inputs
	DEFINE_INPUTFUNC( CPlayer_Manhack, FIELD_VOID, "Activate", InputActivate ),
	DEFINE_INPUTFUNC( CPlayer_Manhack, FIELD_VOID, "Deactivate", InputDeactivate ),
	DEFINE_INPUTFUNC( CPlayer_Manhack, FIELD_FLOAT, "SetThrust", InputSetThrust ),
	DEFINE_INPUTFUNC( CPlayer_Manhack, FIELD_FLOAT, "SetSideThrust", InputSetSideThrust ),

END_DATADESC()

LINK_ENTITY_TO_CLASS( player_manhack, CPlayer_Manhack );


//------------------------------------------------------------------------------
// Purpose :
// Input   :
// Output  :
//------------------------------------------------------------------------------
void CPlayer_Manhack::Precache( void )
{
	engine->PrecacheModel("models/manhack.mdl");
	engine->PrecacheModel("models/manhack_sphere.mdl");

	enginesound->PrecacheSound( "npc/manhack/grind1.wav");
	enginesound->PrecacheSound( "npc/manhack/grind2.wav");
	enginesound->PrecacheSound( "npc/manhack/grind3.wav");
	enginesound->PrecacheSound( "npc/manhack/grind_flesh1.wav");
	enginesound->PrecacheSound( "npc/manhack/grind_flesh2.wav");
	enginesound->PrecacheSound( "npc/manhack/grind_flesh3.wav");
}

//------------------------------------------------------------------------------
// Purpose :
// Input   :
// Output  :
//------------------------------------------------------------------------------
void CPlayer_Manhack::Spawn( void )
{
	Precache();
	SetMoveType( MOVETYPE_FLY );
//	m_flFriction = 0.55; // deading the bounce a bit
	SetFriction( 0.55f );
	
	SetModel( "models/manhack.mdl" );
	UTIL_SetSize(this, PMANHACK_HULL_MINS, PMANHACK_HULL_MAXS);

	m_bActive = false;
	
	m_bFlying = false;
	m_bSideFlying = false;
	m_flFlyThrust = 0.0f;
	m_flSideFlyThrust = 0.0f;

	CreateVPhysics();
}

//-----------------------------------------------------------------------------

bool CPlayer_Manhack::CreateVPhysics( void )
{
	// Create the object in the physics system
	VPhysicsInitNormal( SOLID_BBOX, 0, false );

	return true;
}

//-----------------------------------------------------------------------------
// Purpose: Input handler for turning on the manhack
//-----------------------------------------------------------------------------
void CPlayer_Manhack::InputActivate( inputdata_t &inputdata )
{
	BaseClass::InputActivate( inputdata );

	SetThink(&CPlayer_Manhack::FlyThink);

	// Switch to a bigger model (a sphere) that keeps the player's
	// camera outside of walls
	VPhysicsDestroyObject();
	SetModel("models/manhack_sphere.mdl" );
	VPhysicsInitNormal( GetSolid(), GetSolidFlags(), false );

	CHL2_Player*	pPlayer		= (CHL2_Player*)UTIL_PlayerByIndex( 1 );

	Assert( pPlayer );

//	pPlayer->SetFOV( 132 );
	pPlayer->SetFOV( 95 );
	pPlayer->FollowEntity( this );

//	engine->SetView( pPlayer->edict(), edict() );
	pPlayer->m_nControlClass	= CLASS_MANHACK;
	pPlayer->GiveNamedItem( "weapon_manhack" );
	pPlayer->SelectItem( "weapon_manhack" );
}

//-----------------------------------------------------------------------------
// Purpose: Input handler for turning off the manhack.
//-----------------------------------------------------------------------------
void CPlayer_Manhack::InputDeactivate( inputdata_t &inputdata )
{
	BaseClass::InputDeactivate( inputdata );

	CHL2_Player*	pPlayer		= (CHL2_Player*)UTIL_PlayerByIndex( 1 );

	Assert( pPlayer );

	StopSound(pPlayer->entindex(), CHAN_ITEM, "npc/manhack/grind1.wav");
	StopSound(pPlayer->entindex(), CHAN_ITEM, "npc/manhack/grind2.wav");
	StopSound(pPlayer->entindex(), CHAN_ITEM, "npc/manhack/grind3.wav");
	StopSound(pPlayer->entindex(), CHAN_ITEM, "npc/manhack/grind_flesh1.wav");
	StopSound(pPlayer->entindex(), CHAN_ITEM, "npc/manhack/grind_flesh2.wav");
	StopSound(pPlayer->entindex(), CHAN_ITEM, "npc/manhack/grind_flesh3.wav");
	StopSound(pPlayer->entindex(), "Player_Manhack.ThrustLow" );
	StopSound(pPlayer->entindex(), "Player_Manhack.ThrustHigh" );	

	// Remove manhack blade weapon from player's inventory
	CBaseEntity* pBlade = (CBaseEntity*)(pPlayer->GetActiveWeapon());
//	pPlayer->Weapon_Drop( GetActiveWeapon() );
//	pPlayer->Weapon_Drop( GetActiveWeapon(), NULL, NULL );
	pPlayer->RemovePlayerItem( pPlayer->GetActiveWeapon() );
	if (m_pSaveWeapon)
	{
		pPlayer->Weapon_Switch( m_pSaveWeapon );
	}
	UTIL_RemoveImmediate(pBlade);
	UTIL_Relink(pPlayer);

	pPlayer->SetLocalOrigin( m_vSaveOrigin );
	pPlayer->StopFollowingEntity();
//	engine->SetView( pPlayer->edict(), pPlayer->edict() );

	// Switch back to manhack model
	VPhysicsDestroyObject();
	SetModel("models/manhack.mdl" );
	VPhysicsInitNormal( GetSolid(), GetSolidFlags(), false );
}

//------------------------------------------------------------------------------
// Purpose:
//------------------------------------------------------------------------------
void CPlayer_Manhack::InputSetThrust( inputdata_t &inputdata )
{
	if (!m_bActive)
	{
		return;
	}

	CBasePlayer*	pPlayer  = UTIL_PlayerByIndex( 1 );	

	Assert( pPlayer );

	if (inputdata.value.Float() == 0)
	{
		CPASAttenuationFilter filter( pPlayer, "Player_Manhack.ThrustLow" );
		EmitSound( filter, pPlayer->entindex(), "Player_Manhack.ThrustLow" );
		
		m_bFlying = false;
		m_flFlyThrust = 0.0f;

		return;
	}
	CPASAttenuationFilter filter( pPlayer, "Player_Manhack.ThrustHigh" );
	EmitSound( filter, pPlayer->entindex(), "Player_Manhack.ThrustHigh" );

	m_bFlying = true;
	m_flFlyThrust = inputdata.value.Float();

	Vector vForce; 
	pPlayer->EyeVectors( &vForce );
	vForce = vForce * inputdata.value.Float() * 600;

	IPhysicsObject*	pPhysObj = VPhysicsGetObject();
	pPhysObj->ApplyForceCenter( vForce );
}


//------------------------------------------------------------------------------
// Purpose:
//------------------------------------------------------------------------------
void CPlayer_Manhack::InputSetSideThrust( inputdata_t &inputdata )
{
	if (!m_bActive)
	{
		return;
	}

	CBasePlayer*	pPlayer  = UTIL_PlayerByIndex( 1 );	

	Assert( pPlayer );

	if (inputdata.value.Float() == 0)
	{
		CPASAttenuationFilter filter( pPlayer, "Player_Manhack.ThrustLow" );
		EmitSound( filter, pPlayer->entindex(), "Player_Manhack.ThrustLow" );
		
		m_bSideFlying = false;
		m_flSideFlyThrust = 0.0f;
		
		return;
	}
	CPASAttenuationFilter filter( pPlayer, "Player_Manhack.ThrustHigh" );
	EmitSound( filter, pPlayer->entindex(), "Player_Manhack.ThrustHigh" );
	
	m_bSideFlying = true;
	m_flSideFlyThrust = inputdata.value.Float();


	// Vector vForce; 
	// pPlayer->EyeVectors( 0, &vForce, 0 );
	// vForce = vForce * inputdata.value.Float() * 600;

	// IPhysicsObject*	pPhysObj = VPhysicsGetObject();
	// pPhysObj->ApplyForceCenter( vForce );
}


//------------------------------------------------------------------------------
// Purpose :
// Input   :
// Output  :
//------------------------------------------------------------------------------
void CPlayer_Manhack::CheckBladeTrace(trace_t &tr)
{
	CBaseEntity* pHitEntity = NULL;
	CBasePlayer*	pPlayer = UTIL_PlayerByIndex( 1 );

	Assert( pPlayer );

//	if (tr.u.ent)
//	if (tr.m_pEnt)
	if (engine->PEntityOfEntIndex( tr.GetEntityIndex() ))
	{
	//	pHitEntity = CBaseEntity::Instance( tr.u.ent );
	//	pHitEntity = CBaseEntity::Instance( tr.m_pEnt );
		pHitEntity = CBaseEntity::Instance( engine->PEntityOfEntIndex( tr.GetEntityIndex() ) );

		// Did I hit an entity that isn't a player?
		if (pHitEntity && pHitEntity->Classify()!=CLASS_PLAYER)
		{
			CTakeDamageInfo info( this, this, 1, DMG_SLASH );
			CalculateMeleeDamageForce( &info, (tr.endpos - tr.endpos), tr.endpos );
			pHitEntity->TakeDamage( info );
		}
	}
	if (tr.fraction != 1.0 || tr.startsolid)
	{

		CBaseCombatCharacter* pBCC = ToBaseCombatCharacter( pHitEntity );

		if (pBCC)
		{
			// Spawn some extra blood in front of player to see
			Vector vPlayerFacing;
			pPlayer->EyeVectors( &vPlayerFacing );
		//	Vector vBloodPos = pPlayer->Center() + vPlayerFacing*30;
			Vector vBloodPos = pPlayer->WorldSpaceCenter() + vPlayerFacing*30;
			SpawnBlood(vBloodPos, pBCC->BloodColor(), 1.0);

			CPASAttenuationFilter filter( pPlayer, "Player_Manhack.GrindFlesh" );
			EmitSound( filter, pPlayer->entindex(), "Player_Manhack.GrindFlesh" );
		}
		else
		{
			if (!(m_spawnflags	& SF_NPC_GAG))
			{
				CPASAttenuationFilter filter( pPlayer, "Player_Manhack.Grind" );
				EmitSound( filter, pPlayer->entindex(), "Player_Manhack.Grind" );
			}
			// For decals and sparks we must trace a line in the direction of the surface norm
			// that we hit.
			Vector vCheck = GetAbsOrigin() - (tr.plane.normal * 24);

			UTIL_TraceLine( GetAbsOrigin()+PMANHACK_OFFSET, vCheck+PMANHACK_OFFSET,MASK_SOLID_BRUSHONLY, this, COLLISION_GROUP_NONE, &tr );
			if (tr.fraction != 1.0)
			{
				g_pEffects->Sparks( tr.endpos );

				CBroadcastRecipientFilter filter;
				te->DynamicLight( filter, 0.0,
					&tr.endpos, 255, 180, 100, 0, 10, 0.1, 0 );


				// Leave decal only if colliding horizontally
				if ((DotProduct(Vector(0,0,1),tr.plane.normal)<0.5) && (DotProduct(Vector(0,0,-1),tr.plane.normal)<0.5))
				{
					UTIL_DecalTrace( &tr, "ManhackCut" );
				}
			}
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: Manhack is still dangerous when dead
// Input  :
// Output :
//-----------------------------------------------------------------------------
void CPlayer_Manhack::FlyThink()
{
	SetNextThink( gpGlobals->curtime );

	// Get physics object
	IPhysicsObject*	pPhysObj = VPhysicsGetObject();
	if (!pPhysObj) return;
	Vector vPhysPos;
	pPhysObj->GetPosition(&vPhysPos,NULL);

	// Update the player's origin
	CBasePlayer*	pPlayer  = UTIL_PlayerByIndex( 1 );	

	Assert( pPlayer );

	Vector vPlayerFacing;
	pPlayer->EyeVectors( &vPlayerFacing );

// void UTIL_SetOrigin( CBaseEntity *entity, const Vector &vecOrigin, bool bFireTriggers )
// {
	// entity->SetLocalOrigin( vecOrigin );
	// engine->RelinkEntity( entity->edict(), bFireTriggers );
// }

//	UTIL_SetOrigin(pPlayer,vPhysPos);
	pPlayer->SetAbsOrigin( vPhysPos );
	engine->RelinkEntity( pPlayer->edict(), false );
	engine->RelinkEntity( edict(), false );

	/*AngularImpulse ampImp;
	pPhysObj->GetVelocity( NULL, &ampImp );
	Vector vecForce;
	vecForce.x = vPlayerFacing.x - ampImp.x;
	vecForce.y = vPlayerFacing.y - ampImp.y;
	vecForce.z = vPlayerFacing.z - ampImp.z;
	pPhysObj->ApplyTorqueCenter( vecForce * 1000 );*/

	/*Vector vecForce;
	vecForce.x = 0 - vPlayerFacing.x;
	vecForce.y = 0;
	vecForce.z = 0 - vPlayerFacing.z;
	pPhysObj->ApplyForceOffset( vecForce, vPhysPos );*/

	/*Vector forward, right, up;
	pPlayer->GetVectors( &forward, &right, &up );
	
	float mul = 0.5f;

	QAngle angForce = QAngle( 1 - pPlayer->GetLocalAngles()[0], 0, 1 - pPlayer->GetLocalAngles()[2] );
//	QAngle angForce = QAngle( pPlayer->GetAbsAngles()[0], pPlayer->GetAbsAngles()[1], pPlayer->GetAbsAngles()[2] );

	if ( angForce[0] != 0 )
	{
	//	Vector pitch = up      * (angForce[0] * 0.5f);
		Vector pitch = up      * (angForce[0] * mul);
		pPhysObj->ApplyForceOffset( forward, pitch );
		pPhysObj->ApplyForceOffset( forward * -1, pitch * -1 );
	}

	Vector left = right * -1;
	if ( angForce[1] != 0 )
	{
		Vector yaw = forward      * (angForce[1] * mul);
		pPhysObj->ApplyForceOffset( left, yaw );
		pPhysObj->ApplyForceOffset( left * -1, yaw * -1 );
	}

	if ( angForce[2] != 0 )
	{
		Vector roll = left      * (angForce[2] * mul);
		pPhysObj->ApplyForceOffset( up, roll );
		pPhysObj->ApplyForceOffset( up * -1, roll * -1 );
	}*/

	if ( m_bFlying )
	{
		Vector vForce; 
		pPlayer->EyeVectors( &vForce );
		vForce = vForce * m_flFlyThrust * 600;

	//	IPhysicsObject*	pPhysObj = VPhysicsGetObject();
		pPhysObj->ApplyForceCenter( vForce );
	}
	
	if ( m_bSideFlying )
	{
		Vector vForce; 
		pPlayer->EyeVectors( 0, &vForce, 0 );
		vForce = vForce * m_flSideFlyThrust * 600;

	//	IPhysicsObject*	pPhysObj = VPhysicsGetObject();
		pPhysObj->ApplyForceCenter( vForce );
	}

	float flBrightness;
	if (random->RandomInt(0,1)==0)
	{
		flBrightness = 255;
	}
	else
	{
		flBrightness = 1;
	}
	color32 white = {flBrightness,flBrightness,flBrightness,20};
	UTIL_ScreenFade( pPlayer, white, 0.01, 0.1, FFADE_MODULATE );

	flBrightness = random->RandomInt(0,255);
	NDebugOverlay::ScreenText( 0.4, 0.15, "...M A N H A C K     A C T I V A T E D...", 255, flBrightness, flBrightness, 255, 0.0);

	// ----------------------------------------
	//	Trace forward to see if I hit anything
	// ----------------------------------------
	trace_t			tr;
	Vector vVelocity;
	GetVelocity( &vVelocity, NULL );
	vVelocity.z = 0;
	Vector			vCheckPos = GetAbsOrigin() + (vVelocity*gpGlobals->frametime);

	// Check in movement direction
	UTIL_TraceHull( GetAbsOrigin()+PMANHACK_OFFSET, vCheckPos+PMANHACK_OFFSET, 
			Vector(-22,-22,-1), Vector(22,22,1),
			MASK_SOLID, this, COLLISION_GROUP_NONE, &tr );
	CheckBladeTrace(tr);

	// Check in facing direction
	UTIL_TraceHull( GetAbsOrigin()+PMANHACK_OFFSET, GetAbsOrigin() + vPlayerFacing*14 + PMANHACK_OFFSET, 
			Vector(-22,-22,-1), Vector(22,22,1),
			MASK_SOLID, this, COLLISION_GROUP_NONE, &tr );
	CheckBladeTrace(tr);

	// ----------
	// Add decay
	// ----------
	Vector vOldVelocity;
	pPhysObj->GetVelocity(&vOldVelocity,NULL);
	float flDecay = -PMANHACK_DECAY*gpGlobals->frametime;
	if (flDecay < -1.0)
	{
		flDecay = -1.0;
	}
	pPhysObj->ApplyForceCenter( flDecay*vOldVelocity );

	// -----------------------
	// Hover noise
	// -----------------------
	float	flNoise1 = 1000*sin(4*gpGlobals->curtime);
	float	flNoise = 12000+ flNoise1;

	Vector vForce = Vector(0,0,flNoise*gpGlobals->frametime);
	pPhysObj->ApplyForceCenter( vForce );

	// -----------------------
	//  Limit velocity
	// -----------------------
	pPhysObj->GetVelocity(&vOldVelocity,NULL);
	if (vOldVelocity.Length() > PMANHACK_MAX_SPEED)
	{
		Vector vNewVelocity = vOldVelocity;
		VectorNormalize(vNewVelocity);
		vNewVelocity = vNewVelocity *PMANHACK_MAX_SPEED;
		Vector vSubtract = vNewVelocity - vOldVelocity;
		pPhysObj->AddVelocity(&vSubtract,NULL);
	}
}
//#endif