/***
*
*	Copyright (c) 1999, Valve LLC. All rights reserved.
*	
*	This product contains software technology licensed from Id 
*	Software, Inc. ("Id Technology").  Id Technology (c) 1996 Id Software, Inc. 
*	All Rights Reserved.
*
*   Use, distribution, and modification of this source code and/or resulting
*   object code is restricted to non-commercial enhancements to products from
*   Valve LLC.  All other use, distribution, or modification is prohibited
*   without written permission from Valve LLC.
*
****/
//
// battery.cpp
//
// implementation of CHudBatteryOld class
//
#include "cbase.h"
#include "hud.h"
#include "hudelement.h"
#include "hud_macros.h"
#include "parsemsg.h"

#include "hud_bitmapnumericdisplay.h"

#include "iclientmode.h"

#include <vgui_controls/AnimationController.h>

#define INIT_BAT	-1

#include "ConVar.h"
extern ConVar hud_enableoldhud;

//-----------------------------------------------------------------------------
// Purpose: Displays suit power (armor) on hud
//-----------------------------------------------------------------------------
class CHudBatteryOld : public CHudBitmapNumericDisplay, public CHudElement
{
	DECLARE_CLASS_SIMPLE( CHudBatteryOld, CHudBitmapNumericDisplay );

public:
	CHudBatteryOld( const char *pElementName );
	void Init( void );
	void Reset( void );
	void VidInit( void );
	void OnThink( void );
	void MsgFunc_BatteryOld(const char *pszName,  int iSize, void *pbuf );
	
private:
	int		m_iBat;	
	int		m_iNewBat;
	float	m_fFade;
	int		m_iGhostBat;
};

DECLARE_HUDELEMENT( CHudBatteryOld );
DECLARE_HUD_MESSAGE( CHudBatteryOld, BatteryOld );

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CHudBatteryOld::CHudBatteryOld( const char *pElementName ) : BaseClass(NULL, "HudSuit2"), CHudElement( pElementName )
{
//	SetActive( hud_enableoldhud.GetBool() );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CHudBatteryOld::Init( void )
{
//	HOOK_MESSAGE(Battery);
	HOOK_MESSAGE_EX(Battery, BatteryOld);
	Reset();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CHudBatteryOld::Reset( void )
{
	m_iBat		= INIT_BAT;
	m_iNewBat   = 0;
	m_iGhostBat	= 0;
	m_fFade		= 0;

	SetLabelText(L"SUIT");
	SetDisplayValue(m_iBat);
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CHudBatteryOld::VidInit( void )
{
	Reset();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CHudBatteryOld::OnThink( void )
{
	if ( !hud_enableoldhud.GetBool() )
	{
		SetPaintEnabled( false );
		SetPaintBackgroundEnabled( false );
		return;
	}
	else
	{
		SetPaintEnabled(true);
		SetPaintBackgroundEnabled(true);
	}

	if ( m_iBat == m_iNewBat )
		return;

	if ( !m_iNewBat )
	{
	 	g_pClientMode->GetViewportAnimationController()->StartAnimationSequence("SuitPowerZero");
	}
	else if ( m_iNewBat < m_iBat )
	{
		// battery power has decreased, so play the damaged animation
		g_pClientMode->GetViewportAnimationController()->StartAnimationSequence("SuitDamageTaken");

		// play an extra animation if we're super low
		if ( m_iNewBat < 20 )
		{
			g_pClientMode->GetViewportAnimationController()->StartAnimationSequence("SuitArmorLow");
		}
	}
	else
	{
		// battery power has increased (if we had no previous armor, or if we just loaded the game, don't use alert state)
		if ( m_iBat == INIT_BAT || m_iBat == 0 || m_iNewBat >= 20)
		{
			g_pClientMode->GetViewportAnimationController()->StartAnimationSequence("SuitPowerIncreasedAbove20");
		}
		else
		{
			g_pClientMode->GetViewportAnimationController()->StartAnimationSequence("SuitPowerIncreasedBelow20");
		}
	}

	m_fFade = 200;
	m_iGhostBat = m_iNewBat;
	m_iBat = m_iNewBat;

	SetDisplayValue(m_iBat);
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CHudBatteryOld::MsgFunc_BatteryOld( const char *pszName,  int iSize, void *pbuf )
{
	BEGIN_READ( pbuf, iSize );
	m_iNewBat = READ_SHORT();
}
