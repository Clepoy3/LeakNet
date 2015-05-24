#include "cbase.h"
#include "tfteammenu.h"
using namespace vgui;
#include <vgui/IVGui.h>
#include <vgui_controls/Frame.h>
#include <vgui_controls/Button.h>
#include "c_basetfplayer.h"

#include "hud_macros.h"
#include "parsemsg.h"

class CTF2TeamPanel : public vgui::Frame
{
	DECLARE_CLASS_SIMPLE(CTF2TeamPanel, vgui::Frame); 
	
	CTF2TeamPanel(vgui::VPANEL parent); 	// Constructor
	~CTF2TeamPanel(){};				// Destructor

public:
	void MsgFunc_ShowTFTeamPanel( const char *pszName, int iSize, void *pbuf );
	void SetVisibleButton(const char *textEntryName, bool state);

protected:
	virtual void OnTick();
	virtual void OnCommand(const char* pcCommand);
	
};

DECLARE_MESSAGE( CTF2TeamPanel( NULL ), ShowTFTeamPanel );


CTF2TeamPanel::CTF2TeamPanel(vgui::VPANEL parent)
: BaseClass(NULL, "TF2TeamPanel")
{
	HOOK_MESSAGE( ShowTFTeamPanel );

	SetParent( parent );
	
	SetKeyBoardInputEnabled( true );
	SetMouseInputEnabled( true );
	
	SetProportional( true );
	SetTitleBarVisible( false );
	SetMinimizeButtonVisible( false );
	SetMaximizeButtonVisible( false );
	SetCloseButtonVisible( false );
	SetSizeable( false );
	SetMoveable( true );
	SetVisible( false );


	SetScheme(vgui::scheme()->LoadSchemeFromFile("resource/ClientSchemeTeamMenu.res", "ClientScheme"));

	LoadControlSettings("resource/UI/TeamMenu.res");

	vgui::ivgui()->AddTickSignal( GetVPanel(), 100 );
	
}

class CTF2TeamPanelInterface : public ITF2TeamPanel
{
private:
	CTF2TeamPanel *TF2TeamPanel;
public:
	CTF2TeamPanelInterface()
	{
		TF2TeamPanel = NULL;
	}
	void Create(vgui::VPANEL parent)
	{
		TF2TeamPanel = new CTF2TeamPanel(parent);
	}
	void Destroy()
	{
		if (TF2TeamPanel)
		{
			TF2TeamPanel->SetParent( (vgui::Panel *)NULL);
			delete TF2TeamPanel;
		}
	}
};
static CTF2TeamPanelInterface g_TF2TeamPanel;
ITF2TeamPanel* tf2teampanel = (ITF2TeamPanel*)&g_TF2TeamPanel;


ConVar tf2_showteamselection("tf2_showteamselection", "0", FCVAR_CLIENTDLL, "Show/hide the team selection panel");

void CTF2TeamPanel::OnTick()
{
	BaseClass::OnTick();

	C_BaseTFPlayer *pPlayer = C_BaseTFPlayer::GetLocalPlayer();
	if ( !pPlayer ) return; //we can't get the local player. are we being called from the main menu?

	if ( tf2_showteamselection.GetBool() )
	{
		SetVisible( true );
	}
	else
	{
		SetVisible( false );
	}

	if ( !pPlayer->GetClass() )
	{
		SetVisibleButton( "CancelButton", false );
	}

}

//-----------------------------------------------------------------------------
// Purpose: Sets the visibility of a button by name
//-----------------------------------------------------------------------------
void CTF2TeamPanel::SetVisibleButton(const char *textEntryName, bool state)
{
	Button *entry = dynamic_cast<Button *>(FindChildByName(textEntryName));
	if (entry)
	{
		entry->SetVisible(state);
	}
}

void CTF2TeamPanel::OnCommand(const char* pcCommand)
{
	if(!Q_stricmp(pcCommand, "turnoff"))
		tf2_showteamselection.SetValue(0);

	// Forward jointeam commands to the server
	if (!Q_stricmp( pcCommand, "jointeam_aliens" ) )
	{
		tf2_showteamselection.SetValue(0);
		if ( C_BaseTFPlayer::GetLocalPlayer()->GetTeamNumber() == TEAM_ALIENS ) return; //Cancel if we're already on that team, duh
		engine->ClientCmd( "jointeam_aliens" );
	}

	if (!Q_stricmp( pcCommand, "jointeam_humans" ) )
	{
		tf2_showteamselection.SetValue(0);
		if ( C_BaseTFPlayer::GetLocalPlayer()->GetTeamNumber() == TEAM_HUMANS ) return; //Cancel if we're already on that team, duh
		engine->ClientCmd( "jointeam_humans" );
	}

	if (!Q_stricmp( pcCommand, "jointeam_auto" ) )
	{
		tf2_showteamselection.SetValue(0);
		engine->ClientCmd( "jointeam_auto" );
	}
}

void CTF2TeamPanel::MsgFunc_ShowTFTeamPanel( const char *pszName, int iSize, void *pbuf )
{
	tf2_showteamselection.SetValue( 1 );
}

CON_COMMAND(changeteam, "Toggles the team selection panel")
{
	tf2_showteamselection.SetValue(!tf2_showteamselection.GetBool());
};