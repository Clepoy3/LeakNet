//========= Copyright © 1996-2003, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
//=============================================================================

#include "cbase.h"
#include "basehlcombatweapon.h"
#include "NPCevent.h"
#include "basecombatcharacter.h"
#include "AI_BaseNPC.h"
#include "player.h"
#include "in_buttons.h"

#define DAMAGE_PER_SECOND 10

#define MAX_SETTINGS	5

float RateOfFire[ MAX_SETTINGS ] = 
{
	0.1,
	0.2,
	0.5,
	0.7,
	1.0,
};

float Damage[ MAX_SETTINGS ] =
{
	/*
	2,
	4,
	10,
	14,
	20,
	*/
	5,
	8,
	15,
	20,
	35,
};


//=========================================================
//=========================================================
class CWeaponAR1 : public CHLMachineGun
{
	DECLARE_DATADESC();
public:
	DECLARE_CLASS( CWeaponAR1, CHLMachineGun );

	DECLARE_SERVERCLASS();

	CWeaponAR1();

	int m_ROF;

	void	Precache( void );
	bool	Deploy( void );

	float GetFireRate( void ) {return RateOfFire[ m_ROF ];}

	int CapabilitiesGet( void ) { return bits_CAP_WEAPON_RANGE_ATTACK1; }

	void SecondaryAttack( void );

	void FireBullets(int cShots, const Vector &vecSrc, const Vector &vecDirShooting, const Vector &vecSpread, float flDistance, int iBulletType, int iTracerFreq);
	
	void AddViewKick( void );

	virtual const Vector& GetBulletSpread( void )
	{
		static const Vector cone = VECTOR_CONE_10DEGREES;
		return cone;
	}

	void Operator_HandleAnimEvent( animevent_t *pEvent, CBaseCombatCharacter *pOperator )
	{
		switch( pEvent->event )
		{
			case EVENT_WEAPON_AR1:
			{
				Vector vecShootOrigin, vecShootDir;
				vecShootOrigin = pOperator->Weapon_ShootPosition( );

				CAI_BaseNPC *npc = pOperator->MyNPCPointer();
				ASSERT( npc != NULL );
				
				vecShootDir = npc->GetActualShootTrajectory( vecShootOrigin );

				WeaponSound(SINGLE_NPC);
				pOperator->FireBullets( 1, vecShootOrigin, vecShootDir, VECTOR_CONE_PRECALCULATED, MAX_TRACE_LENGTH, m_iPrimaryAmmoType, 2 );
				pOperator->m_fEffects |= EF_MUZZLEFLASH;
			}
			break;
			default:
				CBaseCombatWeapon::Operator_HandleAnimEvent( pEvent, pOperator );
				break;
		}
	}
	DECLARE_ACTTABLE();
};

IMPLEMENT_SERVERCLASS_ST(CWeaponAR1, DT_WeaponAR1)
END_SEND_TABLE()

LINK_ENTITY_TO_CLASS( weapon_ar1, CWeaponAR1 );
PRECACHE_WEAPON_REGISTER(weapon_ar1);

acttable_t	CWeaponAR1::m_acttable[] = 
{
	{ ACT_RANGE_ATTACK1, ACT_RANGE_ATTACK_AR1, true },

	// VXP: Sergeant Stacker's
	{ ACT_IDLE,						ACT_IDLE_SMG1,					false },
	{ ACT_IDLE_ANGRY,				ACT_IDLE_ANGRY_SMG1,			false },
	{ ACT_WALK,						ACT_WALK_RIFLE,					false },
	{ ACT_WALK_AIM,					ACT_WALK_AIM_RIFLE,				false },
	{ ACT_WALK_CROUCH,				ACT_WALK_CROUCH_RIFLE,			false },
	{ ACT_WALK_CROUCH_AIM,			ACT_WALK_CROUCH_AIM_RIFLE,		false },
	{ ACT_RUN,						ACT_RUN_RIFLE,					false },
	{ ACT_RUN_AIM,					ACT_RUN_AIM_RIFLE,				false },
	{ ACT_RUN_CROUCH,				ACT_RUN_CROUCH_RIFLE,			false },
	{ ACT_RUN_CROUCH_AIM,			ACT_RUN_CROUCH_AIM_RIFLE,		false },
	{ ACT_GESTURE_RANGE_ATTACK1,	ACT_GESTURE_RANGE_ATTACK_SMG1,	true },
};

IMPLEMENT_ACTTABLE(CWeaponAR1);

//---------------------------------------------------------
// Save/Restore
//---------------------------------------------------------
BEGIN_DATADESC( CWeaponAR1 )

	DEFINE_FIELD( CWeaponAR1, m_ROF,			FIELD_INTEGER ),

END_DATADESC()


CWeaponAR1::CWeaponAR1( )
{
	m_ROF = 0;
}

void CWeaponAR1::Precache( void )
{
	BaseClass::Precache();
}

bool CWeaponAR1::Deploy( void )
{
	//CBaseCombatCharacter *pOwner  = m_hOwner;
	return BaseClass::Deploy();
}


//=========================================================
//=========================================================
void CWeaponAR1::FireBullets(int cShots, const Vector &vecSrc, const Vector &vecDirShooting, const Vector &vecSpread, float flDistance, int iBulletType, int iTracerFreq)
{
	if(CBasePlayer *pPlayer = ToBasePlayer( GetOwner() ))
	{
		pPlayer->FireBullets(cShots, vecSrc, vecDirShooting, vecSpread, flDistance, iBulletType, iTracerFreq, -1, -1, Damage[ m_ROF ] );
	}
}


void CWeaponAR1::SecondaryAttack( void )
{
	CBasePlayer *pPlayer = ToBasePlayer( GetOwner() );
	if ( pPlayer )
	{
		pPlayer->m_nButtons &= ~IN_ATTACK2;
	}

	m_flNextSecondaryAttack = gpGlobals->curtime + 0.1;

	m_ROF += 1;

	if( m_ROF == MAX_SETTINGS )
	{
		m_ROF = 0;
	}

	int i;

	Msg( "\n" );
	for( i = 0 ; i < MAX_SETTINGS ; i++ )
	{
		if( i == m_ROF )
		{
			Msg( "|" );
		}
		else
		{
			Msg( "-" );
		}
	}
	Msg( "\n" );
}

/*
==================================================
AddViewKick
==================================================
*/

void CWeaponAR1::AddViewKick( void )
{
	#define	EASY_DAMPEN			0.5f
	#define	MAX_VERTICAL_KICK	24.0f	//Degrees
	#define	SLIDE_LIMIT			3.0f	//Seconds
	
	//Get the view kick
	CBasePlayer *pPlayer = ToBasePlayer( GetOwner() );

	if (!pPlayer)
		return;

	DoMachineGunKick( pPlayer, EASY_DAMPEN, MAX_VERTICAL_KICK, m_fFireDuration, SLIDE_LIMIT );
}
