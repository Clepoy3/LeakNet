//========= Copyright © 1996-2001, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================
#if !defined( HUD_SUITPOWER_H )
#define HUD_SUITPOWER_H
#ifdef _WIN32
#pragma once
#endif

#include "hudelement.h"
#include "hud_numericdisplay.h"
#include <vgui_controls/Panel.h>

//-----------------------------------------------------------------------------
// Purpose: Shows the sprint power bar
//-----------------------------------------------------------------------------
class CHudSuitPowerOld : public CHudElement, public vgui::Panel
{
	DECLARE_CLASS_SIMPLE( CHudSuitPowerOld, vgui::Panel );

public:
	CHudSuitPowerOld( const char *pElementName );
	virtual void Init( void );
	virtual void Reset( void );
	virtual void OnThink( void );

protected:
	virtual void PaintBackground( void );
	virtual void Paint();

private:
	CPanelAnimationVar( Color, m_AuxPowerColor, "AuxPowerColor", "255 0 0 255" );
	CPanelAnimationVar( int, m_iAuxPowerDisabledAlpha, "AuxPowerDisabledAlpha", "70" );

//	CPanelAnimationVar( Color, m_Color, "Color", "FgColor" );
	CPanelAnimationVar( int, m_flAlphaOverride, "Alpha", "255" ); // VXP

	CPanelAnimationVarAliasType( float, m_flBarInsetX, "BarInsetX", "8", "proportional_float" );
	CPanelAnimationVarAliasType( float, m_flBarInsetY, "BarInsetY", "8", "proportional_float" );
	CPanelAnimationVarAliasType( float, m_flBarWidth, "BarWidth", "80", "proportional_float" );
	CPanelAnimationVarAliasType( float, m_flBarHeight, "BarHeight", "10", "proportional_float" );
//	CPanelAnimationVarAliasType( float, m_flBarChunkWidth, "BarChunkWidth", "10", "proportional_float" );
//	CPanelAnimationVarAliasType( float, m_flBarChunkGap, "BarChunkGap", "2", "proportional_float" );

//	CPanelAnimationVar( vgui::HFont, m_hTextFont, "TextFont", "Default" );
//	CPanelAnimationVarAliasType( float, text_xpos, "text_xpos", "8", "proportional_float" );
//	CPanelAnimationVarAliasType( float, text_ypos, "text_ypos", "20", "proportional_float" );

	float m_flSuitPower;
	bool m_bSuitPowerLow;
};	

#endif // HUD_SUITPOWER_H
