//========= Copyright © 1996-2003, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
//=============================================================================

#include "cbase.h"
#include "hud.h"
#include "hudelement.h"
#include "hud_macros.h"
#include "parsemsg.h"

#include "hud_bitmapnumericdisplay.h"

#include "iclientmode.h"

#include <vgui_controls/AnimationController.h>

#if defined( HL2_CLIENT_DLL )
#include "ConVar.h"
extern ConVar hud_enableoldhud;
#endif

//-----------------------------------------------------------------------------
// Purpose: Displays current ammunition level
//-----------------------------------------------------------------------------
class CHudAmmoOld : public CHudBitmapNumericDisplay, public CHudElement
{
	DECLARE_CLASS_SIMPLE( CHudAmmoOld, CHudBitmapNumericDisplay );

public:
	CHudAmmoOld( const char *pElementName );
	void Init( void );
	void VidInit( void );

	void SetAmmo(int ammo, int maxammo, bool playAnimation);
	void SetAmmo2(int ammo2, bool playAnimation);
	
		
protected:
	virtual void OnThink();
	
private:
	CHandle< C_BaseCombatWeapon > m_hCurrentActiveWeapon;
	int		m_iAmmo;
	int		m_iAmmo2;
};

DECLARE_HUDELEMENT( CHudAmmoOld );

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CHudAmmoOld::CHudAmmoOld( const char *pElementName ) : BaseClass(NULL, "HudAmmo2"), CHudElement( pElementName )
{
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CHudAmmoOld::Init( void )
{
	m_iAmmo		= -1;
	m_iAmmo2	= -1;

	SetLabelText(L"AMMO");
	SetPaintBackgroundEnabled( false );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CHudAmmoOld::VidInit( void )
{
}

//-----------------------------------------------------------------------------
// Purpose: called every frame to get ammo info from the weapon
//-----------------------------------------------------------------------------
void CHudAmmoOld::OnThink()
{
	C_BaseCombatWeapon *wpn = GetActiveWeapon();
	C_BasePlayer *player = C_BasePlayer::GetLocalPlayer();
	if (!wpn || !player || !wpn->UsesPrimaryAmmo() 
#if defined( HL2_CLIENT_DLL )
		|| !hud_enableoldhud.GetBool()
#endif
		)
	{
		SetPaintEnabled( false );
	//	SetPaintBackgroundEnabled( false );
		return;
	}
	else
	{
		SetPaintEnabled( true );
	//	SetPaintBackgroundEnabled( true );
	}

	// get the ammo in our clip
	int ammo1 = wpn->Clip1();
	int ammo2;
	if (ammo1 < 0)
	{
		// we don't use clip ammo, just use the total ammo count
		ammo1 = player->GetAmmoCount(wpn->GetPrimaryAmmoType());
		ammo2 = 0;
	}
	else
	{
		// we use clip ammo, so the second ammo is the total ammo
		ammo2 = player->GetAmmoCount(wpn->GetPrimaryAmmoType());
	}

	if (wpn == m_hCurrentActiveWeapon)
	{
		// same weapon, just update counts
		SetAmmo(ammo1, wpn->GetMaxClip1(), true);
		SetAmmo2(ammo2, true);
	}
	else
	{
		// diferent weapon, change without triggering
		SetAmmo(ammo1, wpn->GetMaxClip1(), false);
		SetAmmo2(ammo2, false);

		// update whether or not we show the total ammo display
		if (wpn->UsesClipsForAmmo1())
		{
			SetShouldDisplaySecondaryValue(true);
			g_pClientMode->GetViewportAnimationController()->StartAnimationSequence("WeaponUsesClipsOld");

		}
		else
		{
			g_pClientMode->GetViewportAnimationController()->StartAnimationSequence("WeaponDoesNotUseClipsOld");
			SetShouldDisplaySecondaryValue(false);
		}

		if ( GetGameRestored() )
		{
			SetGameRestored( false );
			g_pClientMode->GetViewportAnimationController()->StartAnimationSequence("WeaponChangedRestoreOld");
		}
		else
		{
			g_pClientMode->GetViewportAnimationController()->StartAnimationSequence("WeaponChangedOld");
		}
		m_hCurrentActiveWeapon = wpn;
	}
}

//-----------------------------------------------------------------------------
// Purpose: Updates ammo display
//-----------------------------------------------------------------------------
void CHudAmmoOld::SetAmmo(int ammo, int maxammo, bool playAnimation)
{
	if (ammo != m_iAmmo)
	{
		if (ammo == 0)
		{
			g_pClientMode->GetViewportAnimationController()->StartAnimationSequence("AmmoEmptyOld");
		}
		else if (ammo < m_iAmmo)
		{
			// ammo has decreased
			g_pClientMode->GetViewportAnimationController()->StartAnimationSequence("AmmoDecreasedOld");
		}
		else
		{
			// ammunition has increased
			g_pClientMode->GetViewportAnimationController()->StartAnimationSequence("AmmoIncreasedOld");
		}

		m_iAmmo = ammo;
	}

	SetDisplayValue(ammo, maxammo);
	
}

//-----------------------------------------------------------------------------
// Purpose: Updates 2nd ammo display
//-----------------------------------------------------------------------------
void CHudAmmoOld::SetAmmo2(int ammo2, bool playAnimation)
{
	if (ammo2 != m_iAmmo2)
	{
		if (ammo2 == 0)
		{
			g_pClientMode->GetViewportAnimationController()->StartAnimationSequence("Ammo2EmptyOld");
		}
		else if (ammo2 < m_iAmmo2)
		{
			// ammo has decreased
			g_pClientMode->GetViewportAnimationController()->StartAnimationSequence("Ammo2DecreasedOld");
		}
		else
		{
			// ammunition has increased
			g_pClientMode->GetViewportAnimationController()->StartAnimationSequence("Ammo2IncreasedOld");
		}

		m_iAmmo2 = ammo2;
	}

	SetSecondaryValue(ammo2);
}

//-----------------------------------------------------------------------------
// Purpose: Displays the secondary ammunition level
//-----------------------------------------------------------------------------
class CHudSecondaryAmmoOld : public CHudBitmapNumericDisplay, public CHudElement
{
	DECLARE_CLASS_SIMPLE( CHudSecondaryAmmoOld, CHudBitmapNumericDisplay );

public:
	CHudSecondaryAmmoOld( const char *pElementName ) : BaseClass( NULL, "HudAmmoSecondary2" ), CHudElement( pElementName )	
	{
		m_iAmmo = -1;

		SetLabelText(L"AMMO2");
	}

	void Init( void )
	{
	}

	void VidInit( void )
	{
	}

	void SetAmmo( int ammo )
	{
		if (ammo != m_iAmmo)
		{
			if (ammo == 0)
			{
				g_pClientMode->GetViewportAnimationController()->StartAnimationSequence("AmmoSecondaryEmptyOld");
			}
			else if (ammo < m_iAmmo)
			{
				// ammo has decreased
				g_pClientMode->GetViewportAnimationController()->StartAnimationSequence("AmmoSecondaryDecreasedOld");
			}
			else
			{
				// ammunition has increased
				g_pClientMode->GetViewportAnimationController()->StartAnimationSequence("AmmoSecondaryIncreasedOld");
			}

			m_iAmmo = ammo;
		}
		SetDisplayValue( ammo );
	}

protected:
	virtual void OnThink( void )
	{
		// set whether or not the panel draws based on if we have a weapon that supports secondary ammo
		C_BaseCombatWeapon *wpn = GetActiveWeapon();
		C_BasePlayer *player = C_BasePlayer::GetLocalPlayer();
		if (!wpn || !player || 
#if defined( HL2_CLIENT_DLL )
			!hud_enableoldhud.GetBool()
#endif
			)
		{
			m_hCurrentActiveWeapon = NULL;
			SetPaintEnabled( false );
			SetPaintBackgroundEnabled( false );
			return;
		}
		else
		{
			SetPaintEnabled( true );
			SetPaintBackgroundEnabled( true );
		}
		
	//	SetShouldDisplaySecondaryValue(true); // VXP: Test

		if (wpn->UsesSecondaryAmmo())
		{
			SetAmmo(player->GetAmmoCount(wpn->GetSecondaryAmmoType()));
		}

		if ( m_hCurrentActiveWeapon != wpn )
		{
			bool restored = false;
			if ( GetGameRestored() )
			{
				SetGameRestored( false );
				restored = true;
			}

			if ( wpn->UsesSecondaryAmmo() )
			{
				// we've changed to a weapon that uses secondary ammo
				g_pClientMode->GetViewportAnimationController()->StartAnimationSequence(
					restored ?
					"WeaponUsesSecondaryAmmoRestoreOld" : 
					"WeaponUsesSecondaryAmmoOld");
			}
			else 
			{
				// we've changed away from a weapon that uses secondary ammo
				g_pClientMode->GetViewportAnimationController()->StartAnimationSequence(
					restored ?
					"WeaponDoesNotUseSecondaryAmmoRestoreOld" :
					"WeaponDoesNotUseSecondaryAmmoOld" );
			}
			m_hCurrentActiveWeapon = wpn;
		}
	}
	
private:
	CHandle< C_BaseCombatWeapon > m_hCurrentActiveWeapon;
	int		m_iAmmo;
};

DECLARE_HUDELEMENT( CHudSecondaryAmmoOld );

