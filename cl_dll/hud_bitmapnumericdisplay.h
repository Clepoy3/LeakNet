//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#ifndef HUD_BITMAPNUMERICDISPLAY_H
#define HUD_BITMAPNUMERICDISPLAY_H
#ifdef _WIN32
#pragma once
#endif

#include "hud_numericdisplay.h"

#define NUMBERS_COUNT 12

class CHudBitmapNumericDisplay : public vgui::Panel
{	
	DECLARE_CLASS_SIMPLE( CHudBitmapNumericDisplay, vgui::Panel );

public:
	CHudBitmapNumericDisplay(vgui::Panel *parent, const char *name);

	void SetDisplayValue(int value, int maxammo); // VXP: Special
	void SetDisplayValue(int value)
	{
		SetDisplayValue(value, -1);
	}
	
	void SetSecondaryValue(int value); // VXP
	void SetShouldDisplayValue(bool state);
	void SetShouldDisplaySecondaryValue(bool state); // VXP
	void SetLabelText(const wchar_t *text); // VXP

protected:
	// vgui overrides
	virtual void PaintBackground( void );
	virtual void Paint();
	bool PrepareTextures(); // VXP
	bool PrepareSmallTextures(); // VXP
	void PaintNumbers(int xpos, int ypos, int value, Color col, int numSigDigits, bool isSmallFont = false);
	virtual void PaintNumbers(int xpos, int ypos, int value, Color col, bool isSmallFont = false)
	{
		PaintNumbers(xpos, ypos, value, col, 1, isSmallFont);
	}

	// VXP
	void PaintLabel(int xpos, int ypos, CHudTexture *label, Color col);
	void PaintProgressBar(int xpos, int ypos, int value, Color col);
	void PaintDummies(int xpos, int ypos, int value, Color col, int numSigDigits, bool isSmallFont = false);
	virtual void PaintDummies(int xpos, int ypos, Color col, bool isSmallFont = false)
	{
		PaintDummies(xpos, ypos, 999, col, 3, isSmallFont);
	}

	CPanelAnimationVar( float, m_flAlphaOverride, "Alpha", "255" );
	CPanelAnimationVar( Color, m_TextColor, "TextColor", "FgColor" );
	CPanelAnimationVar( float, m_flBlur, "Blur", "0" );

	CPanelAnimationVar( Color, m_Color, "Color", "FgColor" );

	CPanelAnimationVarAliasType( float, text_xpos, "text_xpos", "8", "proportional_float" );
	CPanelAnimationVarAliasType( float, text_ypos, "text_ypos", "20", "proportional_float" );
	CPanelAnimationVarAliasType( float, text_height, "text_height", "14", "proportional_float" );
	CPanelAnimationVarAliasType( float, digit_xpos, "digit_xpos", "0", "proportional_float" );
	CPanelAnimationVarAliasType( float, digit_ypos, "digit_ypos", "0", "proportional_float" );
	CPanelAnimationVarAliasType( float, digit_height, "digit_height", "16", "proportional_float" );
//	CPanelAnimationVarAliasType( float, digit_xpos, "digit_xpos", "50", "proportional_float" );
//	CPanelAnimationVarAliasType( float, digit_ypos, "digit_ypos", "2", "proportional_float" );
//	CPanelAnimationVarAliasType( float, digit_height, "digit_height", "25", "proportional_float" );
	CPanelAnimationVarAliasType( float, digit2_xpos, "digit2_xpos", "0", "proportional_float" );
	CPanelAnimationVarAliasType( float, digit2_ypos, "digit2_ypos", "0", "proportional_float" );
	CPanelAnimationVarAliasType( float, digit2_height, "digit2_height", "8", "proportional_float" );

private:

//	CHudTexture *m_pNumbers[10];
	CHudTexture *m_pNumbers[NUMBERS_COUNT];
	CHudTexture *m_pSmallNumbers[NUMBERS_COUNT];

	int m_iValue;
	int m_iMaxValue;
	int m_iSecondaryValue;
	bool m_bDisplayValue, m_bDisplaySecondaryValue;


	// VXP

	wchar_t m_LabelText[32];

	CHudTexture *m_pHealthLabel;
	CHudTexture *m_pSuitLabel;
	CHudTexture *m_pAmmoLabel;
	CHudTexture *m_pProgressBar;
};

#endif //HUD_BITMAPNUMERICDISPLAY_H