//========= Copyright © 1996-2001, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================
#include "cbase.h"
#include "ivieweffects.h"
#include "shake.h"
#include "parsemsg.h"
#include "usermessages.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

// VXP: Arbitrary limit so that bad entity logic on the server can't consume tons of memory on the client.
#define MAX_SHAKES		32

//-----------------------------------------------------------------------------
// Purpose: Screen fade variables
//-----------------------------------------------------------------------------
struct screenfade_t
{
	float		Speed;		// How fast to fade (tics / second) (+ fade in, - fade out)
	float		End;		// When the fading hits maximum
	float		Reset;		// When to reset to not fading (for fadeout and hold)
	byte		r, g, b, alpha;	// Fade color
	int			Flags;		// Fading flags
};

//-----------------------------------------------------------------------------
// Purpose: Screen shake variables
//-----------------------------------------------------------------------------
struct screenshake_t 
{
	float	time;
	float	duration;
	float	amplitude;
	float	frequency;
	float	nextShake;
	Vector	offset;
	float	angle;
//	Vector	appliedOffset;
//	float	appliedAngle;
};

//-----------------------------------------------------------------------------
// Purpose: Implements the view effects interface for the client .dll
//-----------------------------------------------------------------------------
class CViewEffects : public IViewEffects
{
public:

	~CViewEffects()
	{
		ClearAllFades();
	}

	virtual void	Init( void );
	virtual void	LevelInit( void );
	virtual void	GetFadeParams( int context, byte *r, byte *g, byte *b, byte *a, bool *blend );
	virtual void	CalcShake( void );
	virtual void	ApplyShake( Vector& origin, QAngle& angles, float factor );

	virtual int		Shake( const char *pszName, int iSize, void *pbuf );
	virtual int		Fade( const char *pszName, int iSize, void *pbuf );
	virtual void	ClearPermanentFades( void );
	virtual void	FadeCalculate( void );
	virtual void	ClearAllFades( void );

private:
	// VXP
	void ClearAllShakes();
	screenshake_t *FindLongestShake();

	CUtlVector< screenfade_t* >	m_FadeList;
//	screenshake_t				m_Shake;

	// VXP
	CUtlVector< screenshake_t* > m_ShakeList;
	Vector m_vecShakeAppliedOffset;
	float m_flShakeAppliedAngle;

	int							m_FadeColorRGBA[4];
	bool						m_bModulate;
};

static CViewEffects g_ViewEffects;
IViewEffects *vieweffects = ( IViewEffects * )&g_ViewEffects;

// Callback function to call at end of screen m_Fade.
static int s_nCallbackParameter;
static void ( *s_pfnFadeDoneCallback )( int parm1 );


//-----------------------------------------------------------------------------
// Purpose: 
// Input  : *pszName - 
//			iSize - 
//			*pbuf - 
// Output : static int
//-----------------------------------------------------------------------------
static int V_ScreenShake( const char *pszName, int iSize, void *pbuf )
{
	return g_ViewEffects.Shake( pszName, iSize, pbuf );
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : *pszName - 
//			iSize - 
//			*pbuf - 
// Output : static int
//-----------------------------------------------------------------------------
static int V_ScreenFade( const char *pszName, int iSize, void *pbuf )
{
	return g_ViewEffects.Fade( pszName, iSize, pbuf );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CViewEffects::Init( void )
{
	usermessages->HookMessage( "Shake", (pfnUserMsgHook)V_ScreenShake );
	usermessages->HookMessage( "Fade", (pfnUserMsgHook)V_ScreenFade );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CViewEffects::LevelInit( void )
{
	// Clear out any screen shake
//	memset( &m_Shake, 0, sizeof(m_Shake) );
	ClearAllShakes(); // VXP

	ClearAllFades();
}

//-----------------------------------------------------------------------------
// Purpose: Apply noise to the eye position.
// UNDONE: Feedback a bit of this into the view model position.  It shakes too much
//-----------------------------------------------------------------------------
void CViewEffects::CalcShake( void )
{
	float	frametime;
	int		i;
	float	fraction, freq;

	// VXP: We'll accumulate the aggregate shake for this frame into these data members.
	m_vecShakeAppliedOffset.Init(0, 0, 0);
	m_flShakeAppliedAngle = 0;

	int nShakeCount = m_ShakeList.Count();

	for ( int nShake = nShakeCount - 1; nShake >= 0; nShake-- )
	{
		screenshake_t *pShake = m_ShakeList.Element( nShake );

		if ( pShake->time == 0 )
		{
		//	return;
			continue;
		}

		if ( ( gpGlobals->curtime > pShake->time ) || 
			pShake->duration <= 0 || 
			pShake->amplitude <= 0 || 
			pShake->frequency <= 0 )
		{
		//	memset( &m_Shake, 0, sizeof(m_Shake) );
		//	return;

			// Retire this shake.
			delete m_ShakeList.Element( nShake );
			m_ShakeList.FastRemove( nShake );
			continue;
		}
		frametime = gpGlobals->frametime;
	
		if ( gpGlobals->curtime > pShake->nextShake )
		{
			// Higher frequency means we recalc the extents more often and perturb the display again
			pShake->nextShake = gpGlobals->curtime + (1.0f / pShake->frequency);
			// Compute random shake extents (the shake will settle down from this)
			for (i = 0; i < 3; i++ )
			{
				pShake->offset[i] = random->RandomFloat( -pShake->amplitude, pShake->amplitude );
			}
			pShake->angle = random->RandomFloat( -pShake->amplitude*0.25, pShake->amplitude*0.25 );
		}
		// Ramp down amplitude over duration (fraction goes from 1 to 0 linearly with slope 1/duration)
		fraction = ( pShake->time - gpGlobals->curtime ) / pShake->duration;

		// Ramp up frequency over duration
		if ( fraction )
		{
			freq = (pShake->frequency / fraction);
		}
		else
		{
			freq = 0;
		}

		// square fraction to approach zero more quickly
		fraction *= fraction;

		// Sine wave that slowly settles to zero
		float angle = gpGlobals->curtime * freq;
		if ( angle > 1e8 )
		{
			angle = 1e8;
		}
		fraction = fraction * sin( angle );
		
		// Add to view origin
	//	for ( i = 0; i < 3; i++ )
	//	{
	//		m_Shake.appliedOffset[i] = m_Shake.offset[i] * fraction;
	//	}
		m_vecShakeAppliedOffset = pShake->offset * fraction;
		
		// Add to roll
		m_flShakeAppliedAngle = pShake->angle * fraction;
	
		// Drop amplitude a bit, less for higher frequency shakes
		pShake->amplitude -= pShake->amplitude * ( frametime / (pShake->duration * pShake->frequency) );
	}
}

//-----------------------------------------------------------------------------
// Purpose: Apply the current screen shake to this origin/angles.  Factor is the amount to apply
//  This is so you can blend in part of the shake
// Input  : *origin - 
//			*angles - 
//			factor - 
//-----------------------------------------------------------------------------
void CViewEffects::ApplyShake( Vector& origin, QAngle& angles, float factor )
{
	VectorMA( origin, factor, m_vecShakeAppliedOffset, origin );
	angles.z += m_flShakeAppliedAngle * factor;
}


//-----------------------------------------------------------------------------
// Purpose: Zeros out all active screen shakes.
//-----------------------------------------------------------------------------
void CViewEffects::ClearAllShakes()
{
	int nShakeCount = m_ShakeList.Count();
	for ( int i = 0; i < nShakeCount; i++ )
	{
		delete m_ShakeList.Element( i );
	}

	m_ShakeList.Purge();
}


//-----------------------------------------------------------------------------
// Purpose: Returns the shake with the longest duration. This is the shake we
//			use anytime we get an amplitude or frequency command, because the
//			most likely case is that we're modifying a shake with a long
//			duration rather than a brief shake caused by an explosion, etc.
//-----------------------------------------------------------------------------
screenshake_t *CViewEffects::FindLongestShake()
{
	screenshake_t *pLongestShake = NULL;

	int nShakeCount = m_ShakeList.Count();
	for ( int i = 0; i < nShakeCount; i++ )
	{
		screenshake_t *pShake = m_ShakeList.Element( i );
		if ( pShake && ( !pLongestShake || ( pShake->duration > pLongestShake->duration ) ) )
		{
			pLongestShake = pShake;
		}
	}

	return pLongestShake;
}


//-----------------------------------------------------------------------------
// Purpose: Message hook to parse ScreenShake messages
// Input  : pszName - 
//			iSize - 
//			pbuf - 
// Output : 
//-----------------------------------------------------------------------------
int CViewEffects::Shake( const char *pszName, int iSize, void *pbuf )
{
	BEGIN_READ( pbuf, iSize );

	ShakeCommand_t eCommand = (ShakeCommand_t)READ_BYTE();
	float amplitude = READ_FLOAT();
	float frequency = READ_FLOAT();
	float duration = READ_FLOAT();
/*
	if (eCommand == SHAKE_STOP)
	{
		m_Shake.amplitude = 0;
		m_Shake.frequency = 0;
		m_Shake.duration = 0;
	}
	else
	{
		if (( eCommand == SHAKE_START) || ( eCommand == SHAKE_FREQUENCY )) 
		{
			m_Shake.frequency = frequency;
		}

		if (( eCommand == SHAKE_START) || ( eCommand == SHAKE_AMPLITUDE ))
		{
			//
			// Don't overwrite larger existing shake unless we are told to.
			//
			if (( amplitude > m_Shake.amplitude ) || ( eCommand == SHAKE_AMPLITUDE ))
			{
				m_Shake.amplitude = amplitude;
			}
		}

		//
		// Only reset the timer for a new shake.
		//
		if (eCommand == SHAKE_START)
		{
			m_Shake.duration = duration;
			m_Shake.nextShake = 0;
			m_Shake.time = gpGlobals->curtime + duration;
		}
	}
*/

	if ( eCommand == SHAKE_START && ( m_ShakeList.Count() < MAX_SHAKES ) )
	{
		screenshake_t *pNewShake = new screenshake_t;
		
		pNewShake->amplitude = amplitude;
		pNewShake->frequency = frequency;
		pNewShake->duration = duration;
		pNewShake->nextShake = 0;
		pNewShake->time = gpGlobals->curtime + duration;

		m_ShakeList.AddToTail( pNewShake );
	}
	else if (eCommand == SHAKE_STOP)
	{
		ClearAllShakes();
	}
	else if ( eCommand == SHAKE_AMPLITUDE )
	{
		// VXP: Look for the most likely shake to modify.
		screenshake_t *pShake = FindLongestShake();
		if ( pShake )
		{
			pShake->amplitude = amplitude;
		}
	}
	else if ( eCommand == SHAKE_FREQUENCY )
	{
		// Look for the most likely shake to modify.
		screenshake_t *pShake = FindLongestShake();
		if ( pShake )
		{
			pShake->frequency = frequency;
		}
	}

	return 1;
}


//-----------------------------------------------------------------------------
// Purpose: Message hook to parse ScreenFade messages
// Input  : *pszName - 
//			iSize - 
//			*pbuf - 
// Output : int
//-----------------------------------------------------------------------------
int CViewEffects::Fade( const char *pszName, int iSize, void *pbuf )
{
	ScreenFade_t *pFade = (ScreenFade_t *)pbuf;

	// Create a new fade and append it to the list
	screenfade_t *pNewFade = new screenfade_t;
	pNewFade->End	= pFade->duration * (1.0f/(float)(1<<SCREENFADE_FRACBITS));
	pNewFade->Reset	= pFade->holdTime * (1.0f/(float)(1<<SCREENFADE_FRACBITS));
	pNewFade->r		= pFade->r;
	pNewFade->g		= pFade->g;
	pNewFade->b		= pFade->b;
	pNewFade->alpha	= pFade->a;
	pNewFade->Flags	= pFade->fadeFlags;
	pNewFade->Speed	= 0;
	
	// Calc fade speed
	if ( pFade->duration > 0 )
	{
		if ( pFade->fadeFlags & FFADE_OUT )
		{
			if ( pNewFade->End )
			{
				pNewFade->Speed = -(float)pNewFade->alpha / pNewFade->End;
			}

			pNewFade->End	+= gpGlobals->curtime;
			pNewFade->Reset	+= pNewFade->End;
		}
		else
		{
			if ( pNewFade->End )
			{
				pNewFade->Speed = (float)pNewFade->alpha / pNewFade->End;
			}

			pNewFade->Reset	+= gpGlobals->curtime;
			pNewFade->End	+= pNewFade->Reset;
		}
	}

	if ( pFade->fadeFlags & FFADE_PURGE )
	{
		ClearAllFades();
	}

	m_FadeList.AddToTail( pNewFade );

	return 1;
}

//-----------------------------------------------------------------------------
// Purpose: Compute the overall color & alpha of the fades
//-----------------------------------------------------------------------------
void CViewEffects::FadeCalculate( void )
{
	// Cycle through all fades and remove any that have finished (work backwards)
	int i;
	int iSize = m_FadeList.Size();
	for (i = iSize-1; i >= 0; i-- )
	{
		screenfade_t *pFade = m_FadeList[i];

		// Keep pushing reset time out indefinitely
		if ( pFade->Flags & FFADE_STAYOUT )
		{
			pFade->Reset = gpGlobals->curtime + 0.1;
		}

		// All done?
		if ( ( gpGlobals->curtime > pFade->Reset ) && ( gpGlobals->curtime > pFade->End ) )
		{
			// User passed in a callback function, call it now
			if ( s_pfnFadeDoneCallback )
			{
				s_pfnFadeDoneCallback( s_nCallbackParameter );
				s_pfnFadeDoneCallback = NULL;
				s_nCallbackParameter = 0;
			}

			// Remove this Fade from the list
			m_FadeList.FindAndRemove( pFade );
			delete pFade;
		}
	}

	m_bModulate = false;
	m_FadeColorRGBA[0] = m_FadeColorRGBA[1] = m_FadeColorRGBA[2] = m_FadeColorRGBA[3] = 0;

	// Cycle through all fades in the list and calculate the overall color/alpha
	for ( i = 0; i < m_FadeList.Size(); i++ )
	{
		screenfade_t *pFade = m_FadeList[i];

		// Color
		m_FadeColorRGBA[0] += pFade->r;
		m_FadeColorRGBA[1] += pFade->g;
		m_FadeColorRGBA[2] += pFade->b;

		// Fading...
		int iFadeAlpha;
		if ( pFade->Flags & (FFADE_OUT|FFADE_IN) )
		{
			iFadeAlpha = pFade->Speed * ( pFade->End - gpGlobals->curtime );
			if ( pFade->Flags & FFADE_OUT )
			{
				iFadeAlpha += pFade->alpha;
			}
			iFadeAlpha = min( iFadeAlpha, pFade->alpha );
			iFadeAlpha = max( 0, iFadeAlpha );
		}
		else
		{
			iFadeAlpha = pFade->alpha;
		}

		// Use highest alpha
		if ( iFadeAlpha > m_FadeColorRGBA[3] )
		{
			m_FadeColorRGBA[3] = iFadeAlpha;
		}

		// Modulate?
		if ( pFade->Flags & FFADE_MODULATE )
		{
			m_bModulate = true;
		}
	}

	// Divide colors
	if ( m_FadeList.Size() )
	{
		m_FadeColorRGBA[0] /= m_FadeList.Size();
		m_FadeColorRGBA[1] /= m_FadeList.Size();
		m_FadeColorRGBA[2] /= m_FadeList.Size();
	}
}

//-----------------------------------------------------------------------------
// Purpose: Clear only the permanent fades in our fade list
//-----------------------------------------------------------------------------
void CViewEffects::ClearPermanentFades( void )
{
	int iSize = m_FadeList.Size();
	for (int i =  iSize-1; i >= 0; i-- )
	{
		screenfade_t *pFade = m_FadeList[i];

		if ( pFade->Flags & FFADE_STAYOUT )
		{
			// Destroy this fade
			m_FadeList.FindAndRemove( pFade );
			delete pFade;
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: Purge & delete all fades in the queue
//-----------------------------------------------------------------------------
void CViewEffects::ClearAllFades( void )
{
	int iSize = m_FadeList.Size();
	for (int i =  iSize-1; i >= 0; i-- )
	{
		delete m_FadeList[i];
	}
	m_FadeList.Purge();
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : context - Which call to Render is this ( CViewSetup::context )
//			*r - 
//			*g - 
//			*b - 
//			*a - 
//			*blend - 
//-----------------------------------------------------------------------------
void CViewEffects::GetFadeParams( int context, byte *r, byte *g, byte *b, byte *a, bool *blend )
{
	FadeCalculate();

	*r = m_FadeColorRGBA[0];
	*g = m_FadeColorRGBA[1];
	*b = m_FadeColorRGBA[2];
	*a = m_FadeColorRGBA[3];
	*blend = m_bModulate;
}
