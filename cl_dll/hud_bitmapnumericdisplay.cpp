//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#include "cbase.h"
#include "hud_bitmapnumericdisplay.h"
#include "iclientmode.h"

#include <Color.h>
#include <KeyValues.h>
#include <vgui/ISurface.h>
#include <vgui/ISystem.h>
#include <vgui/IVGui.h>

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

using namespace vgui;

//-----------------------------------------------------------------------------
// Purpose: constructor
//-----------------------------------------------------------------------------
CHudBitmapNumericDisplay::CHudBitmapNumericDisplay(vgui::Panel *parent, const char *name) : vgui::Panel(parent, name)
{
	vgui::Panel *pParent = g_pClientMode->GetViewport();
	SetParent( pParent );

	m_iValue = 0;
	m_iMaxValue = -1;
	m_bDisplayValue = true;
//	memset( m_pNumbers, 0, 10*sizeof(CHudTexture *) );
	memset( m_pNumbers, 0, 12*sizeof(CHudTexture *) );
	memset( m_pSmallNumbers, 0, 12*sizeof(CHudTexture *) );

	m_LabelText[0] = 0;
	m_pHealthLabel = NULL;
	m_pSuitLabel = NULL;
	m_pAmmoLabel = NULL;
	m_pProgressBar = NULL;

	m_iSecondaryValue = 0;
	m_bDisplaySecondaryValue = false;
}

//-----------------------------------------------------------------------------
// Purpose: data accessor
//-----------------------------------------------------------------------------
//void CHudBitmapNumericDisplay::SetDisplayValue(int value)
void CHudBitmapNumericDisplay::SetDisplayValue(int value, int maxammo)
{
	m_iValue = value;
	m_iMaxValue = maxammo;
}

//-----------------------------------------------------------------------------
// Purpose: data accessor
//-----------------------------------------------------------------------------
void CHudBitmapNumericDisplay::SetSecondaryValue(int value)
{
	m_iSecondaryValue = value;
}

//-----------------------------------------------------------------------------
// Purpose: data accessor
//-----------------------------------------------------------------------------
void CHudBitmapNumericDisplay::SetShouldDisplaySecondaryValue(bool state)
{
	m_bDisplaySecondaryValue = state;
}

//-----------------------------------------------------------------------------
// Purpose: data accessor
//-----------------------------------------------------------------------------
void CHudBitmapNumericDisplay::SetLabelText(const wchar_t *text)
{
/*	if ( wcscmp(text, L"HEALTH") == 0 )
	{
		Msg( "Drawing HEALTH\n" );

		Msg("m_pLabel before: %s, m_pHealthLabel: %s\n", (m_pLabel ? "Yes" : "No"), (m_pHealthLabel ? "Yes" : "No"));
		m_pLabel = m_pHealthLabel;
		Msg("m_pLabel after: %s\n", (m_pLabel ? "Yes" : "No"));
	}
	else if ( wcscmp(text, L"SUIT") == 0 )
	{
		Msg( "Drawing SUIT\n" );

		m_pLabel = m_pSuitLabel;
	}*/
	wcsncpy(m_LabelText, text, sizeof(m_LabelText) / sizeof(wchar_t));
	m_LabelText[(sizeof(m_LabelText) / sizeof(wchar_t)) - 1] = 0;
}


//-----------------------------------------------------------------------------
// Purpose: renders the vgui panel
//-----------------------------------------------------------------------------
void CHudBitmapNumericDisplay::Paint()
{
	if ( !PrepareTextures() || !PrepareSmallTextures() )
		return;

	float alpha = m_flAlphaOverride / 255;
	Color fgColor = GetFgColor();
	fgColor[3] *= alpha;
	SetFgColor( fgColor );

	if (m_bDisplayValue)
	{
		// VXP: Draw label
		if ( wcscmp(m_LabelText, L"HEALTH") == 0 )
		{
			PaintLabel(digit_xpos - 58, digit_ypos, m_pHealthLabel, GetFgColor());
		}
		else if ( wcscmp(m_LabelText, L"SUIT") == 0 )
		{
			PaintLabel(digit_xpos - 58, digit_ypos, m_pSuitLabel, GetFgColor());
		}
		else if ( wcscmp(m_LabelText, L"AMMO") == 0 )
		{
			PaintLabel(digit_xpos - 58, digit_ypos, m_pAmmoLabel, GetFgColor());
			PaintProgressBar(digit_xpos, digit2_ypos + digit2_ypos/2 + 2, m_iValue, GetFgColor());
		}

		// VXP: Draw some dummies
		PaintDummies(digit_xpos, digit_ypos + 14, GetFgColor());
	
		// draw our numbers
	//	surface()->DrawSetTextColor(GetFgColor());
		PaintNumbers(digit_xpos, digit_ypos + 14, m_iValue, GetFgColor());

		// draw the overbright blur
		for (float fl = m_flBlur; fl > 0.0f; fl -= 1.0f)
		{
			if (fl >= 1.0f)
			{
				PaintNumbers(digit_xpos, digit_ypos + 14, m_iValue, GetFgColor());
			}
			else
			{
				// draw a percentage of the last one
				Color col = GetFgColor();
				col[3] *= fl;
				PaintNumbers(digit_xpos, digit_ypos + 14, m_iValue, col);
			}
		}
	}

	// total ammo
	if (m_bDisplaySecondaryValue)
	{
		//fgColor = m_Ammo2Color;
		//fgColor[3] *= alpha;
	//	surface()->DrawSetTextColor(fgColor);
		PaintDummies(digit2_xpos, digit2_ypos + 14, fgColor, true);
		PaintNumbers(digit2_xpos, digit2_ypos + 14, m_iSecondaryValue, fgColor, true);
	}
}

//-----------------------------------------------------------------------------
// Purpose: data accessor
//-----------------------------------------------------------------------------
void CHudBitmapNumericDisplay::SetShouldDisplayValue(bool state)
{
	m_bDisplayValue = state;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CHudBitmapNumericDisplay::PaintBackground( void )
{
	int alpha = m_flAlphaOverride / 255;
	Color bgColor = GetBgColor();
//	bgColor[3] *= alpha;
	bgColor[3] = 0;
	SetBgColor( bgColor );

	BaseClass::PaintBackground();
}

#define DIGIT_NUMBER_DUMMY1 10
#define DIGIT_NUMBER_DUMMY2 11
#define DIGIT_TEXT_HEALTH 12

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
bool CHudBitmapNumericDisplay::PrepareTextures()
{
	if( !m_pNumbers[0] )
	{
		int i;
		char a[16];

		for( i=0;i<10;i++ )
		{
			sprintf( a, "number_%d", i );

			m_pNumbers[i] = gHUD.GetIcon( a );
		}

		// VXP: Additional textures (should I do this here?)
		m_pNumbers[DIGIT_NUMBER_DUMMY1] = gHUD.GetIcon( "number_dummy1" );
		m_pNumbers[DIGIT_NUMBER_DUMMY2] = gHUD.GetIcon( "number_dummy2" );

		if( !m_pNumbers[0] || !m_pNumbers[DIGIT_NUMBER_DUMMY1] || !m_pNumbers[DIGIT_NUMBER_DUMMY2] )
			return false;

		m_pHealthLabel = gHUD.GetIcon( "health_label" );
		m_pSuitLabel = gHUD.GetIcon( "suit_label" );
		m_pAmmoLabel = gHUD.GetIcon( "ammo_label" );

		if( !m_pHealthLabel || !m_pSuitLabel || !m_pAmmoLabel )
			return false;

		m_pProgressBar = gHUD.GetIcon( "progress_bar" );
		if ( !m_pProgressBar )
			return false;
	}

	return true;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
bool CHudBitmapNumericDisplay::PrepareSmallTextures()
{
	if( !m_pSmallNumbers[0] )
	{
		int i;
		char a[16];

		for( i=0;i<10;i++ )
		{
			sprintf( a, "number_%d_small", i );

			m_pSmallNumbers[i] = gHUD.GetIcon( a );
		}

		// VXP: Additional textures (should I do this here?)
		m_pSmallNumbers[DIGIT_NUMBER_DUMMY1] = gHUD.GetIcon( "number_dummy1_small" );
		m_pSmallNumbers[DIGIT_NUMBER_DUMMY2] = gHUD.GetIcon( "number_dummy2_small" );

		if( !m_pSmallNumbers[0] || !m_pSmallNumbers[DIGIT_NUMBER_DUMMY1] || !m_pSmallNumbers[DIGIT_NUMBER_DUMMY2] )
			return false;
	}

	return true;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CHudBitmapNumericDisplay::PaintLabel(int xpos, int ypos, CHudTexture *label, Color col )
{
//	Msg("Label is NULL? %s %s %s\n", ((label == NULL) ? "Yes" : "No"), ((m_pLabel == NULL) ? "Yes" : "No"), ((m_pHealthLabel == NULL) ? "Yes" : "No"));

	if ( !label )
		return;

//	Msg("Drawing health label\n");

//	float scale = ( digit_height / (float)label->Height());
	float scale = 1.0f;
	int width = label->Width() * scale;
	int height = label->Height() * scale;

	label->DrawSelf( xpos, ypos, width, height, col );
}

#define BAR_HEIGHT 28
#define BAR_INCREMENT 2

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CHudBitmapNumericDisplay::PaintProgressBar(int xpos, int ypos, int value, Color col )
{
//	float scale = ( digit_height / (float)label->Height());
	float scale = 1.0f;
	int width = m_pProgressBar->Width() * scale;
	int height = m_pProgressBar->Height() * scale;

	// DrawSelfCropped( int x, int y, int cropx, int cropy, int cropw, int croph, Color& clr )
//	int newHeight = height;
//	if ( value > 0 )
//		newHeight = value / height * 100; // 52 : 400 * 100

//	int newHeight = (BAR_HEIGHT * value) / 32768;
	int newHeight = (BAR_HEIGHT * value) / m_iMaxValue;
        //newHeight = ((newHeight + (BAR_INCREMENT-1)) / BAR_INCREMENT) * BAR_INCREMENT;  // round to nearest BAR_INCREMENT
		newHeight = ((newHeight + (BAR_INCREMENT-1)) / BAR_INCREMENT) * BAR_INCREMENT;  // round to nearest BAR_INCREMENT

//	Msg("height: %i, value: %i, m_iSecondaryValue: %i; newHeight: %i\n", height, value, m_iSecondaryValue, newHeight);
//	m_pProgressBar->DrawSelfCropped( xpos, ypos, 0, newHeight, width, height, col );
	m_pProgressBar->DrawSelf( xpos, (ypos - newHeight) + height + 1, width, height, col );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CHudBitmapNumericDisplay::PaintDummies(int xpos, int ypos, Color col, int numSigDigits, bool isSmallFont )
{
	float downScale = 1.12f;

	CHudTexture *numberArray[12];
	memset( numberArray, 0, 12*sizeof(CHudTexture *) );
	if ( !isSmallFont )
	{
	//	memcpy(numberArray, m_pNumbers, 12*sizeof(CHudTexture *));
	}
	else
	{
		downScale = 0.7f;
	//	memcpy(numberArray, m_pSmallNumbers, 12*sizeof(CHudTexture *));
	}
	memcpy(numberArray, m_pNumbers, 12*sizeof(CHudTexture *));

	int pos = 10000;

	
	float scale = ( digit_height / (float)numberArray[DIGIT_NUMBER_DUMMY1]->Height()) * downScale;
	int width = numberArray[DIGIT_NUMBER_DUMMY1]->Width() * scale;
	int height = numberArray[DIGIT_NUMBER_DUMMY1]->Height() * scale;
	bool bStart = false;

	//right align to xpos

	int numdigits = 1;

/*	int x = pos;
	while( x >= 10 )
	{
		if( value >= x )
			numdigits++;

		x /= 10;
	}*/

	if( numdigits < numSigDigits )
		numdigits = numSigDigits;
//	Msg("numdigits: %i\n", numdigits);

	xpos -= numdigits * width;

	//draw the digits
	while( pos >= 1 )
	{
		bool isSecondPosition = false;
		if ( pos == 1 )
		{
			isSecondPosition = true;
		}
	//	Msg("pos: %i\n", pos);

		if( bStart || pos <= pow(10.0f,numSigDigits-1) )
		{
			bStart = true;
			// VXP: Make this for another dummy
		//	Msg("%i, %i, %i, %i\n", pos, xpos, width, numdigits);
		//	if ( pos == 10 ) // VXP: Shi~
		//	if ( isSecondPosition )
		//	{
		//		m_pNumbers[DIGIT_NUMBER_DUMMY2]->DrawSelf( xpos, ypos, width, height, col );
		//	}
		//	else
		//	{
				numberArray[DIGIT_NUMBER_DUMMY1]->DrawSelf( xpos, ypos, width, height, col );
		//	}
			xpos += width;
		}

		pos /= 10;
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CHudBitmapNumericDisplay::PaintNumbers(int xpos, int ypos, int value, Color col, int numSigDigits, bool isSmallFont )
{
	float downScale = 1.12f;

	CHudTexture *numberArray[12];
	memset( numberArray, 0, 12*sizeof(CHudTexture *) );
	if ( !isSmallFont )
	{
	//	memcpy(numberArray, m_pNumbers, 12*sizeof(CHudTexture *));
	}
	else
	{
		downScale = 0.7f;
	//	memcpy(numberArray, m_pSmallNumbers, 12*sizeof(CHudTexture *));
	}
	memcpy(numberArray, m_pNumbers, 12*sizeof(CHudTexture *));

	if( value > 100000 )
	{
		value = 99999;
	}

	int pos = 10000;

	float scale = ( digit_height / (float)numberArray[0]->Height()) * downScale;

	int digit;
	Color color = GetFgColor();
	int width = numberArray[0]->Width() * scale;
	int height = numberArray[0]->Height() * scale;
	bool bStart = false;

	//right align to xpos

	int numdigits = 1;

	int x = pos;
	while( x >= 10 )
	{
		if( value >= x )
			numdigits++;

		x /= 10;
	}

	if( numdigits < numSigDigits )
		numdigits = numSigDigits;

	xpos -= numdigits * width;

//	Msg( "digit_xpos: %i, digit_ypos: %i, digit_height: %i\n", digit_xpos, digit_ypos, digit_height );

	//draw the digits
	while( pos >= 1 )
	{
		digit = value / pos;
		value = value % pos;
		
		if( bStart || digit > 0 || pos <= pow(10.0f,numSigDigits-1) )
		{
			bStart = true;
			numberArray[digit]->DrawSelf( xpos, ypos, width, height, col );
		//	Msg("digit: %i, value: %i, xpos: %i, ypos: %i, width: %i, height: %i\n", digit, value, xpos, ypos, width, height);
			xpos += width;
		}		

		pos /= 10;
	}
}