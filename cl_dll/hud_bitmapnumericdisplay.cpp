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
	memset( m_pNumbers, 0, NUMBERS_COUNT * sizeof(CHudTexture *) );
	memset( m_pSmallNumbers, 0, NUMBERS_COUNT * sizeof(CHudTexture *) );

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

//	SetFgColor( m_Color );
//	SetFgColor( GetFgColor() );

	// VXP: Adding extra alpha, if override is present
	/*
	float alpha = m_flAlphaOverride / 255;
//	Color fgColor = GetFgColor();
	Color fgColor = m_Color;
	fgColor[3] *= alpha;

	// VXP: Test values
//	fgColor[0] = 0.0f;
//	fgColor[1] = 255.0f;
//	fgColor[2] = 0.0f;

//	fgColor[0] = 177.0f;
//	fgColor[1] = 150.0f;
//	fgColor[2] = 70.0f;
//	fgColor[3] = 255.0f;

	SetFgColor( fgColor );
	*/

//	int x = 0, y = 0;
//	GetPos(x, y);
//	Msg("Value: %i at %i %i\n", m_iValue, x, y);

	if (m_bDisplayValue)
	{
		bool isAmmoHudPrimary = false;
		bool isAmmoHudSecondary = false;

		// VXP: Draw label
		if ( wcscmp(m_LabelText, L"HEALTH") == 0 )
		{
			PaintLabel(text_xpos, text_ypos, m_pHealthLabel, GetFgColor());
		}
		else if ( wcscmp(m_LabelText, L"SUIT") == 0 )
		{
			PaintLabel(text_xpos, text_ypos, m_pSuitLabel, GetFgColor());
		}
		else if ( wcscmp(m_LabelText, L"AMMO") == 0 )
		{
			isAmmoHudPrimary = true;
		//	PaintLabel(((m_iValue < 100) ? (text_xpos + 19) : text_xpos), text_ypos, m_pAmmoLabel, GetFgColor());
		//	PaintLabel(((m_iValue < 100) ? (text_xpos + m_pNumbers[0]->Width()) : text_xpos), text_ypos, m_pAmmoLabel, GetFgColor());
		//	PaintLabel(((m_iValue < 100) ? (text_xpos + (float)XRES((float)m_pNumbers[0]->Width())) : text_xpos), text_ypos, m_pAmmoLabel, GetFgColor());
			if ( m_iValue < 100 )
			{
			//	PaintLabel(text_xpos + (float)XRES((float)m_pNumbers[0]->Width()), text_ypos, m_pAmmoLabel, GetFgColor());
			//	PaintLabel(text_xpos + GetProportionalNormalizedValue(m_pNumbers[0]->Width()), text_ypos, m_pAmmoLabel, GetFgColor());
			//	PaintLabel(text_xpos + scheme()->GetProportionalScaledValue(m_pNumbers[0]->Width()), text_ypos, m_pAmmoLabel, GetFgColor());
			//	PaintLabel(scheme()->GetProportionalScaledValue(m_pNumbers[0]->Width()) / 2, text_ypos, m_pAmmoLabel, GetFgColor());
				PaintLabel(XRES(m_pNumbers[0]->Width()) / 2, text_ypos, m_pAmmoLabel, GetFgColor());
			}
			else
			{
				PaintLabel(text_xpos, text_ypos, m_pAmmoLabel, GetFgColor());
			}
		//	PaintProgressBar(digit_xpos, digit_ypos, m_iValue, GetFgColor());
		//	PaintProgressBar(digit_xpos, text_ypos + text_height, m_iValue, GetFgColor());
		//	PaintProgressBar(digit_xpos, digit2_ypos - text_height/2, m_iValue, GetFgColor()); // VXP: Good from 800x600 to <1920x1080
			PaintProgressBar(digit_xpos, digit_ypos, m_iValue, GetFgColor());
		}
		else if ( wcscmp(m_LabelText, L"AMMO2") == 0 )
		{
			isAmmoHudSecondary = true;
		}

		// VXP: Draw some dummies
	//	PaintDummies(digit_xpos, digit_ypos, GetFgColor(), (((isAmmoHudPrimary && m_iValue < 100) || isAmmoHudSecondary) ? 2 : 3));
		if ( (isAmmoHudPrimary && m_iValue < 100) || isAmmoHudSecondary)
		{
			PaintDummies(digit_xpos, digit_ypos, 99, GetFgColor(), 2);
		}
		else
		{
		//	PaintDummies(digit_xpos, digit_ypos, 999, GetFgColor(), 3);

			Color col = GetFgColor();
			col[3] = 90;
			PaintDummies(digit_xpos, digit_ypos, 999, col, 3);
		}
	
		// draw our numbers
	//	surface()->DrawSetTextColor(GetFgColor());
		PaintNumbers(digit_xpos, digit_ypos, m_iValue, GetFgColor());

		// draw the overbright blur
		for (float fl = m_flBlur; fl > 0.0f; fl -= 1.0f)
		{
			if (fl >= 1.0f)
			{
				PaintNumbers(digit_xpos, digit_ypos, m_iValue, GetFgColor());
			}
			else
			{
				// draw a percentage of the last one
				Color col = GetFgColor();
			//	col[3] *= fl;
				col[3] *= fl / 2; // VXP: Do I need to fix this at HudAnimations.txt's Blur entries?
				PaintNumbers(digit_xpos, digit_ypos, m_iValue, col);
			}
		}
	}

	// total ammo
	if (m_bDisplaySecondaryValue)
	{
		//fgColor = m_Ammo2Color;
		//fgColor[3] *= alpha;
	//	surface()->DrawSetTextColor(fgColor);
	//	PaintDummies(digit2_xpos, digit2_ypos, fgColor, true);
		PaintDummies(digit2_xpos, digit2_ypos, GetFgColor(), true);
		PaintNumbers(digit2_xpos, digit2_ypos, m_iSecondaryValue, GetFgColor(), true);
		// VXP: TODO: Remove this!!!
	//	PaintNumbers(digit2_xpos, digit2_ypos, 60, GetFgColor(), true);
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
/*	int alpha = m_flAlphaOverride / 255;
	Color bgColor = GetBgColor();
//	bgColor[3] *= alpha;
	bgColor[3] = 0;
	SetBgColor( bgColor );*/

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

		for ( i = 0; i < 10; i++ )
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
		m_pSuitLabel = gHUD.GetIcon( "battery_label" );
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

		for ( i = 0; i < 10; i++ )
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
void CHudBitmapNumericDisplay::PaintLabel(float xpos, float ypos, CHudTexture *label, Color col )
{
//	Msg("Label is NULL? %s %s %s\n", ((label == NULL) ? "Yes" : "No"), ((m_pLabel == NULL) ? "Yes" : "No"), ((m_pHealthLabel == NULL) ? "Yes" : "No"));

	if ( !label )
		return;

//	Msg("Drawing health label\n");

//	float downScale = 1.0f;

//	float scale = ( digit_height / (float)label->Height());
//	float scale = 1.0f;
//	float scale = (float)label->Height() * downScale;
//	float scale = ( digit_height / (float)label->Height()) * downScale;

	float width = text_width;
	float height = text_height;
//	float scale = ( (digit_height * 1.92f) / (float)label->Height()) * downScale;

	// VXP: Based on height
//	float width = text_height / ( (float)label->Height() / (float)label->Width());
//	float height = text_height;
//	int width = text_height / ( label->Height() / (float)label->Width());
//	int height = text_height;

	// VXP: Based on width
//	int width = text_width;
//	int height = text_width / ( label->Width() / (float)label->Height() );

//	float scale = ( text_height / (float)label->Height());
//	int width = label->Width() * scale;
//	int height = label->Height() * scale;

//	float width = XRES( 31.8f );
//	float width = XRES( 31.9f );
//	float width = XRES( 31.5f );
//	float height = YRES( 8.5f );

	// VXP: Almost good
//	float width = XRES( 29.0f );
//	float height = YRES( 6.0f );

//	float width = XRES( 31.0f );
//	float height = YRES( 8.0f );

//	int width = label->Width();
//	int height = label->Height();

//	Msg("width: %i, height: %i, digit_height: %i\n", label->Width(), label->Height(), digit_height);

	label->DrawSelf( xpos, ypos, width, height, col );
}

#define BAR_HEIGHT 26
#define BAR_INCREMENT 2

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CHudBitmapNumericDisplay::PaintProgressBar(float xpos, float ypos, int value, Color col )
{
//	float scale = ( digit_height / (float)m_pProgressBar->Height());
//	float scale = 1.0f;
//	int width = m_pProgressBar->Width() * scale;
//	int height = m_pProgressBar->Height() * scale;

	float width = (float)digit_height / ( (float)m_pProgressBar->Height() / (float)m_pProgressBar->Width());
	float height = (float)digit_height;

	// y2 = y1 + h1 / 2 - h2 / 2
//	ypos = ypos + digit_height/2 - height/2; // VXP: At least center this bar

	// DrawSelfCropped( int x, int y, int cropx, int cropy, int cropw, int croph, Color& clr )
//	int newHeight = height;
//	if ( value > 0 )
//		newHeight = value / height * 100; // 52 : 400 * 100

//	int newHeight = (BAR_HEIGHT * value) / 32768;
//	int newHeight = (BAR_HEIGHT * value) / m_iMaxValue;
//	int newHeight = (m_pProgressBar->Height() * value) / m_iMaxValue;
        //newHeight = ((newHeight + (BAR_INCREMENT-1)) / BAR_INCREMENT) * BAR_INCREMENT;  // round to nearest BAR_INCREMENT
//		newHeight = ((newHeight + (BAR_INCREMENT-1)) / BAR_INCREMENT) * BAR_INCREMENT;  // round to nearest BAR_INCREMENT

//	Msg("height: %i, value: %i, m_iSecondaryValue: %i; newHeight: %i\n", height, value, m_iSecondaryValue, newHeight);
//	m_pProgressBar->DrawSelfCropped( xpos, ypos, 0, newHeight, width, height, col );
//	m_pProgressBar->DrawSelf( xpos, (ypos - newHeight) + height + 1, width, height, col );
//	m_pProgressBar->DrawSelf( xpos, (ypos - newHeight) + height, width, height, col );
//	m_pProgressBar->DrawSelf( xpos, (ypos - newHeight) + height, width, newHeight, col );

	float ammoPerc = 1.0f - ( (float) value / (float) m_iMaxValue );
	gHUD.DrawIconProgressBar( xpos, ypos, width, height, m_pProgressBar, ammoPerc, col, CHud::HUDPB_VERTICAL );
//	gHUD.DrawIconProgressBar( xpos, ypos, m_pProgressBar, ammoPerc, col, CHud::HUDPB_VERTICAL );
//	gHUD.DrawProgressBar( xpos, ypos, width, height, ammoPerc, col, CHud::HUDPB_VERTICAL );
}

void CHudBitmapNumericDisplay::PaintDummies(float xpos, float ypos, int value, Color col, int numSigDigits, bool isSmallFont )
{
//	return; /// TODO: Remove!

	CHudTexture *numberArray[NUMBERS_COUNT];
	memset( numberArray, 0, NUMBERS_COUNT * sizeof(CHudTexture *) );
	if ( !isSmallFont )
	{
		memcpy(numberArray, m_pNumbers, NUMBERS_COUNT * sizeof(CHudTexture *));
	}
	else
	{
	//	downScale = 0.7f;
		memcpy(numberArray, m_pSmallNumbers, NUMBERS_COUNT * sizeof(CHudTexture *));
	}
//	memcpy(numberArray, m_pNumbers, NUMBERS_COUNT * sizeof(CHudTexture *));

	if( value > 100000 )
	{
		value = 99999;
	}

	int pos = 10000;

//	float scale = ( digit_height / (float)m_pNumbers[DIGIT_NUMBER_DUMMY1]->Height());
//	float width = ( isSmallFont ? digit2_width : digit_width );
//	float height = ( isSmallFont ? digit2_height : digit_height );
//	float width = ( isSmallFont ? digit2_height : digit_height ) / ( (float)m_pNumbers[DIGIT_NUMBER_DUMMY1]->Height() / (float)m_pNumbers[DIGIT_NUMBER_DUMMY1]->Width());
//	float height = ( isSmallFont ? digit2_height : digit_height );
//	float width = width / ( (float)XRES(26) / XRES(20));

	//	float scale = ( digit_height / (float)numberArray[0]->Height()) * downScale;
//	float width = ( isSmallFont ? digit2_width : digit_width );
	float width = (float)( isSmallFont ? digit2_height : digit_height ) / ( (float)numberArray[0]->Height() / (float)numberArray[0]->Width());
//	float height = ( isSmallFont ? digit2_height : digit_height );
	float height = (float)( isSmallFont ? digit2_height : digit_height );
	

	int digit;
	Color color = GetFgColor();
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

	//draw the digits
	while( pos >= 1 )
	{
		digit = value / pos;
		value = value % pos;
		
		bool isSecondPosition = false;
	//	Msg("%i\n", pos);
		if ( pos == 10 )
		{
			isSecondPosition = true;
		}
		
		if( bStart || digit > 0 || pos <= pow(10.0f,numSigDigits-1) )
		{
			bStart = true;
		//	m_pNumbers[digit]->DrawSelf( xpos, ypos, width, height, col );
			if ( isSecondPosition )
			{
				m_pNumbers[DIGIT_NUMBER_DUMMY2]->DrawSelf( xpos, ypos, width, height, col );
			}
			else
			{
				m_pNumbers[DIGIT_NUMBER_DUMMY1]->DrawSelf( xpos, ypos, width, height, col );
			}
			xpos += width;
		}		

		pos /= 10;
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
/*
void CHudBitmapNumericDisplay::PaintNumbers(float xpos, float ypos, int value, Color col, int numSigDigits, bool isSmallFont )
{
//	float downScale = 1.12f;

	CHudTexture *numberArray[NUMBERS_COUNT];
	memset( numberArray, 0, NUMBERS_COUNT * sizeof(CHudTexture *) );
	if ( !isSmallFont )
	{
	//	memcpy(numberArray, m_pNumbers, NUMBERS_COUNT * sizeof(CHudTexture *));
	}
	else
	{
	//	downScale = 0.7f;
	//	memcpy(numberArray, m_pSmallNumbers, NUMBERS_COUNT * sizeof(CHudTexture *));
	}
	memcpy(numberArray, m_pNumbers, NUMBERS_COUNT * sizeof(CHudTexture *));

	if( value > 100000 )
	{
		value = 99999;
	}

	int pos = 10000;

//	float scale = ( digit_height / (float)numberArray[0]->Height()) * downScale;
//	float width = ( isSmallFont ? digit2_width : digit_width );
	float width = (float)( isSmallFont ? digit2_height : digit_height ) / ( (float)numberArray[0]->Height() / (float)numberArray[0]->Width());
//	float height = ( isSmallFont ? digit2_height : digit_height );
	float height = (float)( isSmallFont ? digit2_height : digit_height );

	int digit;
	Color color = GetFgColor();
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
		//	xpos += width + 2; // VXP: New ValveTime.net screenshots
		//	xpos += width + 1; // VXP: It just moves numbers when they are below 100 and 10. What if number equals 0?
		}		

		pos /= 10;
	}
}
*/
void CHudBitmapNumericDisplay::PaintNumbers(float xpos, float ypos, int value, Color col, int numSigDigits, bool isSmallFont )
{
//	float downScale = 1.12f;

	CHudTexture *numberArray[NUMBERS_COUNT];
	memset( numberArray, 0, NUMBERS_COUNT * sizeof(CHudTexture *) );
	if ( !isSmallFont )
	{
		memcpy(numberArray, m_pNumbers, NUMBERS_COUNT * sizeof(CHudTexture *));
	}
	else
	{
	//	downScale = 0.7f;
		memcpy(numberArray, m_pSmallNumbers, NUMBERS_COUNT * sizeof(CHudTexture *));
	}
//	memcpy(numberArray, m_pNumbers, NUMBERS_COUNT * sizeof(CHudTexture *));

	if( value > 100000 )
	{
		value = 99999;
	}

	int pos = 10000;

//	float scale = ( digit_height / (float)numberArray[0]->Height()) * downScale;
//	float width = ( isSmallFont ? digit2_width : digit_width );
	float width = (float)( isSmallFont ? digit2_height : digit_height ) / ( (float)numberArray[0]->Height() / (float)numberArray[0]->Width());
//	float height = ( isSmallFont ? digit2_height : digit_height );
	float height = (float)( isSmallFont ? digit2_height : digit_height );

	int digit;
	Color color = GetFgColor();
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
		//	xpos += width + 2; // VXP: New ValveTime.net screenshots
		//	xpos += width + 1; // VXP: It just moves numbers when they are below 100 and 10. What if number equals 0?
		}		

		pos /= 10;
	}
}