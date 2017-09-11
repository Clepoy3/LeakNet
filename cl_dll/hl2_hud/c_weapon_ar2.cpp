//========= Copyright © 1996-2001, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================
#include "cbase.h"
#include "hud.h"
#include "hudelement.h"
#include "hud_macros.h"
#include "parsemsg.h"
#include "vgui_BasePanel.h"
#include "c_basehlcombatweapon.h"
#include "iclientmode.h"

#define	AR2MODE_FADE_TIME			120
#define AR2MODE_MIN_ALPHA			40 // 16

ConVar hud_ar2_mode_icon( "hud_ar2_mode_icon", "1", FCVAR_ARCHIVE, "Enables or disables appearing of the AR2 mode HUD icon" );
ConVar hud_ar2_mode_full_dim( "hud_ar2_mode_full_dim", "0", FCVAR_ARCHIVE, "Enables or disables full dimming of the AR2 mode HUD icon" );

//=============================================================================
// Purpose: 
//=============================================================================
class CHudAR2Mode : public CHudElement, public vgui::Panel
{
	DECLARE_CLASS_SIMPLE( CHudAR2Mode, vgui::Panel );

public:
	CHudAR2Mode( const char *pElementName );
	void			Init( void );
	virtual bool	ShouldDraw();
	virtual void	ApplySchemeSettings( vgui::IScheme *scheme );
	virtual void	Paint( void );
	void			VidInit( void );

	enum AR2Mode_t
	{
		AR2MODE_NONE = -1,
		AR2MODE_ZOOM = 0,
		AR2MODE_GRENADE
	};

	void			SetMode( AR2Mode_t mode );
	void			MsgFunc_AR2ModeChanged( const char *pszName, int iSize, void *pbuf );

protected:
	CPanelAnimationVarAliasType( float, icon_xpos, "icon_xpos", "0", "proportional_float" );
	CPanelAnimationVarAliasType( float, icon_ypos, "icon_ypos", "0", "proportional_float" );

private:
	CHudTexture		*m_pZoomIcon;
	CHudTexture		*m_pGrenadeIcon;

	AR2Mode_t		m_currentMode;

	float			m_fFade;
};

DECLARE_HUDELEMENT( CHudAR2Mode );
DECLARE_HUD_MESSAGE( CHudAR2Mode, AR2ModeChanged );

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
CHudAR2Mode::CHudAR2Mode( const char *pElementName ) :
	CHudElement( pElementName ), BaseClass( NULL, pElementName )
{
	vgui::Panel *pParent = g_pClientMode->GetViewport();
	SetParent( pParent );

//	SetAutoDelete( false );
//	SetName( "AR2Mode" );

	m_currentMode	= AR2MODE_ZOOM;
	m_fFade			= AR2MODE_FADE_TIME;
}

//-----------------------------------------------------------------------------
// Purpose: Setup
//-----------------------------------------------------------------------------
void CHudAR2Mode::Init( void )
{
	HOOK_MESSAGE( AR2ModeChanged );
}

void CHudAR2Mode::SetMode( AR2Mode_t mode )
{
	if ( m_currentMode != mode )
	{
		m_currentMode = mode;
	}

	m_fFade			= AR2MODE_FADE_TIME;
}

bool CHudAR2Mode::ShouldDraw()
{
//	C_HLPlayer *pPlayer = C_HLPlayer::GetLocalHLPlayer();
//	return pPlayer && pPlayer->GetActiveWeapon();

	C_BasePlayer *pPlayer = C_BasePlayer::GetLocalPlayer();
	if ( !pPlayer )
		return false;

	C_BaseCombatWeapon *pWeapon = pPlayer->GetActiveWeapon();
	if ( !pWeapon )
		return false;

	return ( CHudElement::ShouldDraw() && FClassnameIs( pWeapon, "weapon_ar2" ) );
}

void CHudAR2Mode::ApplySchemeSettings( vgui::IScheme *scheme )
{
	BaseClass::ApplySchemeSettings( scheme );

	SetPaintBackgroundEnabled( false );
}

//-----------------------------------------------------------------------------
// Purpose: clear out string etc between levels
//-----------------------------------------------------------------------------
void CHudAR2Mode::VidInit()
{
	CHudElement::VidInit();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CHudAR2Mode::Paint()
{
	if ( !hud_ar2_mode_icon.GetBool() )
		return;

	if ( !m_pZoomIcon )
	{
		m_pZoomIcon = gHUD.GetIcon( "ar2_zoommode" );
	}

	if ( !m_pGrenadeIcon )
	{
		m_pGrenadeIcon = gHUD.GetIcon( "ar2_grenademode" );
	}

	int scalar;
	int minscalar = hud_ar2_mode_full_dim.GetBool() ? 0 : AR2MODE_MIN_ALPHA;
	// Get our intensity
	if ( m_fFade > 0.0f )
	{
		m_fFade  -= (gpGlobals->frametime * 50.0f);

		if ( m_fFade  < 128.0f )
		{
			scalar = (int) max( minscalar, (m_fFade) ); // We want full animation
		}
		else
		{
			scalar = 128;
		}
	}
	else
	{
		scalar = minscalar;
	}

	if ( scalar <= 0 )
		return;

	scalar = -1.0f * scalar;

	Color col = gHUD.m_clrNormal;
	col[3] = 255 * scalar;

//	Msg( "Current AR2 mode: " );
	switch ( m_currentMode )
	{
	case AR2MODE_ZOOM:
	//	Msg( "AR2MODE_ZOOM\n" );
		m_pZoomIcon->DrawSelf( icon_xpos, icon_ypos, m_pZoomIcon->Width(), m_pZoomIcon->Height(), col );
		m_pZoomIcon->DrawSelf( icon_xpos, icon_ypos, m_pZoomIcon->Width(), m_pZoomIcon->Height(), col );
		break;
	case AR2MODE_GRENADE:
	//	Msg( "AR2MODE_GRENADE\n" );
		m_pGrenadeIcon->DrawSelf( icon_xpos, icon_ypos, m_pGrenadeIcon->Width(), m_pGrenadeIcon->Height(), col );
		m_pGrenadeIcon->DrawSelf( icon_xpos, icon_ypos, m_pGrenadeIcon->Width(), m_pGrenadeIcon->Height(), col );
		break;
	default:
		break;
	}
}

//-----------------------------------------------------------------------------
// Purpose: Handle an item pickup event from the server
//-----------------------------------------------------------------------------
void CHudAR2Mode::MsgFunc_AR2ModeChanged( const char *pszName, int iSize, void *pbuf )
{
	BEGIN_READ( pbuf, iSize );
	const bool m_bUseGrenade = READ_BOOL();

	if ( m_bUseGrenade )
	{
		Msg( "Setting %i (grenade) mode for AR2\n", CHudAR2Mode::AR2MODE_GRENADE );
		SetMode( CHudAR2Mode::AR2MODE_GRENADE );
	}
	else
	{
		Msg( "Setting %i (zoom) mode for AR2\n", CHudAR2Mode::AR2MODE_ZOOM );
		SetMode( CHudAR2Mode::AR2MODE_ZOOM );
	}
}

/*
class C_WeaponAR2 : public C_HLMachineGun
{
	DECLARE_CLASS( C_WeaponAR2, C_HLMachineGun );

public:
	DECLARE_PREDICTABLE();
	DECLARE_CLIENTCLASS();
//	C_WeaponAR2() {};
	C_WeaponAR2();
	~C_WeaponAR2();

	void	Spawn( void );

//	bool	m_bUseGrenade;

private:
	C_WeaponAR2( const C_WeaponAR2 & );

	CHudAR2Mode	*m_pAR2Mode;
};

BEGIN_PREDICTION_DATA( C_WeaponAR2 )
END_PREDICTION_DATA()

LINK_ENTITY_TO_CLASS( weapon_ar2, C_WeaponAR2 );

IMPLEMENT_CLIENTCLASS_DT( C_WeaponAR2, DT_WeaponAR2, CWeaponAR2 )
//	RecvPropBool( RECVINFO( m_bUseGrenade ) ),
END_RECV_TABLE()

C_WeaponAR2::C_WeaponAR2()
{
	m_pAR2Mode = GET_HUDELEMENT( CHudAR2Mode );
}

C_WeaponAR2::~C_WeaponAR2()
{
}

*/