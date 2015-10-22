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
#include "soundent.h"
#include "game.h"
#include "npcevent.h"
#include "entitylist.h"
#include "activitylist.h"
#include "ai_basenpc.h"
#include "engine/IEngineSound.h"

#include "npc_cremator.h"
#include "grenade_brickbat.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

//=========================================================
// Private activities
//=========================================================
int	ACT_MYCUSTOMACTIVITY2 = -1;

//=========================================================
// Custom schedules
//=========================================================
enum
{
	SCHED_MYCUSTOMSCHEDULE = LAST_SHARED_SCHEDULE,
};

//=========================================================
// Custom tasks
//=========================================================
enum 
{
	TASK_MYCUSTOMTASK = LAST_SHARED_TASK,
};


//=========================================================
// Custom Conditions
//=========================================================
enum 
{
	COND_MYCUSTOMCONDITION = LAST_SHARED_CONDITION,
};

LINK_ENTITY_TO_CLASS( npc_cremator, CNPC_Cremator );
IMPLEMENT_CUSTOM_AI( npc_cremator, CNPC_Cremator );


//---------------------------------------------------------
// Save/Restore
//---------------------------------------------------------
BEGIN_DATADESC( CNPC_Cremator )

	DEFINE_FIELD( CNPC_Cremator, m_iDeleteThisField, FIELD_INTEGER ),

END_DATADESC()

//-----------------------------------------------------------------------------
// Purpose: Initialize the custom schedules
// Input  :
// Output :
//-----------------------------------------------------------------------------
void CNPC_Cremator::InitCustomSchedules(void) 
{
	INIT_CUSTOM_AI(CNPC_Cremator);

	ADD_CUSTOM_TASK(CNPC_Cremator,		TASK_MYCUSTOMTASK);

	ADD_CUSTOM_SCHEDULE(CNPC_Cremator,	SCHED_MYCUSTOMSCHEDULE);

	ADD_CUSTOM_ACTIVITY(CNPC_Cremator,	ACT_MYCUSTOMACTIVITY2);

	ADD_CUSTOM_CONDITION(CNPC_Cremator,	COND_MYCUSTOMCONDITION);
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CNPC_Cremator::Precache( void )
{
	PrecacheModel( "models/cremator.mdl" );
	PrecacheModel( "models/cremator_body.mdl" );
	PrecacheModel( "models/cremator_head.mdl" );

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
	m_iHealth			= 50;
	m_flFieldOfView		= 0.5;
	m_NPCState			= NPC_STATE_IDLE;

	CapabilitiesClear();
	//CapabilitiesAdd( bits_CAP_NONE );

	NPCInit();
}


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
Class_T	CNPC_Cremator::Classify( void )
{
	return	CLASS_CREMATOR;
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
	Vector vecOrigin = GetAbsOrigin();
	vecOrigin.z += 90.0f;
	CGrenade_Brickbat *pBrickbat = (CGrenade_Brickbat*)Create( "grenade_crematorhead", vecOrigin, GetAbsAngles(), NULL );
	if (!pBrickbat)
	{
		return;
	}
	
	pBrickbat->SetCollisionGroup( COLLISION_GROUP_INTERACTIVE_DEBRIS );
	pBrickbat->SetMoveType( MOVETYPE_VPHYSICS );	  
	
	IPhysicsObject *pPhysicsObject = pBrickbat->VPhysicsGetObject();
	if ( pPhysicsObject )
	{
		Vector vVel	= GetAbsVelocity();
		pPhysicsObject->AddVelocity( &vVel, NULL );
	}

	BaseClass::Event_Killed( info );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CNPC_Cremator::DeathSound ( void )
{
	EmitSound("NPC_Cremator.Die");
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CNPC_Cremator::FoundEnemySound ( void )
{
	EmitSound("NPC_Cremator.SeeEnemy");
}

/*int CNPC_Cremator::TranslateSchedule( int scheduleType )
{
	switch( scheduleType )
	{
		case SCHED_IDLE_WALK:
		{
			return SCHED_CUSTOM_IDLE_WALK;
			break;
		}
	}
 
	return BaseClass::TranslateSchedule( scheduleType );
}*/

/*AI_BEGIN_CUSTOM_NPC( npc_cremator, CNPC_Cremator )
	DEFINE_SCHEDULE
	(
		SCHED_DODGE_ENEMY_FIRE,
 
		"	Tasks"
		"		TASK_FIND_DODGE_DIRECTION	3"
		"		TASK_JUMP	 		0"
		""
		"	Interrupts"
		"		COND_LIGHT_DAMAGE"
	)
AI_END_CUSTOM_NPC()*/
