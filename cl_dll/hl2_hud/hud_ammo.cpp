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

#if !defined( HL2_CLIENT_DLL )
#include "hud_numericdisplay.h"
#else
#include "hud_bitmapnumericdisplay.h"
#endif // HL2_DLL

#include "iclientmode.h"

#include <vgui_controls/AnimationController.h>

//-----------------------------------------------------------------------------
// Purpose: Displays current ammunition level
//-----------------------------------------------------------------------------
#if !defined( HL2_CLIENT_DLL )
class CHudAmmo : public CHudNumericDisplay, public CHudElement
#else
class CHudAmmo : public CHudBitmapNumericDisplay, public CHudElement
#endif // HL2_DLL
{
#if !defined( HL2_CLIENT_DLL )
	DECLARE_CLASS_SIMPLE( CHudAmmo, CHudNumericDisplay );
#else
	DECLARE_CLASS_SIMPLE( CHudAmmo, CHudBitmapNumericDisplay );
#endif // HL2_DLL

public:
	CHudAmmo( const char *pElementName );
	void Init( void );
	void VidInit( void );

#if !defined( HL2_CLIENT_DLL )
	void SetAmmo(int ammo, bool playAnimation);
#else
	void SetAmmo(int ammo, int maxammo, bool playAnimation);
	void SetAmmo2(int ammo2, bool playAnimation);
#endif // HL2_DLL
	
		
protected:
	virtual void OnThink();
	
private:
	CHandle< C_BaseCombatWeapon > m_hCurrentActiveWeapon;
	int		m_iAmmo;
	int		m_iAmmo2;
};

DECLARE_HUDELEMENT( CHudAmmo );

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
#if !defined( HL2_CLIENT_DLL )
CHudAmmo::CHudAmmo( const char *pElementName ) : BaseClass(NULL, "HudAmmo"), CHudElement( pElementName )
#else
CHudAmmo::CHudAmmo( const char *pElementName ) : BaseClass(NULL, "HudAmmo2"), CHudElement( pElementName )
#endif // HL2_DLL

{
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CHudAmmo::Init( void )
{
	m_iAmmo		= -1;
	m_iAmmo2	= -1;

	SetLabelText(L"AMMO");
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CHudAmmo::VidInit( void )
{
}

//-----------------------------------------------------------------------------
// Purpose: called every frame to get ammo info from the weapon
//-----------------------------------------------------------------------------
void CHudAmmo::OnThink()
{
	C_BaseCombatWeapon *wpn = GetActiveWeapon();
	C_BasePlayer *player = C_BasePlayer::GetLocalPlayer();
	if (!wpn || !player || !wpn->UsesPrimaryAmmo())
	{
		SetPaintEnabled(false);
		SetPaintBackgroundEnabled(false);
		return;
	}
	else
	{
		SetPaintEnabled(true);
		SetPaintBackgroundEnabled(true);
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
#if !defined( HL2_CLIENT_DLL )
		SetAmmo(ammo1, true);
#else
		SetAmmo(ammo1, wpn->GetMaxClip1(), true);
		SetAmmo2(ammo2, true);
#endif // HL2_DLL
	}
	else
	{
		// diferent weapon, change without triggering
#if !defined( HL2_CLIENT_DLL )
		SetAmmo(ammo1, false);
#else
		SetAmmo(ammo1, wpn->GetMaxClip1(), false);
		SetAmmo2(ammo2, false);
#endif // HL2_DLL

		// update whether or not we show the total ammo display
		if (wpn->UsesClipsForAmmo1())
		{
			SetShouldDisplaySecondaryValue(true);
			g_pClientMode->GetViewportAnimationController()->StartAnimationSequence("WeaponUsesClips");

		}
		else
		{
			g_pClientMode->GetViewportAnimationController()->StartAnimationSequence("WeaponDoesNotUseClips");
			SetShouldDisplaySecondaryValue(false);
		}

		if ( GetGameRestored() )
		{
			SetGameRestored( false );
			g_pClientMode->GetViewportAnimationController()->StartAnimationSequence("WeaponChangedRestore");
		}
		else
		{
			g_pClientMode->GetViewportAnimationController()->StartAnimationSequence("WeaponChanged");
		}
		m_hCurrentActiveWeapon = wpn;
	}
}

//-----------------------------------------------------------------------------
// Purpose: Updates ammo display
//-----------------------------------------------------------------------------
#if !defined( HL2_CLIENT_DLL )
void CHudAmmo::SetAmmo(int ammo, bool playAnimation)
#else
void CHudAmmo::SetAmmo(int ammo, int maxammo, bool playAnimation)
#endif // HL2_DLL
{
	if (ammo != m_iAmmo)
	{
		if (ammo == 0)
		{
			g_pClientMode->GetViewportAnimationController()->StartAnimationSequence("AmmoEmpty");
		}
		else if (ammo < m_iAmmo)
		{
			// ammo has decreased
			g_pClientMode->GetViewportAnimationController()->StartAnimationSequence("AmmoDecreased");
		}
		else
		{
			// ammunition has increased
			g_pClientMode->GetViewportAnimationController()->StartAnimationSequence("AmmoIncreased");
		}

		m_iAmmo = ammo;
	}

#if !defined( HL2_CLIENT_DLL )
	SetDisplayValue(ammo);
#else
	SetDisplayValue(ammo, maxammo);
#endif // HL2_DLL
	
}

#if defined( HL2_CLIENT_DLL )
//-----------------------------------------------------------------------------
// Purpose: Updates 2nd ammo display
//-----------------------------------------------------------------------------
void CHudAmmo::SetAmmo2(int ammo2, bool playAnimation)
{
	if (ammo2 != m_iAmmo2)
	{
		if (ammo2 == 0)
		{
			g_pClientMode->GetViewportAnimationController()->StartAnimationSequence("Ammo2Empty");
		}
		else if (ammo2 < m_iAmmo2)
		{
			// ammo has decreased
			g_pClientMode->GetViewportAnimationController()->StartAnimationSequence("Ammo2Decreased");
		}
		else
		{
			// ammunition has increased
			g_pClientMode->GetViewportAnimationController()->StartAnimationSequence("Ammo2Increased");
		}

		m_iAmmo2 = ammo2;
	}

	SetSecondaryValue(ammo2);
}
#endif // HL2_DLL

//-----------------------------------------------------------------------------
// Purpose: Displays the secondary ammunition level
//-----------------------------------------------------------------------------
#if !defined( HL2_CLIENT_DLL )
class CHudSecondaryAmmo : public CHudNumericDisplay, public CHudElement
#else
class CHudSecondaryAmmo : public CHudBitmapNumericDisplay, public CHudElement
#endif // HL2_DLL

{
#if !defined( HL2_CLIENT_DLL )
	DECLARE_CLASS_SIMPLE( CHudSecondaryAmmo, CHudNumericDisplay );
#else
	DECLARE_CLASS_SIMPLE( CHudSecondaryAmmo, CHudBitmapNumericDisplay );
#endif // HL2_DLL
	

public:
#if !defined( HL2_CLIENT_DLL )
	CHudSecondaryAmmo( const char *pElementName ) : BaseClass( NULL, "HudAmmoSecondary" ), CHudElement( pElementName )
#else
	CHudSecondaryAmmo( const char *pElementName ) : BaseClass( NULL, "HudAmmoSecondary2" ), CHudElement( pElementName )
#endif // HL2_DLL
	
	{
		m_iAmmo = -1;

#if defined( HL2_CLIENT_DLL )
		SetLabelText(L"AMMO2");
#endif // HL2_DLL
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
				g_pClientMode->GetViewportAnimationController()->StartAnimationSequence("AmmoSecondaryEmpty");
			}
			else if (ammo < m_iAmmo)
			{
				// ammo has decreased
				g_pClientMode->GetViewportAnimationController()->StartAnimationSequence("AmmoSecondaryDecreased");
			}
			else
			{
				// ammunition has increased
				g_pClientMode->GetViewportAnimationController()->StartAnimationSequence("AmmoSecondaryIncreased");
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
		if (!wpn || !player)
		{
			m_hCurrentActiveWeapon = NULL;
			SetPaintEnabled(false);
			SetPaintBackgroundEnabled(false);
			return;
		}
		else
		{
			SetPaintEnabled(true);
			SetPaintBackgroundEnabled(true);
		}

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
					"WeaponUsesSecondaryAmmoRestore" : 
					"WeaponUsesSecondaryAmmo");
			}
			else 
			{
				// we've changed away from a weapon that uses secondary ammo
				g_pClientMode->GetViewportAnimationController()->StartAnimationSequence(
					restored ?
					"WeaponDoesNotUseSecondaryAmmoRestore" :
					"WeaponDoesNotUseSecondaryAmmo" );
			}
			m_hCurrentActiveWeapon = wpn;
		}
	}
	
private:
	CHandle< C_BaseCombatWeapon > m_hCurrentActiveWeapon;
	int		m_iAmmo;
};

DECLARE_HUDELEMENT( CHudSecondaryAmmo );

