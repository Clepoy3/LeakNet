//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose:		Zombie Assassin. Big scary-ass headcrab zombie, based heavily on the Bullsquid.+
//				+++Repurposed into the Zombie Assassin
//				++Can now Jump and Climb like the fast zombie
// $NoKeywords: $
//=============================================================================//

#include "cbase.h"
#include "game.h"
#include "AI_Default.h"
#include "AI_Schedule.h"
#include "AI_Hull.h"
#include "AI_Route.h"
#include "AI_Hint.h"
#include "AI_Navigator.h"
#include "AI_Senses.h"
#include "NPCEvent.h"
#include "animation.h"
#include "npc_zassassin.h"
#include "gib.h"
#include "soundent.h"
#include "ndebugoverlay.h"
#include "vstdlib/random.h"
#include "engine/IEngineSound.h"
//#include "hl1_grenade_spit.h"
#include "util.h"
#include "shake.h"
#include "movevars_shared.h"
#include "decals.h"
#include "hl2_shareddefs.h"
#include "hl2_gamerules.h"
#include "ammodef.h"

/*
#include "player.h"
#include "ai_network.h"
#include "ai_navigator.h"
#include "ai_motor.h"
#include "ai_default.h"
#include "ai_schedule.h"
#include "ai_hull.h"
#include "ai_node.h"
#include "ai_memory.h"
#include "ai_senses.h"
#include "bitstring.h"
#include "EntityFlame.h"
#include "hl2_shareddefs.h"
#include "npcevent.h"
#include "activitylist.h"
#include "entitylist.h"
#include "gib.h"
#include "soundenvelope.h"
#include "ndebugoverlay.h"
#include "rope.h"
#include "rope_shared.h"
#include "igamesystem.h"
#include "vstdlib/random.h"
#include "engine/IEngineSound.h"
#include "props.h"
#include "hl2_gamerules.h"
#include "weapon_physcannon.h"
#include "ammodef.h"
#include "vehicle_base.h"
*/

#define		ZOMBIE_ASSASSIN_SPRINT_DIST	256


#define ZOMBIE_BURN_TIME		10 // If ignited, burn for this many seconds
#define ZOMBIE_BURN_TIME_NOISE	2  // Give or take this many seconds.

ConVar sk_zombie_assassin_health ( "sk_zombie_assassin_health", "300" );
ConVar sk_zombie_assassin_dmg_bite ( "sk_zombie_assassin_dmg_bite", "25" );
ConVar sk_zombie_assassin_dmg_whip ( "sk_zombie_assassin_dmg_whip", "35" );
ConVar sk_zombie_assassin_dmg_spit ( "sk_zombie_assassin_dmg_spit", "15" );

//=========================================================
// monster-specific schedule types
//=========================================================
enum
{
	SCHED_ZOMBIE_ASSASSIN_HURTHOP = LAST_SHARED_SCHEDULE,
	SCHED_ZOMBIE_ASSASSIN_SEECRAB,
	SCHED_ZOMBIE_ASSASSIN_EAT,
	SCHED_ZOMBIE_ASSASSIN_SNIFF_AND_EAT,
	SCHED_ZOMBIE_ASSASSIN_WALLOW,
	SCHED_ZOMBIE_ASSASSIN_CHASE_ENEMY,
};

//=========================================================
// monster-specific tasks
//=========================================================
enum 
{
	TASK_ZOMBIE_ASSASSIN_HOPTURN = LAST_SHARED_TASK,
	TASK_ZOMBIE_ASSASSIN_EAT,
};

//-----------------------------------------------------------------------------
// Zombie Assassin Conditions
//-----------------------------------------------------------------------------
enum
{
	COND_ZOMBIE_ASSASSIN_SMELL_FOOD	= LAST_SHARED_CONDITION + 1,
};


//=========================================================
// Monster's Anim Events Go Here
//=========================================================
#define	ZOMBIE_ASSASSIN_AE_SPIT			1
#define	ZOMBIE_ASSASSIN_AE_BITE			2
#define	ZOMBIE_ASSASSIN_AE_BLINK		3
#define	ZOMBIE_ASSASSIN_AE_TAILWHIP		4
#define	ZOMBIE_ASSASSIN_AE_HOP			5
#define	ZOMBIE_ASSASSIN_AE_THROW		6

LINK_ENTITY_TO_CLASS( monster_zassassin, CNPC_ZombieAssassin );
LINK_ENTITY_TO_CLASS( npc_zassassin, CNPC_ZombieAssassin ); //For Zombie Assassin version -Stacker

int ACT_ZOMBIE_ASSASSIN_EXCITED;
int ACT_ZOMBIE_ASSASSIN_EAT;
int ACT_ZOMBIE_ASSASSIN_DETECT_SCENT;
int ACT_ZOMBIE_ASSASSIN_INSPECT_FLOOR;

//=========================================================
// Zombie Assassin's spit projectile
//=========================================================
class CZombieAssassinSpit : public CBaseEntity
{
	DECLARE_CLASS( CZombieAssassinSpit, CBaseEntity );
public:
	void Spawn( void );
	void Precache( void );

	static void Shoot( CBaseEntity *pOwner, Vector vecStart, Vector vecVelocity );
	void Touch( CBaseEntity *pOther );
	void Animate( void );

	int m_nZombieAssassinSpitSprite;

	DECLARE_DATADESC();

	void SetSprite( CBaseEntity *pSprite )
	{
		m_hSprite = pSprite;	
	}

	CBaseEntity *GetSprite( void )
	{
		return m_hSprite.Get();
	}

private:
	EHANDLE m_hSprite;


};

LINK_ENTITY_TO_CLASS( zombieassassinspit, CZombieAssassinSpit );

BEGIN_DATADESC( CZombieAssassinSpit )
	DEFINE_FIELD( CZombieAssassinSpit, m_nZombieAssassinSpitSprite, FIELD_INTEGER ),
	DEFINE_FIELD( CZombieAssassinSpit, m_hSprite, FIELD_EHANDLE ),
END_DATADESC()


void CZombieAssassinSpit::Precache( void )
{
	m_nZombieAssassinSpitSprite = PrecacheModel("sprites/za_spit.vmt");// client side spittle.

	PrecacheScriptSound( "NPC_ZombieAssassin.SpitTouch1" );
	PrecacheScriptSound( "NPC_ZombieAssassin.SpitHit1" );
	PrecacheScriptSound( "NPC_ZombieAssassin.SpitHit2" );

	PrecacheScriptSound( "Zombie.AttackHit" );
	PrecacheScriptSound( "Zombie.AttackMiss" );
}

void CZombieAssassinSpit:: Spawn( void )
{
	Precache();

	SetMoveType ( MOVETYPE_FLY );
	SetClassname( "zombieassassinspit" );
	
	SetSolid( SOLID_BBOX );

	m_nRenderMode = kRenderTransAlpha;
	SetRenderColorA( 255 );
	SetModel( "" );

	SetSprite( CSprite::SpriteCreate( "sprites/za_spit.vmt", GetAbsOrigin(), true ) );
	SetRenderColor( 150, 0, 0, 255 );
	
	UTIL_SetSize( this, Vector( 0, 0, 0), Vector(0, 0, 0) );

	SetCollisionGroup( HL2COLLISION_GROUP_SPIT );
}

void CZombieAssassinSpit::Shoot( CBaseEntity *pOwner, Vector vecStart, Vector vecVelocity )
{
	CZombieAssassinSpit *pSpit = CREATE_ENTITY( CZombieAssassinSpit, "zombieassassinspit" );
	pSpit->Spawn();
	
	UTIL_SetOrigin( pSpit, vecStart );
	pSpit->SetAbsVelocity( vecVelocity );
	pSpit->SetOwnerEntity( pOwner );

	UTIL_Relink( pSpit );

	CSprite *pSprite = (CSprite*)pSpit->GetSprite();

	if ( pSprite )
	{
		pSprite->SetAttachment( pSpit, 0 );
		pSprite->SetOwnerEntity( pSpit );

		pSprite->SetScale( 0.75 );
		pSprite->SetTransparency( pSpit->m_nRenderMode, pSpit->m_clrRender->r, pSpit->m_clrRender->g, pSpit->m_clrRender->b, pSpit->m_clrRender->a, pSpit->m_nRenderFX );
		pSprite->Relink();
	}


	CPVSFilter filter( vecStart );

	VectorNormalize( vecVelocity );
	te->SpriteSpray( filter, 0.0, &vecStart , &vecVelocity, pSpit->m_nZombieAssassinSpitSprite, 210, 25, 15 );
}

void CZombieAssassinSpit::Touch ( CBaseEntity *pOther )
{
	trace_t tr;

	if ( pOther->GetSolidFlags() & FSOLID_TRIGGER )
		 return;

	if ( pOther->GetCollisionGroup() == HL2COLLISION_GROUP_SPIT)
	{
		return;
	}

	EmitSound( "NPC_ZombieAssassin.SpitTouch1" );
	EmitSound( "NPC_ZombieAssassin.SpitHit" );

	if ( !pOther->m_takedamage )
	{
		// make a splat on the wall
		UTIL_TraceLine( GetAbsOrigin(), GetAbsOrigin() + GetAbsVelocity() * 10, MASK_SOLID, this, COLLISION_GROUP_NONE, &tr );
		UTIL_DecalTrace(&tr, "Blood" ); //BeerSplash

		// make some flecks
		CPVSFilter filter( tr.endpos );

		te->SpriteSpray( filter, 0.0,	&tr.endpos, &tr.plane.normal, m_nZombieAssassinSpitSprite, 30, 8, 5 );

	}
	else
	{
		CTakeDamageInfo info( this, this, sk_zombie_assassin_dmg_spit.GetFloat(), DMG_BULLET );
		CalculateBulletDamageForce( &info, GetAmmoDef()->Index("9mmRound"), GetAbsVelocity(), GetAbsOrigin() );
		pOther->TakeDamage( info );
	}

	UTIL_Remove( m_hSprite );
	UTIL_Remove( this );
}


BEGIN_DATADESC( CNPC_ZombieAssassin )
	DEFINE_FIELD( CNPC_ZombieAssassin, m_fCanThreatDisplay, FIELD_BOOLEAN ),
	DEFINE_FIELD( CNPC_ZombieAssassin, m_flLastHurtTime, FIELD_TIME ),
	DEFINE_FIELD( CNPC_ZombieAssassin, m_flNextSpitTime, FIELD_TIME ),
	DEFINE_FIELD( CNPC_ZombieAssassin, m_flBurnDamage, FIELD_FLOAT ),
	DEFINE_FIELD( CNPC_ZombieAssassin, m_flBurnDamageResetTime, FIELD_TIME ),

	DEFINE_FIELD( CNPC_ZombieAssassin, m_flHungryTime, FIELD_TIME ),
END_DATADESC()

//=========================================================
// Spawn
//=========================================================
void CNPC_ZombieAssassin::Spawn()
{
	Precache( );

	SetModel( "models/zombie_assassin.mdl");
	SetHullType(HULL_HUMAN);
	SetHullSizeNormal();

	SetSolid( SOLID_BBOX );
	AddSolidFlags( FSOLID_NOT_STANDABLE );
	SetMoveType( MOVETYPE_STEP );
	m_bloodColor		= BLOOD_COLOR_YELLOW;
	
	SetRenderColor( 255, 255, 255, 255 );
	
	m_iHealth			= sk_zombie_assassin_health.GetFloat();
	m_flFieldOfView		= 0.2;// indicates the width of this monster's forward view cone ( as a dotproduct result )
	m_NPCState			= NPC_STATE_NONE;
	
	CapabilitiesClear();
	CapabilitiesAdd( bits_CAP_MOVE_GROUND | bits_CAP_INNATE_RANGE_ATTACK1 | bits_CAP_INNATE_MELEE_ATTACK1 | bits_CAP_INNATE_MELEE_ATTACK2 );
	CapabilitiesAdd( bits_CAP_MOVE_JUMP | bits_CAP_MOVE_CLIMB );
	CapabilitiesAdd( bits_CAP_OPEN_DOORS | bits_CAP_AUTO_DOORS | bits_CAP_DOORS_GROUP );
	
	m_fCanThreatDisplay	= TRUE;
	m_flNextSpitTime = gpGlobals->curtime;

	NPCInit();

	m_flDistTooFar		= 784;
}

//=========================================================
// Precache - precaches all resources this monster needs
//=========================================================
void CNPC_ZombieAssassin::Precache()
{
	BaseClass::Precache();
	
	PrecacheModel("models/zombie_assassin.mdl");
	
	PrecacheModel("sprites/za_spit.vmt");// spit projectile.

	PrecacheScriptSound( "NPC_ZombieAssassin.Idle" );
	PrecacheScriptSound( "NPC_ZombieAssassin.Pain" );
	PrecacheScriptSound( "NPC_ZombieAssassin.Alert" );
	PrecacheScriptSound( "NPC_ZombieAssassin.Die" );
	PrecacheScriptSound( "NPC_ZombieAssassin.Attack" );
	PrecacheScriptSound( "NPC_ZombieAssassin.Bite" );
	PrecacheScriptSound( "NPC_ZombieAssassin.Growl" );
}


int CNPC_ZombieAssassin::TranslateSchedule( int scheduleType )
{	
	switch	( scheduleType )
	{
		case SCHED_CHASE_ENEMY:
			return SCHED_ZOMBIE_ASSASSIN_CHASE_ENEMY;
			break;
	}

	return BaseClass::TranslateSchedule( scheduleType );
}

//-----------------------------------------------------------------------------
// Purpose: Indicates this monster's place in the relationship table.
// Output : 
//-----------------------------------------------------------------------------
Class_T	CNPC_ZombieAssassin::Classify( void )
{
	return CLASS_ZOMBIE; 
}

bool CNPC_ZombieAssassin::IsJumpLegal(const Vector &startPos, const Vector &apex, const Vector &endPos) const
{
	const float MAX_JUMP_RISE		= 400.0f;
	const float MAX_JUMP_DISTANCE	= 800.0f;
	const float MAX_JUMP_DROP		= 2048.0f;

	if ( BaseClass::IsJumpLegal( startPos, apex, endPos, MAX_JUMP_RISE, MAX_JUMP_DROP, MAX_JUMP_DISTANCE ) )
	{
		// Hang onto the jump distance. The AI is going to want it.
		m_flJumpDist = (startPos - endPos).Length();

		return true;
	}
	return false;
}

//=========================================================
// IdleSound 
//=========================================================
#define ZOMBIE_ASSASSIN_ATTN_IDLE	(float)1.5
void CNPC_ZombieAssassin::IdleSound( void )
{
	CPASAttenuationFilter filter( this, ZOMBIE_ASSASSIN_ATTN_IDLE );
	EmitSound( filter, entindex(), "NPC_ZombieAssassin.Idle" );	
}

//=========================================================
// PainSound 
//=========================================================
void CNPC_ZombieAssassin::PainSound( const CTakeDamageInfo &info )
{
	CPASAttenuationFilter filter( this );
	EmitSound( filter, entindex(), "NPC_ZombieAssassin.Pain" );	
}

//=========================================================
// AlertSound
//=========================================================
void CNPC_ZombieAssassin::AlertSound( void )
{
	CPASAttenuationFilter filter( this );
	EmitSound( filter, entindex(), "NPC_ZombieAssassin.Alert" );	
}

//=========================================================
// DeathSound
//=========================================================
void CNPC_ZombieAssassin::DeathSound( const CTakeDamageInfo &info )
{
	CPASAttenuationFilter filter( this );
	EmitSound( filter, entindex(), "NPC_ZombieAssassin.Die" );	
}

//=========================================================
// AttackSound
//=========================================================
void CNPC_ZombieAssassin::AttackSound( void )
{
	CPASAttenuationFilter filter( this );
	EmitSound( filter, entindex(), "NPC_ZombieAssassin.Attack" );	
}

//=========================================================
// SetYawSpeed - allows each sequence to have a different
// turn rate associated with it.
//=========================================================
float CNPC_ZombieAssassin::MaxYawSpeed( void )
{
	float flYS = 0;

	switch ( GetActivity() )
	{
	case	ACT_WALK:			flYS = 90;	break;
	case	ACT_RUN:			flYS = 90;	break;
	case	ACT_IDLE:			flYS = 90;	break;
	case	ACT_RANGE_ATTACK1:	flYS = 90;	break;
	default:
		flYS = 90;
		break;
	}

	return flYS;
}

bool CNPC_ZombieAssassin::ShouldIgnite( const CTakeDamageInfo &info )
{
 	if ( IsOnFire() )
	{
		// Already burning!
		return false;
	}

	if ( info.GetDamageType() & DMG_BURN )
	{
		//
		// If we take more than ten percent of our health in burn damage within a five
		// second interval, we should catch on fire.
		//
		m_flBurnDamage += info.GetDamage();
		m_flBurnDamageResetTime = gpGlobals->curtime + 5;

		if ( m_flBurnDamage >= m_iMaxHealth * 0.1 )
		{
			Ignite(100.0f);
			return true;
		}
	}

	return false;
}

void CNPC_ZombieAssassin::PrescheduleThink( void )
{
	BaseClass::PrescheduleThink();

	if ( ( m_flBurnDamageResetTime ) && ( gpGlobals->curtime >= m_flBurnDamageResetTime ) )
	{
		m_flBurnDamage = 0;
	}
}

//=========================================================
// HandleAnimEvent - catches the monster-specific messages
// that occur when tagged animation frames are played.
//=========================================================
void CNPC_ZombieAssassin::HandleAnimEvent( animevent_t *pEvent )
{
	switch( pEvent->event )
	{
		case ZOMBIE_ASSASSIN_AE_SPIT:
		{
			if ( GetEnemy() )
			{
				Vector	vecSpitOffset;
				Vector	vecSpitDir;
				Vector  vRight, vUp, vForward;

				AngleVectors ( GetAbsAngles(), &vForward, &vRight, &vUp );

				// !!!HACKHACK - the spot at which the spit originates (in front of the mouth) was measured in 3ds and hardcoded here.
				// we should be able to read the position of bones at runtime for this info.
				vecSpitOffset = ( vRight * 8 + vForward * 60 + vUp * 50 );		
				vecSpitOffset = ( GetAbsOrigin() + vecSpitOffset );
				vecSpitDir = ( ( GetEnemy()->BodyTarget( GetAbsOrigin() ) ) - vecSpitOffset );

				VectorNormalize( vecSpitDir );

				vecSpitDir.x += random->RandomFloat( -0.05, 0.05 );
				vecSpitDir.y += random->RandomFloat( -0.05, 0.05 );
				vecSpitDir.z += random->RandomFloat( -0.05, 0 );
						
				AttackSound();
			
				CZombieAssassinSpit::Shoot( this, vecSpitOffset, vecSpitDir * 900 );
			}
		}
		break;

		case ZOMBIE_ASSASSIN_AE_BITE:
		{
		// SOUND HERE!
			CPASAttenuationFilter filter( this );
			CBaseEntity *pHurt = CheckTraceHullAttack( 70, Vector(-16,-16,-16), Vector(16,16,16), sk_zombie_assassin_dmg_bite.GetFloat(), DMG_SLASH );
			if ( pHurt )
			{
				Vector forward, up;
				AngleVectors( GetAbsAngles(), &forward, NULL, &up );
				pHurt->SetAbsVelocity( pHurt->GetAbsVelocity() - (forward * 100) );
				pHurt->SetAbsVelocity( pHurt->GetAbsVelocity() + (up * 100) );
				pHurt->SetGroundEntity( NULL );
				EmitSound( filter, entindex(), "Zombie.AttackHit" );
			}
			else // Play a random attack miss sound
			{
				EmitSound( filter, entindex(), "Zombie.AttackMiss" );
			}
		}
		break;

		case ZOMBIE_ASSASSIN_AE_TAILWHIP:
		{
			CPASAttenuationFilter filter( this );
			EmitSound( filter, entindex(), "NPC_ZombieAssassin.Bite" );	
			CBaseEntity *pHurt = CheckTraceHullAttack( 70, Vector(-16,-16,-16), Vector(16,16,16), sk_zombie_assassin_dmg_whip.GetFloat(), DMG_SLASH | DMG_ALWAYSGIB );
			if ( pHurt ) 
			{
				Vector right, up;
				AngleVectors( GetAbsAngles(), NULL, &right, &up );

				if ( pHurt->GetFlags() & ( FL_NPC | FL_CLIENT ) )
					 pHurt->ViewPunch( QAngle( 20, 0, -20 ) );
			
				pHurt->SetAbsVelocity( pHurt->GetAbsVelocity() + (right * 200) );
				pHurt->SetAbsVelocity( pHurt->GetAbsVelocity() + (up * 100) );
			}

		}
		break;

		case ZOMBIE_ASSASSIN_AE_BLINK:
		{
			// close eye. 
			m_nSkin = 1;
		}
		break;

		case ZOMBIE_ASSASSIN_AE_HOP:
		{
			float flGravity = sv_gravity.GetFloat();

			// throw the squid up into the air on this frame.
			if ( GetFlags() & FL_ONGROUND )
			{
				SetGroundEntity( NULL );
			}

			// jump into air for 0.8 (24/30) seconds
			Vector vecVel = GetAbsVelocity();
			vecVel.z += ( 0.625 * flGravity ) * 0.5;
			SetAbsVelocity( vecVel );
		}
		break;

		case ZOMBIE_ASSASSIN_AE_THROW:
			{
				// squid throws its prey IF the prey is a client. 
				CBaseEntity *pHurt = CheckTraceHullAttack( 70, Vector(-16,-16,-16), Vector(16,16,16), 0, 0 );


				if ( pHurt )
				{
					// croonchy bite sound
					CPASAttenuationFilter filter( this );
					EmitSound( filter, entindex(), "NPC_ZombieAssassin.Bite" );	

					// screeshake transforms the viewmodel as well as the viewangle. No problems with seeing the ends of the viewmodels.
					UTIL_ScreenShake( pHurt->GetAbsOrigin(), 25.0, 1.5, 0.7, 2, SHAKE_START );

					if ( pHurt->IsPlayer() )
					{
						Vector forward, up;
						AngleVectors( GetAbsAngles(), &forward, NULL, &up );
				
						pHurt->SetAbsVelocity( pHurt->GetAbsVelocity() + forward * 300 + up * 300 );
					}
				}
			}
		break;

		default:
			BaseClass::HandleAnimEvent( pEvent );
	}
}

int CNPC_ZombieAssassin::RangeAttack1Conditions( float flDot, float flDist )
{
	if ( IsMoving() && flDist >= 512 )
	{
		// squid will far too far behind if he stops running to spit at this distance from the enemy.
		return ( COND_NONE );
	}

	if ( flDist > 85 && flDist <= 784 && flDot >= 0.5 && gpGlobals->curtime >= m_flNextSpitTime )
	{
		if ( GetEnemy() != NULL )
		{
			if ( fabs( GetAbsOrigin().z - GetEnemy()->GetAbsOrigin().z ) > 256 )
			{
				// don't try to spit at someone up really high or down really low.
				return( COND_NONE );
			}
		}

		if ( IsMoving() )
		{
			// don't spit again for a long time, resume chasing enemy.
			m_flNextSpitTime = gpGlobals->curtime + 5;
		}
		else
		{
			// not moving, so spit again pretty soon.
			m_flNextSpitTime = gpGlobals->curtime + 0.5;
		}

		return( COND_CAN_RANGE_ATTACK1 );
	}

	return( COND_NONE );
}

//=========================================================
// MeleeAttack2Conditions - Zombie Assassin is a big guy, so has a longer
// melee range than most monsters. This is the tailwhip attack
//=========================================================
int CNPC_ZombieAssassin::MeleeAttack1Conditions( float flDot, float flDist )
{
	if ( GetEnemy()->m_iHealth <= sk_zombie_assassin_dmg_whip.GetFloat() && flDist <= 85 && flDot >= 0.7 )
	{
		return ( COND_CAN_MELEE_ATTACK1 );
	}
	
	return( COND_NONE );
}

//=========================================================
// MeleeAttack2Conditions - Zombie Assassin is a big guy, so has a longer
// melee range than most monsters. This is the bite attack.
// this attack will not be performed if the tailwhip attack
// is valid.
//=========================================================
int CNPC_ZombieAssassin::MeleeAttack2Conditions( float flDot, float flDist )
{
	if ( flDist <= 85 && flDot >= 0.7 && !HasCondition( COND_CAN_MELEE_ATTACK1 ) )		// The player & Zombie Assassin can be as much as their bboxes 
		 return ( COND_CAN_MELEE_ATTACK2 );
	
	return( COND_NONE );
}

bool CNPC_ZombieAssassin::FValidateHintType ( CAI_Hint *pHint )
{
	if ( pHint->HintType() == HINT_HL1_WORLD_HUMAN_BLOOD )
		 return true;

	Msg ( "Couldn't validate hint type" );

	return false;
}

void CNPC_ZombieAssassin::RemoveIgnoredConditions( void )
{
	if ( m_flHungryTime > gpGlobals->curtime )
		 ClearCondition( COND_ZOMBIE_ASSASSIN_SMELL_FOOD );

	if ( gpGlobals->curtime - m_flLastHurtTime <= 20 )
	{
		// haven't been hurt in 20 seconds, so let the squid care about stink. 
		ClearCondition( COND_SMELL );
	}

	if ( GetEnemy() != NULL )
	{
		// ( Unless after a tasty headcrab, yumm ^_^ )
		if ( FClassnameIs( GetEnemy(), "npc_citizen" ) )
			 ClearCondition( COND_SMELL );
	}

	if ( GetActivity() == ACT_MELEE_ATTACK1 )
	{
		ClearCondition( COND_LIGHT_DAMAGE );
		ClearCondition( COND_HEAVY_DAMAGE );
	}

	if ( GetActivity() == ACT_MELEE_ATTACK2 )
	{
		ClearCondition( COND_LIGHT_DAMAGE );
		ClearCondition( COND_HEAVY_DAMAGE );
	}
}

Disposition_t CNPC_ZombieAssassin::IRelationType( CBaseEntity *pTarget )
{
	if ( gpGlobals->curtime - m_flLastHurtTime < 5 && FClassnameIs ( pTarget, "npc_citizen" ) )
	{
		// if squid has been hurt in the last 5 seconds, and is getting relationship for a headcrab, 
		// tell squid to disregard crab. 
		return D_NU;
	}

	return BaseClass::IRelationType ( pTarget );
}



static float DamageForce( const Vector &size, float damage )
{ 
	float force = damage * ((32 * 32 * 72.0) / (size.x * size.y * size.z)) * 5;
	
	if ( force > 1000.0) 
	{
		force = 1000.0;
	}

	return force;
}

//=========================================================
// TakeDamage - overridden for Zombie Assassin so we can keep track
// of how much time has passed since it was last injured
//=========================================================
int CNPC_ZombieAssassin::OnTakeDamage_Alive( const CTakeDamageInfo &inputInfo )
{
	CTakeDamageInfo info = inputInfo;

	if( inputInfo.GetDamageType() & DMG_BURN )
	{
		// If a zombie is on fire it only takes damage from the fire that's attached to it. (DMG_DIRECT)
		// This is to stop zombies from burning to death 10x faster when they're standing around
		// 10 fire entities.
	/* VXP: Sorry, nope.
		if( IsOnFire() && !(inputInfo.GetDamageType() & DMG_DIRECT) )
		{
			return 0;
		}
	*/
		
		Scorch( 8, 50 );
		Ignite( 100.0f ); //THE FUCK ISN'T THIS WORKING FOR
	}

	if ( ShouldIgnite( info ) )
	{
		Ignite( 100.0f );
	}

	if ( !FClassnameIs ( inputInfo.GetAttacker(), "npc_citizen" ) )
	{
		// don't forget about headcrabs if it was a headcrab that hurt the squid.
		m_flLastHurtTime = gpGlobals->curtime;
	}

	if ( info.GetDamageType() == DMG_BULLET )
	{
		Vector vecDir = GetAbsOrigin() - info.GetInflictor()->WorldSpaceCenter();
		VectorNormalize( vecDir );
		float flForce = DamageForce( WorldAlignSize(), info.GetDamage() );
		SetAbsVelocity( GetAbsVelocity() + vecDir * flForce );
		info.ScaleDamage( 0.25f );
	}

	return BaseClass::OnTakeDamage_Alive ( inputInfo );
}


//=========================================================
// GetSoundInterests - returns a bit mask indicating which types
// of sounds this monster regards. In the base class implementation,
// monsters care about all sounds, but no scents.
//=========================================================
int CNPC_ZombieAssassin::GetSoundInterests ( void )
{
	return	SOUND_WORLD	|
			SOUND_COMBAT	|
		    SOUND_CARCASS	|
			SOUND_MEAT		|
			SOUND_GARBAGE	|
			SOUND_PLAYER;
}

//=========================================================
// OnListened - monsters dig through the active sound list for
// any sounds that may interest them. (smells, too!)
//=========================================================
void CNPC_ZombieAssassin::OnListened( void )
{
	AISoundIter_t iter;
	
	CSound *pCurrentSound;

	static int conditionsToClear[] = 
	{
		COND_ZOMBIE_ASSASSIN_SMELL_FOOD,
	};

	ClearConditions( conditionsToClear, ARRAYSIZE( conditionsToClear ) );
	
	pCurrentSound = GetSenses()->GetFirstHeardSound( &iter );
	
	while ( pCurrentSound )
	{
		// the npc cares about this sound, and it's close enough to hear.
		int condition = COND_NONE;
		
		if ( !pCurrentSound->FIsSound() )
		{
			// if not a sound, must be a smell - determine if it's just a scent, or if it's a food scent
			if ( pCurrentSound->IsSoundType( SOUND_MEAT | SOUND_CARCASS ) )
			{
				// the detected scent is a food item
				condition = COND_ZOMBIE_ASSASSIN_SMELL_FOOD;
			}
		}
		
		if ( condition != COND_NONE )
			SetCondition( condition );

		pCurrentSound = GetSenses()->GetNextHeardSound( &iter );
	}

	BaseClass::OnListened();
}

//========================================================
// RunAI - overridden for Zombie Assassin because there are things
// that need to be checked every think.
//========================================================
void CNPC_ZombieAssassin::RunAI ( void )
{
	// first, do base class stuff
	BaseClass::RunAI();

	if ( m_nSkin != 0 )
	{
		// close eye if it was open.
		m_nSkin = 0; 
	}

	if ( random->RandomInt( 0,39 ) == 0 )
	{
		m_nSkin = 1;
	}

	if ( GetEnemy() != NULL && GetActivity() == ACT_RUN )
	{
		// chasing enemy. Sprint for last bit
		if ( (GetAbsOrigin() - GetEnemy()->GetAbsOrigin()).Length2D() < ZOMBIE_ASSASSIN_SPRINT_DIST )
		{
			m_flPlaybackRate = 1.25;
		}
	}

}


void CNPC_ZombieAssassin::Ignite( float flFlameLifetime, bool bNPCOnly, float flSize, bool bCalledByLevelDesigner )
{
	BaseClass::Ignite( flFlameLifetime, bNPCOnly, flSize, bCalledByLevelDesigner );

	// Set the zombie up to burn to death in about ten seconds.
#define FLAME_DIRECT_DAMAGE_PER_SEC		5.0f
//	SetHealth( MIN( m_iHealth, FLAME_DIRECT_DAMAGE_PER_SEC * (ZOMBIE_BURN_TIME + random->RandomFloat( -ZOMBIE_BURN_TIME_NOISE, ZOMBIE_BURN_TIME_NOISE)) ) );
	SetHealth( min( m_iHealth, FLAME_DIRECT_DAMAGE_PER_SEC * (ZOMBIE_BURN_TIME + random->RandomFloat( -ZOMBIE_BURN_TIME_NOISE, ZOMBIE_BURN_TIME_NOISE)) ) );

	// FIXME: use overlays when they come online
	//AddOverlay( ACT_ZOM_WALK_ON_FIRE, false );
		Activity activity = GetActivity();
		Activity burningActivity = activity;

		if ( activity == ACT_WALK )
		{
			burningActivity = ACT_WALK_ON_FIRE;
		}
		else if ( activity == ACT_RUN )
		{
			burningActivity = ACT_RUN_ON_FIRE;
		}
		else if ( activity == ACT_IDLE )
		{
			burningActivity = ACT_IDLE_ON_FIRE;
		}

		if( HaveSequenceForActivity(burningActivity) )
		{
			// Make sure we have a sequence for this activity (torsos don't have any, for instance) 
			// to prevent the baseNPC & baseAnimating code from throwing red level errors.
			SetActivity( burningActivity );
		}
}

//=========================================================
// GetSchedule 
//=========================================================
int CNPC_ZombieAssassin::SelectSchedule( void )
{
	switch	( m_NPCState )
	{
	case NPC_STATE_ALERT:
		{
			if ( HasCondition( COND_LIGHT_DAMAGE ) || HasCondition( COND_HEAVY_DAMAGE ) )
			{
				return SCHED_ZOMBIE_ASSASSIN_HURTHOP;
			}

			if ( HasCondition( COND_ZOMBIE_ASSASSIN_SMELL_FOOD ) )
			{
				CSound		*pSound;

				pSound = GetBestScent();
				
				if ( pSound && (!FInViewCone ( pSound->GetSoundOrigin() ) || !FVisible ( pSound->GetSoundOrigin() )) )
				{
					// scent is behind or occluded
					return SCHED_ZOMBIE_ASSASSIN_SNIFF_AND_EAT;
				}

				// food is right out in the open. Just go get it.
				return SCHED_ZOMBIE_ASSASSIN_EAT;
			}

			if ( HasCondition( COND_SMELL ) )
			{
				// there's something stinky. 
				CSound		*pSound;

				pSound = GetBestScent();
				if ( pSound )
					return SCHED_ZOMBIE_ASSASSIN_WALLOW;
			}

			break;
		}
	case NPC_STATE_COMBAT:
		{
// dead enemy
			if ( HasCondition( COND_ENEMY_DEAD ) )
			{
				// call base class, all code to handle dead enemies is centralized there.
				return BaseClass::SelectSchedule();
			}

			if ( HasCondition( COND_NEW_ENEMY ) )
			{
				if ( m_fCanThreatDisplay && IRelationType( GetEnemy() ) == D_HT && FClassnameIs( GetEnemy(), "npc_citizen" ) )
				{
					// this means squid sees a headcrab!
					m_fCanThreatDisplay = FALSE;// only do the headcrab dance once per lifetime.
					return SCHED_ZOMBIE_ASSASSIN_SEECRAB;
				}
				else
				{
					return SCHED_WAKE_ANGRY;
				}
			}

			if ( HasCondition( COND_ZOMBIE_ASSASSIN_SMELL_FOOD ) )
			{
				CSound		*pSound;

				pSound = GetBestScent();
				
				if ( pSound && (!FInViewCone ( pSound->GetSoundOrigin() ) || !FVisible ( pSound->GetSoundOrigin() )) )
				{
					// scent is behind or occluded
					return SCHED_ZOMBIE_ASSASSIN_SNIFF_AND_EAT;
				}

				// food is right out in the open. Just go get it.
				return SCHED_ZOMBIE_ASSASSIN_EAT;
			}

			if ( HasCondition( COND_CAN_RANGE_ATTACK1 ) )
			{
				return SCHED_RANGE_ATTACK1;
			}

			if ( HasCondition( COND_CAN_MELEE_ATTACK1 ) )
			{
				return SCHED_MELEE_ATTACK1;
			}

			if ( HasCondition( COND_CAN_MELEE_ATTACK2 ) )
			{
				return SCHED_MELEE_ATTACK2;
			}
			
			return SCHED_CHASE_ENEMY;

			break;
		}
	}

	return BaseClass::SelectSchedule();
}



//=========================================================
// FInViewCone - returns true is the passed vector is in
// the caller's forward view cone. The dot product is performed
// in 2d, making the view cone infinitely tall. 
//=========================================================
bool CNPC_ZombieAssassin::FInViewCone ( Vector pOrigin )
{
	Vector los = ( pOrigin - GetAbsOrigin() );

	// do this in 2D
	los.z = 0;
	VectorNormalize( los );

	Vector facingDir = EyeDirection2D( );

	float flDot = DotProduct( los, facingDir );

	if ( flDot > m_flFieldOfView )
		return true;

	return false;
}

//=========================================================
// FVisible - returns true if a line can be traced from
// the caller's eyes to the target vector
//=========================================================
bool CNPC_ZombieAssassin::FVisible ( Vector vecOrigin )
{
	trace_t tr;
	Vector		vecLookerOrigin;
	
	vecLookerOrigin = EyePosition();//look through the caller's 'eyes'
//	UTIL_TraceLine(vecLookerOrigin, vecOrigin, MASK_BLOCKLOS, this/*pentIgnore*/, COLLISION_GROUP_NONE, &tr);
	UTIL_TraceLine(vecLookerOrigin, vecOrigin, MASK_OPAQUE, this/*pentIgnore*/, COLLISION_GROUP_NONE, &tr);
	
	if ( tr.fraction != 1.0 )
		 return false; // Line of sight is not established
	else
		 return true;// line of sight is valid.
}

//=========================================================
// Start task - selects the correct activity and performs
// any necessary calculations to start the next task on the
// schedule.  OVERRIDDEN for Zombie Assassin because it needs to
// know explicitly when the last attempt to chase the enemy
// failed, since that impacts its attack choices.
//=========================================================
void CNPC_ZombieAssassin::StartTask ( const Task_t *pTask )
{
	switch ( pTask->iTask )
	{
	case TASK_MELEE_ATTACK2:
		{
			CPASAttenuationFilter filter( this );
			EmitSound( filter, entindex(), "NPC_ZombieAssassin.Growl" );		
			BaseClass::StartTask ( pTask );
			break;
		}
	case TASK_ZOMBIE_ASSASSIN_HOPTURN:
		{
			SetActivity ( ACT_HOP );
			
			if ( GetEnemy() )
			{
				Vector	vecFacing = ( GetEnemy()->GetAbsOrigin() - GetAbsOrigin() );
				VectorNormalize( vecFacing );

				GetMotor()->SetIdealYaw( vecFacing );
			}

			break;
		}
	case TASK_ZOMBIE_ASSASSIN_EAT:
		{
			m_flHungryTime = gpGlobals->curtime + pTask->flTaskData;
			break;
		}

	default:
		{
			BaseClass::StartTask ( pTask );
			break;
		}
	}
}

//=========================================================
// RunTask
//=========================================================
void CNPC_ZombieAssassin::RunTask ( const Task_t *pTask )
{
	switch ( pTask->iTask )
	{
	case TASK_ZOMBIE_ASSASSIN_HOPTURN:
		{
			if ( GetEnemy() )
			{
				Vector	vecFacing = ( GetEnemy()->GetAbsOrigin() - GetAbsOrigin() );
				VectorNormalize( vecFacing );
				GetMotor()->SetIdealYaw( vecFacing );
			}

			if ( IsSequenceFinished() )
			{
				TaskComplete(); 
			}
			break;
		}
	default:
		{
			BaseClass::RunTask( pTask );
			break;
		}
	}
}

//=========================================================
// GetIdealState - Overridden for Zombie Assassin to deal with
// the feature that makes it lose interest in headcrabs for 
// a while if something injures it. 
//=========================================================
NPC_STATE CNPC_ZombieAssassin::SelectIdealState ( void )
{
	// If no schedule conditions, the new ideal state is probably the reason we're in here.
	switch ( m_NPCState )
	{
	case NPC_STATE_COMBAT:
		/*
		COMBAT goes to ALERT upon death of enemy
		*/
		{
			if ( GetEnemy() != NULL && ( HasCondition( COND_LIGHT_DAMAGE ) || HasCondition ( COND_HEAVY_DAMAGE ) ) && FClassnameIs( GetEnemy(), "npc_citizen" ) )
			{
				// if the squid has a headcrab enemy and something hurts it, it's going to forget about the crab for a while.
				SetEnemy( NULL );
				return NPC_STATE_ALERT;
			}
			break;
		}
	}

	return BaseClass::SelectIdealState();
}


//------------------------------------------------------------------------------
//
// Schedules
//
//------------------------------------------------------------------------------

AI_BEGIN_CUSTOM_NPC( monster_zassassin, CNPC_ZombieAssassin )

	DECLARE_TASK ( TASK_ZOMBIE_ASSASSIN_HOPTURN )
	DECLARE_TASK ( TASK_ZOMBIE_ASSASSIN_EAT )

	DECLARE_CONDITION( COND_ZOMBIE_ASSASSIN_SMELL_FOOD )

	DECLARE_ACTIVITY( ACT_ZOMBIE_ASSASSIN_EXCITED )
	DECLARE_ACTIVITY( ACT_ZOMBIE_ASSASSIN_EAT )
	DECLARE_ACTIVITY( ACT_ZOMBIE_ASSASSIN_DETECT_SCENT )
	DECLARE_ACTIVITY( ACT_ZOMBIE_ASSASSIN_INSPECT_FLOOR )

	//=========================================================
	// > SCHED_ZOMBIE_ASSASSIN_HURTHOP
	//=========================================================
	DEFINE_SCHEDULE
	(
		SCHED_ZOMBIE_ASSASSIN_HURTHOP,
	
		"	Tasks"
		"		TASK_STOP_MOVING			0"
		"		TASK_SOUND_WAKE				0"
		"		TASK_ZOMBIE_ASSASSIN_HOPTURN			0"
		"		TASK_FACE_ENEMY				0"
		"	"
		"	Interrupts"
	)
	
	//=========================================================
	// > SCHED_ZOMBIE_ASSASSIN_SEECRAB
	//=========================================================
	DEFINE_SCHEDULE
	(
		SCHED_ZOMBIE_ASSASSIN_SEECRAB,
	
		"	Tasks"
		"		TASK_STOP_MOVING			0"
		"		TASK_SOUND_WAKE				0"
		"		TASK_PLAY_SEQUENCE			ACTIVITY:ACT_ZOMBIE_ASSASSIN_EXCITED"
		"		TASK_FACE_ENEMY				0"
		"	"
		"	Interrupts"
		"		COND_LIGHT_DAMAGE"
		"		COND_HEAVY_DAMAGE"
	)
	
	//=========================================================
	// > SCHED_ZOMBIE_ASSASSIN_EAT
	//=========================================================
	DEFINE_SCHEDULE
	(
		SCHED_ZOMBIE_ASSASSIN_EAT,
	
		"	Tasks"
		"		TASK_STOP_MOVING					0"
		"		TASK_ZOMBIE_ASSASSIN_EAT						10"
		"		TASK_STORE_LASTPOSITION				0"
		"		TASK_GET_PATH_TO_BESTSCENT			0"
		"		TASK_WALK_PATH						0"
		"		TASK_WAIT_FOR_MOVEMENT				0"
		"		TASK_PLAY_SEQUENCE					ACTIVITY:ACT_ZOMBIE_ASSASSIN_EAT"
		"		TASK_PLAY_SEQUENCE					ACTIVITY:ACT_ZOMBIE_ASSASSIN_EAT"
		"		TASK_PLAY_SEQUENCE					ACTIVITY:ACT_ZOMBIE_ASSASSIN_EAT"
		"		TASK_ZOMBIE_ASSASSIN_EAT						50"
		"		TASK_GET_PATH_TO_LASTPOSITION		0"
		"		TASK_WALK_PATH						0"
		"		TASK_WAIT_FOR_MOVEMENT				0"
		"		TASK_CLEAR_LASTPOSITION				0"
		"	"
		"	Interrupts"
		"		COND_LIGHT_DAMAGE"
		"		COND_HEAVY_DAMAGE"
		"		COND_NEW_ENEMY"
		"		COND_SMELL"
	)
	
	//=========================================================
	// > SCHED_ZOMBIE_ASSASSIN_SNIFF_AND_EAT
	//=========================================================
	DEFINE_SCHEDULE
	(
		SCHED_ZOMBIE_ASSASSIN_SNIFF_AND_EAT,
	
		"	Tasks"
		"		TASK_STOP_MOVING					0"
		"		TASK_ZOMBIE_ASSASSIN_EAT						10"
		"		TASK_PLAY_SEQUENCE					ACTIVITY:ACT_ZOMBIE_ASSASSIN_DETECT_SCENT"
		"		TASK_STORE_LASTPOSITION				0"
		"		TASK_GET_PATH_TO_BESTSCENT			0"
		"		TASK_WALK_PATH						0"
		"		TASK_WAIT_FOR_MOVEMENT				0"
		"		TASK_PLAY_SEQUENCE					ACTIVITY:ACT_ZOMBIE_ASSASSIN_EAT"
		"		TASK_PLAY_SEQUENCE					ACTIVITY:ACT_ZOMBIE_ASSASSIN_EAT"
		"		TASK_PLAY_SEQUENCE					ACTIVITY:ACT_ZOMBIE_ASSASSIN_EAT"
		"		TASK_ZOMBIE_ASSASSIN_EAT						50"
		"		TASK_GET_PATH_TO_LASTPOSITION		0"
		"		TASK_WALK_PATH						0"
		"		TASK_WAIT_FOR_MOVEMENT				0"
		"		TASK_CLEAR_LASTPOSITION				0"
		"	"
		"	Interrupts"
		"		COND_LIGHT_DAMAGE"
		"		COND_HEAVY_DAMAGE"
		"		COND_NEW_ENEMY"
		"		COND_SMELL"
	)
	
	//=========================================================
	// > SCHED_ZOMBIE_ASSASSIN_WALLOW
	//=========================================================
	DEFINE_SCHEDULE
	(
		SCHED_ZOMBIE_ASSASSIN_WALLOW,
	
		"	Tasks"
		"		TASK_STOP_MOVING				0"
		"		TASK_ZOMBIE_ASSASSIN_EAT					10"
		"		TASK_STORE_LASTPOSITION			0"
		"		TASK_GET_PATH_TO_BESTSCENT		0"
		"		TASK_WALK_PATH					0"
		"		TASK_WAIT_FOR_MOVEMENT			0"
		"		TASK_PLAY_SEQUENCE				ACTIVITY:ACT_ZOMBIE_ASSASSIN_INSPECT_FLOOR"
		"		TASK_ZOMBIE_ASSASSIN_EAT					50"
		"		TASK_GET_PATH_TO_LASTPOSITION	0"
		"		TASK_WALK_PATH					0"
		"		TASK_WAIT_FOR_MOVEMENT			0"
		"		TASK_CLEAR_LASTPOSITION			0"
		"	"
		"	Interrupts"
		"		COND_LIGHT_DAMAGE"
		"		COND_HEAVY_DAMAGE"
		"		COND_NEW_ENEMY"
	)
	
	//=========================================================
	// > SCHED_ZOMBIE_ASSASSIN_CHASE_ENEMY
	//=========================================================
	DEFINE_SCHEDULE
	(
		SCHED_ZOMBIE_ASSASSIN_CHASE_ENEMY,
	
		"	Tasks"
		"		TASK_SET_FAIL_SCHEDULE			SCHEDULE:SCHED_RANGE_ATTACK1"
		"		TASK_GET_PATH_TO_ENEMY			0"
		"		TASK_RUN_PATH					0"
		"		TASK_WAIT_FOR_MOVEMENT			0"
		"	"
		"	Interrupts"
		"		COND_LIGHT_DAMAGE"
		"		COND_HEAVY_DAMAGE"
		"		COND_NEW_ENEMY"
		"		COND_ENEMY_DEAD"
		"		COND_SMELL"
		"		COND_CAN_RANGE_ATTACK1"
		"		COND_CAN_MELEE_ATTACK1"
		"		COND_CAN_MELEE_ATTACK2"
		"		COND_TASK_FAILED"
	)

AI_END_CUSTOM_NPC()
