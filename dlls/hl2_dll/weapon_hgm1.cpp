//========= Copyright © 1996-2003, Valve LLC, All rights reserved. ============
//
// Purpose: Heavy machine gun (currently an AK-47).
//
//=============================================================================

#include "cbase.h"
#include "basehlcombatweapon.h"
#include "NPCevent.h"
#include "basecombatcharacter.h"
#include "AI_BaseNPC.h"

class CWeaponHMG1 : public CHLSelectFireMachineGun
{
public:
	DECLARE_CLASS( CWeaponHMG1, CHLSelectFireMachineGun );

	DECLARE_SERVERCLASS();

	CWeaponHMG1();
	
	void	Precache( void );
	bool	Deploy( void );
	
	void	AddViewKick( void ); // VXP

	int CapabilitiesGet( void ) { return bits_CAP_WEAPON_RANGE_ATTACK1; }

	virtual const Vector& GetBulletSpread( void )
	{
	//	static Vector cone = VECTOR_CONE_15DEGREES;
		static const Vector cone = VECTOR_CONE_15DEGREES;
		return cone;
	}

	void Operator_HandleAnimEvent( animevent_t *pEvent, CBaseCombatCharacter *pOperator )
	{
		switch( pEvent->event )
		{
			case EVENT_WEAPON_HMG1:
			{
				Vector vecShootOrigin, vecShootDir;
				vecShootOrigin = pOperator->Weapon_ShootPosition( );

				CAI_BaseNPC *npc = pOperator->MyNPCPointer();
				Vector vecSpread;
				if (npc)
				{
					vecShootDir = npc->GetActualShootTrajectory( vecShootOrigin );
					vecSpread = VECTOR_CONE_PRECALCULATED;
				}
				else
				{
					AngleVectors( pOperator->GetLocalAngles(), &vecShootDir );
					vecSpread = GetBulletSpread();
				}
				WeaponSound(SINGLE_NPC);
				pOperator->FireBullets( 1, vecShootOrigin, vecShootDir, vecSpread, MAX_TRACE_LENGTH, m_iPrimaryAmmoType, 2 );
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

IMPLEMENT_SERVERCLASS_ST(CWeaponHMG1, DT_WeaponHMG1)
END_SEND_TABLE()

LINK_ENTITY_TO_CLASS( weapon_hmg1, CWeaponHMG1 );

acttable_t	CWeaponHMG1::m_acttable[] = 
{
	{ ACT_RANGE_ATTACK1, ACT_RANGE_ATTACK_HMG1, true },
};

IMPLEMENT_ACTTABLE(CWeaponHMG1);

//=========================================================
CWeaponHMG1::CWeaponHMG1( )
{
}

void CWeaponHMG1::Precache( void )
{
	BaseClass::Precache();
}

bool CWeaponHMG1::Deploy( void )
{
	//CBaseCombatCharacter *pOwner  = m_hOwner;
	return BaseClass::Deploy();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CWeaponHMG1::AddViewKick( void )
{
	#define	EASY_DAMPEN			0.5f
	#define	MAX_VERTICAL_KICK	2.0f	//Degrees
	#define	SLIDE_LIMIT			1.0f	//Seconds
	
	//Get the view kick
	CBasePlayer *pPlayer = ToBasePlayer( GetOwner() );

	if (!pPlayer)
		return;

	

	QAngle viewPunch;
	viewPunch.x = SHARED_RANDOMFLOAT( -0.5f, 0.5f );
	viewPunch.y = SHARED_RANDOMFLOAT( -1.0f, 1.0f );
	viewPunch.z = 0;

	if ( pPlayer->GetFlags() & FL_DUCKING )
	{
		viewPunch *= 0.25;
	}

	pPlayer->ViewPunch( viewPunch );
	DoMachineGunKick( pPlayer, EASY_DAMPEN, MAX_VERTICAL_KICK, m_fFireDuration, SLIDE_LIMIT );
}
