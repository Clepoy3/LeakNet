//========= Copyright © 1996-2002, Valve LLC, All rights reserved. ============
//
// Purpose: HUD Target ID element
//
// $NoKeywords: $
//=============================================================================
#include "cbase.h"
#include "hud.h"
#include "hudelement.h"
#include "imessagechars.h"
#include "c_basehlplayer.h"
#include "c_playerresource.h"
#include "vgui_EntityPanel.h"
//#include "vgui_healthbar.h"
//#include "commanderoverlay.h"
//#include "c_baseobject.h"
#include "iclientmode.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

//#define PLAYER_HINT_DISTANCE	150
//#define PLAYER_HINT_DISTANCE_SQ	(PLAYER_HINT_DISTANCE*PLAYER_HINT_DISTANCE)

// Max Length of ID Strings
#define MAX_ID_STRING		256

static ConVar hud_showplayerinfo( "hud_showplayerinfo", "1", FCVAR_ARCHIVE, "Should game display player's info on the screen? 1 - enabled, 0 - disabled" );

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
class CTargetID : public CHudElement, public vgui::Panel
{
	DECLARE_CLASS_SIMPLE( CTargetID, vgui::Panel );

public:
	CTargetID( const char *pElementName );
	void Init( void );
	virtual void	ApplySchemeSettings( vgui::IScheme *scheme );
	virtual void	Paint( void );
	void VidInit( void );

private:
	vgui::HFont		m_hFont;
	int				m_iLastEntIndex;
	float			m_flLastChangeTime;
	char			m_sIDString[ MAX_ID_STRING ];
};

DECLARE_HUDELEMENT( CTargetID );

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
CTargetID::CTargetID( const char *pElementName ) :
	CHudElement( pElementName ), BaseClass( NULL, "TargetID" )
{
	vgui::Panel *pParent = g_pClientMode->GetViewport();
	SetParent( pParent );

	m_hFont = g_hFontTrebuchet24;
	m_flLastChangeTime = 0;
	m_iLastEntIndex = 0;
	m_sIDString[0] = 0;
}

//-----------------------------------------------------------------------------
// Purpose: Setup
//-----------------------------------------------------------------------------
void CTargetID::Init( void )
{
};

void CTargetID::ApplySchemeSettings( vgui::IScheme *scheme )
{
	BaseClass::ApplySchemeSettings( scheme );

	SetPaintBackgroundEnabled( false );
}

//-----------------------------------------------------------------------------
// Purpose: clear out string etc between levels
//-----------------------------------------------------------------------------
void CTargetID::VidInit()
{
	CHudElement::VidInit();

	m_flLastChangeTime = 0;
	m_iLastEntIndex = 0;
	m_sIDString[0] = 0;
}

//-----------------------------------------------------------------------------
// Purpose: Draw function for the element
//-----------------------------------------------------------------------------
void CTargetID::Paint()
{
	// No id if still choosing class
//	if ( C_BaseTFPlayer::GetLocalPlayer()->GetClass() == TFCLASS_UNDECIDED )
	if( hud_showplayerinfo.GetInt() != 1 )
		return;
	
	if( !C_BaseHLPlayer::GetLocalPlayer() ) // VXP: Hope, this prevent crashes on "disconnect"
		return;

	// Get our target's ent index
	int iEntIndex = C_BaseHLPlayer::GetLocalPlayer()->GetIDTarget();
	// Didn't find one?
	if ( !iEntIndex )
	{
		// Check to see if we should clear our ID
	//	if ( m_flLastChangeTime && (gpGlobals->curtime > (m_flLastChangeTime + 0.5)) )
		if ( m_flLastChangeTime && (gpGlobals->curtime > m_flLastChangeTime) )
		{
			m_flLastChangeTime = 0;
			m_sIDString[0] = 0;
			m_iLastEntIndex = 0;
		}
		else
		{
			// Keep re-using the old one
			iEntIndex = m_iLastEntIndex;
		}
	}
	else
	{
		m_flLastChangeTime = gpGlobals->curtime;
	}

	// Is this an entindex sent by the server?
	if ( iEntIndex )
	{
		C_BaseHLPlayer *pPlayer = static_cast<C_BaseHLPlayer*>(cl_entitylist->GetEnt( iEntIndex ));
	//	C_BasePlayer *pLocalPlayer = C_BasePlayer::GetLocalPlayer();
		C_BaseHLPlayer *pLocalPlayer = dynamic_cast<C_BaseHLPlayer *>(C_BasePlayer::GetLocalPlayer());

		// Some entities we always want to check, cause the text may change
		// even while we're looking at it
		// Is it a player?
		if ( IsPlayerIndex( iEntIndex ) )
		{
			if ( pPlayer->InSameTeam(pLocalPlayer) )
			{
				// Check distance to other player, and if the player is on the same team
			//	float flDistSq = pPlayer->GetRenderOrigin().DistToSqr( pLocalPlayer->GetRenderOrigin() );
			//	if ( flDistSq < PLAYER_HINT_DISTANCE_SQ )
			//	{
			//		Q_snprintf( m_sIDString, sizeof(m_sIDString), "%s\nHealth: %.0f percent\nUse to donate resources",
			//			pPlayer->GetPlayerName(), ((float)pPlayer->GetHealth() / (float)pPlayer->GetMaxHealth() ) );
			//	}
			//	else
				{
					Q_snprintf( m_sIDString, sizeof(m_sIDString), "%s\nHealth: %.0f percent",
						pPlayer->GetPlayerName(), ((float)pPlayer->GetHealth() / (float)pPlayer->GetMaxHealth() ) );
				}
			}
		//	else if (( pPlayer->GetHealth() == 0) && (pLocalPlayer->GetClass() == TFCLASS_INFILTRATOR) )
		//	{
		//		Q_snprintf( m_sIDString, sizeof(m_sIDString), "%s\nUse to disguise as this player", pPlayer->GetPlayerName() );
		//	}
			else
			{
				m_sIDString[0] = 0;
				m_iLastEntIndex = 0;
			}
		}
		else
		{
			// Objects
			C_BaseEntity *pEnt = cl_entitylist->GetEnt( iEntIndex );
			if ( !pEnt || !pEnt->InSameTeam(pLocalPlayer) )
			{
				// This can happen because the object was destroyed
				m_sIDString[0] = 0;
				m_iLastEntIndex = 0;
			}
			else
			{
				// Don't check validity if it's sent by the server
				Q_strncpy( m_sIDString, pEnt->GetIDString(), sizeof(m_sIDString) );
				m_iLastEntIndex = iEntIndex;
			}
		}
	}

	// Draw our ID string
	if ( m_sIDString[0] )
	{
		int width, height;
		int ypos = YRES(300);

		// Messagechars can't handle multiple line strings, so parse out the \n's and give it one line at a time
		char *ch = m_sIDString;
		while ( *ch )
		{
			// Find the next newline
			char *next_line;
			for ( next_line = ch; *next_line != '\n' && *next_line != 0; next_line++ )
			{
			}

			// Stomp the newline
			char *top = next_line;
			if ( *top == '\n' )
			{
				*top = 0;
			}
			else
			{
				top = NULL;
			}

			// Draw the line
			messagechars->GetStringLength( m_hFont, &width, &height, ch );
			messagechars->DrawString( m_hFont, (ScreenWidth() - width) / 2, ypos, 255, 255, 245, 255, ch, IMessageChars::MESSAGESTRINGID_NONE );

			ypos += height;

			// Restore the newline
			if ( top ) 
			{
				*top = '\n';
			}

			// Move to the next line
			ch = next_line;
			if ( *ch == '\n' )
			{
				ch++;
			}
		}
	}
}
