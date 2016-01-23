//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
// This is a skeleton file for use when creating a new 
// NPC. Copy and rename this file for the new
// NPC and add the copy to the build.
//
// Leave this file in the build until we ship! Allowing 
// this file to be rebuilt with the rest of the game ensures
// that it stays up to date with the rest of the NPC code.
//
// Replace occurances of CNPC_Cremator with the new NPC's
// classname. Don't forget the lower-case occurance in 
// LINK_ENTITY_TO_CLASS()
//
//
// ASSUMPTIONS MADE:
//
// You're making a character based on CAI_BaseNPC. If this 
// is not true, make sure you replace all occurances
// of 'CAI_BaseNPC' in this file with the appropriate 
// parent class.
//
// You're making a human-sized NPC that walks.
//
//=============================================================================//
/*#include "cbase.h"
#include "npc_cremator.h"
#include "physics_prop_ragdoll.h"
//#include "gib.h"
#include "grenade_brickbat.h"*/

#include "cbase.h"
#include "ai_default.h"
#include "ai_task.h"
#include "ai_schedule.h"
#include "ai_hull.h"
#include "ai_motor.h"
#include "soundent.h"
#include "game.h"
#include "npcevent.h"
#include "entitylist.h"
#include "activitylist.h"
#include "ai_basenpc.h"
#include "IEffects.h"
#include "engine/IEngineSound.h"

#include "studio.h"

#include "npc_cremator.h"
#include "grenade_brickbat.h"
#include "weapon_immolator.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

#define CREMATOR_ATTACH_MUZZLE		0
#define CREMATOR_ATTACH_HEAD		1

#define CREM_ATTN_IDLE	(float) 4.5

//=========================================================
// Private activities
//=========================================================
int	ACT_MYCUSTOMACTIVITY2 = -1;

//=========================================================
// Custom schedules
//=========================================================
enum CrematorSchedules
{
	SCHED_CREMATOR_CHASE = LAST_SHARED_SCHEDULE,
	SCHED_CREMATOR_RANGE_ATTACK1,
	SCHED_CREMATOR_IDLE,
	
};

//=========================================================
// Custom tasks
//=========================================================
enum CrematorTasks
{
	TASK_CREMATOR_ALERT = LAST_SHARED_TASK,
	TASK_CREMATOR_IDLE,
	TASK_CREMATOR_RANGE_ATTACK1,
};


//=========================================================
// Custom Conditions
//=========================================================
enum CrematorConditions
{
	COND_MYCUSTOMCONDITION = LAST_SHARED_CONDITION,
};


//---------------------------------------------------------
// Save/Restore
//---------------------------------------------------------
BEGIN_DATADESC( CNPC_Cremator )

	//DEFINE_FIELD( CNPC_Cremator, m_iDeleteThisField, FIELD_INTEGER ),
	DEFINE_FIELD( CNPC_Cremator, m_flNextIdleSoundTime, FIELD_TIME ),

END_DATADESC()

//-----------------------------------------------------------------------------
// Purpose: Initialize the custom schedules
// Input  :
// Output :
//-----------------------------------------------------------------------------
void CNPC_Cremator::InitCustomSchedules(void) 
{
	INIT_CUSTOM_AI(CNPC_Cremator);

	ADD_CUSTOM_TASK(CNPC_Cremator,	TASK_CREMATOR_ALERT);
	ADD_CUSTOM_TASK(CNPC_Cremator,	TASK_CREMATOR_IDLE);
	ADD_CUSTOM_TASK(CNPC_Cremator,	TASK_CREMATOR_RANGE_ATTACK1);

	ADD_CUSTOM_SCHEDULE(CNPC_Cremator,	SCHED_CREMATOR_CHASE);
	ADD_CUSTOM_SCHEDULE(CNPC_Cremator,	SCHED_CREMATOR_RANGE_ATTACK1);
	ADD_CUSTOM_SCHEDULE(CNPC_Cremator,	SCHED_CREMATOR_IDLE);

//	ADD_CUSTOM_ACTIVITY(CNPC_Cremator,	ACT_CONSCRIPT_AIM);

	AI_LOAD_SCHEDULE(CNPC_Cremator,	SCHED_CREMATOR_CHASE);
	AI_LOAD_SCHEDULE(CNPC_Cremator,	SCHED_CREMATOR_RANGE_ATTACK1);
	AI_LOAD_SCHEDULE(CNPC_Cremator,	SCHED_CREMATOR_IDLE);
}

LINK_ENTITY_TO_CLASS( npc_cremator, CNPC_Cremator );
IMPLEMENT_CUSTOM_AI( npc_cremator, CNPC_Cremator );

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CNPC_Cremator::Precache( void )
{
	PrecacheModel( "models/cremator.mdl" );
	PrecacheModel( "models/cremator_body.mdl" );
	PrecacheModel( "models/cremator_head.mdl" );
	
	enginesound->PrecacheSound( "npc/cremator/amb_loop.wav" );
	enginesound->PrecacheSound( "npc/cremator/amb1.wav" );
	enginesound->PrecacheSound( "npc/cremator/amb2.wav" );
	enginesound->PrecacheSound( "npc/cremator/amb3.wav" );
	enginesound->PrecacheSound( "npc/cremator/alert_object.wav" );
	enginesound->PrecacheSound( "npc/cremator/alert_player.wav" );

	PrecacheScriptSound( "NPC_Cremator.Die" );
	PrecacheScriptSound( "NPC_Cremator.SeeEnemy" );
	
	BaseClass::Precache();
}


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CNPC_Cremator::Spawn( void )
{
	Precache();

	SetModel( "models/cremator.mdl" );
	SetHullType(HULL_WIDE_TALL);
	SetHullSizeNormal();

	SetSolid( SOLID_BBOX );
	AddSolidFlags( FSOLID_NOT_STANDABLE );
	SetMoveType( MOVETYPE_STEP );
	SetBloodColor( DONT_BLEED );
	m_iMaxHealth		= 100; // VXP: Make a convar
	m_iHealth			= m_iMaxHealth;
	m_flFieldOfView		= -0.7f;
	m_NPCState			= NPC_STATE_IDLE;
	
	m_flNextIdleSoundTime	= gpGlobals->curtime;

	CapabilitiesClear();
	//CapabilitiesAdd( bits_CAP_NONE );
	CapabilitiesAdd( bits_CAP_MOVE_GROUND );
	CapabilitiesAdd( bits_CAP_INNATE_RANGE_ATTACK1 );
	CapabilitiesAdd( bits_CAP_MOVE_SHOOT );
	
	// VXP: TODO: DMG_PLASMA

	NPCInit();
	
	BaseClass::Spawn();

#ifdef _DEBUG
	studiohdr_t *pstudiohdr = GetModelPtr( );
//	mstudioseqdesc_t	*pseqdesc = pstudiohdr->pSeqdesc( 0 );
	mstudioseqdesc_t	*pseqdesc;
//	int iSequence;
	for ( int i = 0; i < pstudiohdr->numseq; i++ )
	{
	//	int iSequence = LookupSequence( "walk1" );
	//	mstudioseqdesc_t *pSeqDesc = GetSeqDesc( iSequence );
	//	Msg( "%i: %i\n", i, pstudiohdr->pSeqdesc( iSequence )->numevents );
		pseqdesc = pstudiohdr->pSeqdesc( i );
		const char *pszName = GetSequenceName( i );
		const char *pszActivityName = GetSequenceActivityName( i );
		if ( pszName != '\0' )
		{
			Msg( "%s - ", pszName );
			if ( pszActivityName[0] != '\0' )
			{
				Msg( "%s", pszActivityName );
			}
			Msg( "\n" );
		}
		for ( int j = 0; j < pseqdesc->numevents; j++ )
		{
			mstudioevent_t *pevent = pseqdesc->pEvent( j );
			Msg( "%i (%s): %i (%f, %i, %i, %s)\n", i, pseqdesc[i].pszLabel(), pseqdesc->numevents, pevent->cycle, pevent->event, pevent->type, pevent->options );
		}
	}
#endif // _DEBUG
}


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
Class_T	CNPC_Cremator::Classify( void )
{
	return	CLASS_CREMATOR;
}

void CNPC_Cremator::IdleSound( void ) // TODO: replace with modern script-based system
{
	Msg("IdleSound\n");
	CPASAttenuationFilter filter( this, CREM_ATTN_IDLE );
	EmitSound("NPC_Cremator.Breathe");
//	enginesound->EmitSound( filter, entindex(), CHAN_VOICE, "npc/cremator/amb_loop.wav", 1, ATTN_NORM );
	EmitSound("NPC_Cremator.CloakSwish");

	m_flNextIdleSoundTime = gpGlobals->curtime + random->RandomFloat( 14.0f, 28.0f );
//	m_flNextIdleSoundTime = gpGlobals->curtime + random->RandomFloat( 1.0f, 2.0f );
}

void CNPC_Cremator::AlertSound( void )
{
	int iPitch = random->RandomInt( 90, 105 );

	CPASAttenuationFilter filter( this );
	switch( random->RandomInt( 0, 1 ) )
	{
	case 0:
		enginesound->EmitSound( filter, entindex(), CHAN_VOICE, "npc/cremator/alert_object.wav", 1, ATTN_NORM, 0, iPitch );	
		break;
	case 1:
		enginesound->EmitSound( filter, entindex(), CHAN_VOICE, "npc/cremator/alert_player.wav", 1, ATTN_NORM, 0, iPitch );	
		break;
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CNPC_Cremator::DeathSound ( void )
{
	EmitSound("NPC_Cremator.Die");
//	StopSound( int iEntIndex, int iChannel, const char *pSample )
	StopSound("NPC_Cremator.Breathe");
	StopSound("NPC_Cremator.CloakSwish");
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CNPC_Cremator::FoundEnemySound ( void )
{
	EmitSound("NPC_Cremator.SeeEnemy");
}

float CNPC_Cremator::MaxYawSpeed( void )
{
	float flYS = 0;

	switch ( GetActivity() )
	{
	case	ACT_WALK_HURT:			flYS = 30;	break;
	case	ACT_RUN:			flYS = 90;	break;
	case	ACT_IDLE:			flYS = 90;	break;
	case	ACT_RANGE_ATTACK1:	flYS = 40;	break;
	default:
		flYS = 90;
		break;
	}

	return flYS;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CNPC_Cremator::Event_Killed(const CTakeDamageInfo &info) 
{
	SetModel( "models/cremator_body.mdl" );

/*	CGib *pGib = CREATE_ENTITY( CGib, "gib" );
	if( pGib )
	{
		pGib->Spawn( "models/cremator_head.mdl" );
		pGib->SetBloodColor( DONT_BLEED );
		pGib->m_nRenderMode = m_nRenderMode;
		pGib->m_clrRender = m_clrRender;
		pGib->m_nRenderFX = m_nRenderFX;
		pGib->m_flModelScale = 1.0;
		pGib->m_nSkin = m_nSkin;
		pGib->m_lifeTime = gpGlobals->curtime + 30.0f;

		Vector vecOrigin = GetAbsOrigin();
		vecOrigin.z = vecOrigin.z = 90.0f;

		pGib->SetAbsOrigin( vecOrigin );
		pGib->SetAbsAngles( GetAbsAngles() );

		pGib->SetCollisionGroup( COLLISION_GROUP_INTERACTIVE_DEBRIS );
		IPhysicsObject *pPhysicsObject = pGib->VPhysicsInitNormal( SOLID_VPHYSICS, pGib->GetSolidFlags(), false );
		pGib->SetMoveType( MOVETYPE_VPHYSICS );	  
		
		if( pPhysicsObject )
		{
			Vector vVel	= GetAbsVelocity();
			pPhysicsObject->AddVelocity(&vVel, NULL);
		}
	}*/
	
	int iBIndex;
	
	// VXP: Spawning cremator head as brickbat
	Vector vHeadPosition = GetAbsOrigin();
	QAngle vHeadAngles = GetAbsAngles();
//	vecOrigin.z += 90.0f;
	iBIndex = LookupBone( "Bip01 Head" );
	if ( iBIndex != -1 )
	{
		GetBonePosition( iBIndex, vHeadPosition, vHeadAngles );
	}
	CGrenade_Brickbat *pBrickbat = (CGrenade_Brickbat*)Create( "grenade_crematorhead", vHeadPosition, vHeadAngles, NULL );
	if ( !pBrickbat )
	{
		return;
	}
//	pBrickbat->SetCollisionGroup( COLLISION_GROUP_INTERACTIVE_DEBRIS );
	pBrickbat->SetMoveType( MOVETYPE_VPHYSICS );	  
	IPhysicsObject *pBrickbatPhysicsObject = pBrickbat->VPhysicsGetObject();
	if ( pBrickbatPhysicsObject )
	{
		Vector vVel	= GetAbsVelocity();
		// VXP: Make here ejecting head with parabola
		pBrickbatPhysicsObject->AddVelocity( &vVel, NULL );
	}
	
	// VXP: Spawning weapon_immolator
	Vector vGunPosition = GetAbsOrigin();
	QAngle vGunAngles = GetAbsAngles();
	Msg( "%f %f %f\n", vGunPosition.x, vGunPosition.y, vGunPosition.z );
//	GetAttachment( "muzzle", vGunPosition, vGunAngles );
//	GetAttachment( 0, vGunPosition, vGunAngles );
//	int iBIndex = LookupBone( "muzzle" );
	iBIndex = LookupBone( "Bip01 R Finger1" );
//	Msg( "%i, %i, %i, %i\n", LookupBone( "Bip01 R Finger1" ), LookupBone( "Bip01.R.Finger1" ), LookupBone( "muzzle" ), LookupBone( "Object08" ) );
	if ( iBIndex != -1 )
	{
		GetBonePosition( iBIndex, vGunPosition, vGunAngles );
	}
/*	GetAttachment( CREMATOR_ATTACH_MUZZLE, vGunPosition, vGunAngles );
	if ( vGunPosition == GetAbsOrigin() || vGunPosition == Vector( 0, 0, 0 ) ) // VXP: We're not done
	{
		Msg("1\n");
		Msg( "%f %f %f\n", vGunPosition.x, vGunPosition.y, vGunPosition.z );
	}
	else
	{
		Msg("2\n");
		Msg( "%f %f %f\n", vGunPosition.x, vGunPosition.y, vGunPosition.z );
	}
	vGunPosition += GetAbsOrigin();*/
	CWeaponImmolator *pImmolator = (CWeaponImmolator*)Create( "weapon_immolator", vGunPosition, vGunAngles, NULL );
	if ( !pImmolator )
	{
		return;
	}
	pImmolator->SetCollisionGroup( COLLISION_GROUP_INTERACTIVE_DEBRIS );
	pImmolator->SetMoveType( MOVETYPE_VPHYSICS );	  
	IPhysicsObject *pImmolatorPhysicsObject = pImmolator->VPhysicsGetObject();
	if ( pImmolatorPhysicsObject )
	{
		Vector vVel	= GetAbsVelocity();
		pImmolatorPhysicsObject->AddVelocity( &vVel, NULL );
	}

	BaseClass::Event_Killed( info );
}

void CNPC_Cremator::FootstepEffect( const Vector &origin )
{
	trace_t tr;
	AI_TraceLine( origin, origin - Vector(0,0,0), MASK_SOLID_BRUSHONLY, this, COLLISION_GROUP_NONE, &tr );
	float yaw = random->RandomInt(0,0);
	for ( int i = 0; i < 2; i++ )
	{
		if ( UTIL_PointContents( tr.endpos + Vector( 0, 0, 1 ) ) & MASK_WATER )
		{
		/*	float flWaterZ = UTIL_FindWaterSurface( tr.endpos, tr.endpos.z, tr.endpos.z + 100.0f );

			CEffectData	data;
			data.m_fFlags = 0;
			data.m_vOrigin = tr.endpos;
			data.m_vOrigin.z = flWaterZ;
			data.m_vNormal = Vector( 0, 0, 1 );
			data.m_flScale = random->RandomFloat( 10.0, 14.0 );

			// Если крематор идет по неглубокой воде, образуются всплески.
			DispatchEffect( "watersplash", data );*/
		}
		
		else
		{
			Vector dir = UTIL_YawToVector( yaw + i*180 ) * 10;
			VectorNormalize( dir );
			dir.z = 0.25;
			VectorNormalize( dir );
			g_pEffects->Dust( tr.endpos, dir, 12, 50 );

			/*g_pEffects->FootprintDecal( tr.endpos, dir, 12, 50 );

			virtual void FootprintDecal( IRecipientFilter& filer, float delay, const Vector *origin, const Vector* right, 
		int entity, int index, unsigned char materialType ) = 0;
			virtual void Dust( IRecipientFilter& filer, float delay,
				 const Vector &pos, const Vector &dir, float size, float speed ) = 0;*/
		}
	}
}

int CNPC_Cremator::OnTakeDamage_Alive( const CTakeDamageInfo &info )
{
	CTakeDamageInfo newInfo = info;
	if( newInfo.GetDamageType() & DMG_BURN)
	{
		newInfo.ScaleDamage( 0 );
		DevMsg( "Fire damage; no actual damage is taken\n" );
	}	

	int nDamageTaken = BaseClass::OnTakeDamage_Alive( newInfo );

//	m_bHeadshot = false;
//	m_bCanisterShot = false;

	return nDamageTaken;
}

void CNPC_Cremator::TraceAttack( const CTakeDamageInfo &info, const Vector &vecDir, trace_t *ptr )
{	
	CTakeDamageInfo newInfo = info;
	if( (newInfo.GetDamageType() & DMG_BULLET || DMG_CLUB || DMG_BUCKSHOT || DMG_ENERGYBEAM ) ) 
	{	
	/*	if( ptr->hitgroup == HITGROUP_HEAD ) 
		{			
			m_bHeadshot = 1;
		}
		else if( ptr->hitgroup == CREMATOR_CANISTER )
		{
			m_bCanisterShot = 1; // попадание в канистру на животе.
		}
		else*/
		//{
		//	newInfo.ScaleDamage( 0.25 ); // В общем случае, крематор получает толко 25% урона (т.е. имеет сопротивление 75)
		//	DevMsg( "Generic 0.25 Non-specified\n" );
		//	SetActivity( ACT_SMALL_FLINCH );
		//	TaskComplete();
		//}
	}

	BaseClass::TraceAttack( newInfo, vecDir, ptr );
}

void CNPC_Cremator::Ignite( float flFlameLifetime )
{
	BaseClass::Ignite( flFlameLifetime );

	// FIXME: use overlays when they come online
	//AddOverlay( ACT_ZOM_WALK_ON_FIRE, false );

	Activity activity = GetActivity();
	if ( activity == ACT_WALK || activity == ACT_RUN )
	{
		SetActivity( ( Activity )ACT_RUN_HURT );
	}
	else if ( activity == ACT_IDLE )
	{
	//	SetActivity( ( Activity )ACT_IDLE_ON_FIRE );
		SetSequenceByName( "fireinout" );
	}
}

void CNPC_Cremator::StartTask( const Task_t *pTask )
{
	switch ( pTask->iTask )
	{
	case TASK_CREMATOR_IDLE:
		{
			SetActivity( ACT_IDLE );
			TaskComplete();
		}
		break;
	case TASK_CREMATOR_RANGE_ATTACK1:
		{
			Vector flEnemyLKP = GetEnemyLKP();
			GetMotor()->SetIdealYawToTarget( flEnemyLKP );
		//	ResetSequence( LookupSequence( "fireinout" ) );
			Msg("1\n");
			SetSequenceByName( "fireinout" );
		}
		break;
	default:
		BaseClass::StartTask( pTask );
		break;
	}
}

void CNPC_Cremator::RunTask( const Task_t *pTask )
{
	switch ( pTask->iTask )
	{
	case TASK_CREMATOR_RANGE_ATTACK1:
		{
			Vector flEnemyLKP = GetEnemyLKP();
			GetMotor()->SetIdealYawToTargetAndUpdate( flEnemyLKP );
			if ( IsActivityFinished() || IsActivityMovementPhased( ACT_WALK ) || IsActivityMovementPhased( ACT_RUN ) )
			{				
			//	StopParticleEffects( this );
				TaskComplete();
				return;
			}
		}
		break;
	}
	BaseClass::RunTask( pTask );
}

Vector CNPC_Cremator::FootHit( float eventtime, bool right )
{	
	Vector footPosition;
	QAngle dummy;
	int iBIndex;
	
	if ( !right )
	{
	//	iBIndex = LookupBone( "Bip01 L Toe0" );
		iBIndex = LookupBone( "Bip01 L Foot" );
		if ( iBIndex != -1 )
		{
			GetBonePosition( iBIndex, footPosition, dummy );
		}
		footPosition.z -= 5.0f;
		CPASAttenuationFilter filter( this );
		EmitSound( filter, entindex(), "NPC_Cremator.LeftFootstep", &footPosition, eventtime );
	}
	else
	{
	//	iBIndex = LookupBone( "Bip01 R Toe0" );
		iBIndex = LookupBone( "Bip01 R Foot" );
		if ( iBIndex != -1 )
		{
			GetBonePosition( iBIndex, footPosition, dummy );
		}
		footPosition.z -= 5.0f;
		CPASAttenuationFilter filter( this );
		EmitSound( filter, entindex(), "NPC_Cremator.RightFootstep", &footPosition, eventtime );
	}

	FootstepEffect( footPosition );
	return footPosition;
}

void CNPC_Cremator::HandleAnimEvent( animevent_t *pEvent )
{
	switch( pEvent->event )
	{
		case NPC_EVENT_LEFTFOOT: // 2050
			{	
				FootHit( pEvent->eventtime, false );
			}
			break;

		case NPC_EVENT_RIGHTFOOT: // 2051
			{				
				FootHit( pEvent->eventtime, true );
			}
			break;
		default:
			BaseClass::HandleAnimEvent( pEvent );
			break;
	}
	BaseClass::HandleAnimEvent( pEvent );
}

NPC_STATE CNPC_Cremator::SelectIdealState( void )
{
	switch( m_NPCState )
	{
	case NPC_STATE_COMBAT:
		{
			if ( GetEnemy() == NULL )
			{
				if ( !HasCondition( COND_ENEMY_DEAD ) )
				{
					SetCondition( COND_ENEMY_DEAD );
				}
				return NPC_STATE_ALERT;
			}
			else if ( HasCondition( COND_ENEMY_DEAD ) )
			{
				
			}
		}
	default:
		{
			m_IdealNPCState = BaseClass::SelectIdealState();
			break;
		}
	}

	return m_IdealNPCState;
}

Activity CNPC_Cremator::NPC_TranslateActivity( Activity baseAct )
{
	if ( baseAct == ACT_WALK || ACT_RUN )
	{
		if ( m_iHealth < ( m_iMaxHealth * 0.5 ) )
		{
			//return (Activity)ACT_WALK_HURT; // the animaton is broken, don't use it
		}
	}
	//return BaseClass::NPC_TranslateActivity( baseAct );
	return baseAct;
}

int CNPC_Cremator::RangeAttack1Conditions( float flDot, float flDist )
{
	if ( flDist > 256 )
		return COND_TOO_FAR_TO_ATTACK;
	if ( flDot < 0.7f )
		return COND_NOT_FACING_ATTACK;
	return COND_CAN_RANGE_ATTACK1;
}

int CNPC_Cremator::SelectSchedule( void )
{
	switch( m_NPCState )
	{
	case NPC_STATE_IDLE:
	case NPC_STATE_ALERT:
		{				
			if ( HasCondition ( COND_HEAR_COMBAT || COND_HEAR_PLAYER || COND_HEAR_BULLET_IMPACT ) )
				return SCHED_INVESTIGATE_SOUND;
		}
		break;
	case NPC_STATE_COMBAT:
		{
			if ( HasCondition( COND_CAN_RANGE_ATTACK1 ) )
			{
				return SCHED_RANGE_ATTACK1;
			}
			if ( HasCondition( COND_SEE_ENEMY ) )
			{
				if( HasCondition( COND_TOO_FAR_TO_ATTACK ) )
				{
					return SCHED_CREMATOR_CHASE;
				}
			}
			if ( HasCondition( COND_TOO_FAR_TO_ATTACK || COND_ENEMY_OCCLUDED ))
			{
				return SCHED_CREMATOR_CHASE;
			}
		}
		break;
	}
	return BaseClass::SelectSchedule();
}

int CNPC_Cremator::TranslateSchedule( int type ) 
{
	switch( type )
	{
	case SCHED_RANGE_ATTACK1:
		return SCHED_CREMATOR_RANGE_ATTACK1;
		break;
	case SCHED_CHASE_ENEMY:
		return SCHED_CREMATOR_CHASE;
		break;
	}
	return BaseClass::TranslateSchedule( type );
}

int CNPC_Cremator::GetSoundInterests( void )
{
	return	SOUND_WORLD			|
			SOUND_COMBAT		|
			SOUND_BULLET_IMPACT	|
			SOUND_PLAYER;
}

AI_DEFINE_SCHEDULE
(
	SCHED_CREMATOR_CHASE,

	"	Tasks"
	"		TASK_SET_TOLERANCE_DISTANCE		320"
	//"		TASK_GET_CHASE_PATH_TO_ENEMY	3072"
	"		TASK_RUN_PATH					0"
	"		TASK_WAIT_FOR_MOVEMENT			0"
	"		TASK_FACE_ENEMY				0"
	"	"
	"	Interrupts"
	"		COND_ENEMY_DEAD"
	"		COND_NEW_ENEMY"
	"		COND_CAN_RANGE_ATTACK1"
	"		COND_CAN_RANGE_ATTACK2"
);
AI_DEFINE_SCHEDULE
(
	SCHED_CREMATOR_RANGE_ATTACK1,

	"	Tasks"
	"		TASK_STOP_MOVING			0"
	"		TASK_FACE_ENEMY				0"
	"		TASK_ANNOUNCE_ATTACK		1"
	//"		TASK_CREMATOR_RANGE_ATTACK1	0"
	"		TASK_PLAY_SEQUENCE			ACTIVITY:ACT_RANGE_ATTACK1"
	"	"
	"	Interrupts"
	"		COND_NEW_ENEMY"
	//"		COND_ENEMY_DEAD"
	//"		COND_NO_PRIMARY_AMMO"
	//"		COND_TOO_FAR_TO_ATTACK"
);
AI_DEFINE_SCHEDULE
(
	SCHED_CREMATOR_IDLE,

	"	Tasks"
	//"		TASK_CREMATOR_RANGE_ATTACK1	0"
	"		TASK_PLAY_SEQUENCE			ACTIVITY:ACT_IDLE"
	"	"
	"	Interrupts"
	"		COND_NEW_ENEMY"
);
