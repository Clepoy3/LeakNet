#include "cbase.h"
#include "tfclassmenu.h"
using namespace vgui;
#include <vgui/IVGui.h>
#include <vgui_controls/Frame.h>
#include <vgui_controls/EditablePanel.h>
#include <game_controls/ClassMenu.h>
#include "c_basetfplayer.h"
//#include <cstrike/VGUI/CStrikeClassMenu.h>
#include <FileSystem.h>

#include "hud_macros.h"
#include "parsemsg.h"

class CTF2ClassPanel : public CClassMenu
{
	//DECLARE_CLASS_SIMPLE(CTF2ClassPanel, vgui::Frame); 
	DECLARE_CLASS_SIMPLE( CTF2ClassPanel, CClassMenu );
	
	CTF2ClassPanel(vgui::VPANEL parent); 	// Constructor
	~CTF2ClassPanel(){};	// Destructor

	vgui::Panel *m_pAssociatedButton; // used to load class .res and .tga files
	vgui::Panel *CreateControlByName(const char *controlName);

public:
	void MsgFunc_ShowTFClassPanel( const char *pszName, int iSize, void *pbuf );
	void SetVisibleButton(const char *textEntryName, bool state);

protected:
	//VGUI overrides:
	virtual void OnTick();
	virtual void OnCommand(const char* pcCommand);

private:
	int m_iFirst;

	class ClassHelperPanel : public vgui::EditablePanel
	{
	private:
		typedef vgui::EditablePanel BaseClass;

	public:
		ClassHelperPanel( vgui::Panel *parent, const char *panelName ) : vgui::EditablePanel( parent, panelName ) { m_pAssociatedButton = NULL; }
		
		virtual void SetAssociatedButton( vgui::Panel *button ) { m_pAssociatedButton = button; }
		virtual vgui::Panel *GetAssociatedButton( void ) { return m_pAssociatedButton; }

	private:
		vgui::Panel *m_pAssociatedButton; // used to load class .res and .tga files

		virtual void ApplySchemeSettings( vgui::IScheme *pScheme );
		const char *GetClassPage( const char *className );
	};

};

DECLARE_MESSAGE( CTF2ClassPanel( NULL ), ShowTFClassPanel );

// Constuctor: Initializes the Panel
CTF2ClassPanel::CTF2ClassPanel(vgui::VPANEL parent)
: BaseClass( this )
//: BaseClass(NULL, "TF2ClassPanel")
{
	HOOK_MESSAGE( ShowTFClassPanel );

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

	LoadControlSettings("resource/UI/ClassMenu.res");

	vgui::ivgui()->AddTickSignal( GetVPanel(), 100 );
	
}

class CTF2ClassPanelInterface : public ITF2ClassPanel
{
private:
	CTF2ClassPanel *TF2ClassPanel;
public:
	CTF2ClassPanelInterface()
	{
		TF2ClassPanel = NULL;
	}
	void Create(vgui::VPANEL parent)
	{
		TF2ClassPanel = new CTF2ClassPanel(parent);
	}
	void Destroy()
	{
		if (TF2ClassPanel)
		{
			TF2ClassPanel->SetParent( (vgui::Panel *)NULL);
			delete TF2ClassPanel;
		}
	}
};
static CTF2ClassPanelInterface g_TF2ClassPanel;
ITF2ClassPanel* tf2classpanel = (ITF2ClassPanel*)&g_TF2ClassPanel;

ConVar tf2_showclassselection("tf2_showclassselection", "0", FCVAR_CLIENTDLL, "Show/hide the class selection panel");

void CTF2ClassPanel::OnTick()
{
	BaseClass::OnTick();

	C_BaseTFPlayer *pPlayer = C_BaseTFPlayer::GetLocalPlayer();
	if ( !pPlayer ) return; //we can't get the local player. are we being called from the main menu?

	if ( tf2_showclassselection.GetBool() )
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
	else
	{
		SetVisibleButton( "CancelButton", true );
	}
}

//-----------------------------------------------------------------------------
// Purpose: Sets the visibility of a button by name
//-----------------------------------------------------------------------------
void CTF2ClassPanel::SetVisibleButton(const char *textEntryName, bool state)
{
	Button *entry = dynamic_cast<Button *>(FindChildByName(textEntryName));
	if (entry)
	{
		entry->SetVisible(state);
	}
}

void CTF2ClassPanel::OnCommand(const char* pcCommand)
{
	if(!Q_stricmp(pcCommand, "turnoff"))
		tf2_showclassselection.SetValue(0);

	if( !Q_stricmp( pcCommand, "changeclass_commando" ) )
	{
		tf2_showclassselection.SetValue( 0 );
		if ( C_BaseTFPlayer::GetLocalPlayer()->GetClass() == TFCLASS_COMMANDO ) return;
		engine->ClientCmd( "changeclass_commando" );
	}

	if( !Q_stricmp( pcCommand, "changeclass_defender" ) )
	{
		tf2_showclassselection.SetValue( 0 );
		if ( C_BaseTFPlayer::GetLocalPlayer()->GetClass() == TFCLASS_DEFENDER ) return;
		engine->ClientCmd( "changeclass_defender" );
	}

	if( !Q_stricmp( pcCommand, "changeclass_escort" ) )
	{
		tf2_showclassselection.SetValue( 0 );
		if ( C_BaseTFPlayer::GetLocalPlayer()->GetClass() == TFCLASS_ESCORT ) return;
		engine->ClientCmd( "changeclass_escort" );
	}

	if( !Q_stricmp( pcCommand, "changeclass_medic" ) )
	{
		tf2_showclassselection.SetValue( 0 );
		if ( C_BaseTFPlayer::GetLocalPlayer()->GetClass() == TFCLASS_MEDIC ) return;
		engine->ClientCmd( "changeclass_medic" );
	}

	if( !Q_stricmp( pcCommand, "changeclass_pyro" ) )
	{
		tf2_showclassselection.SetValue( 0 );
		if ( C_BaseTFPlayer::GetLocalPlayer()->GetClass() == TFCLASS_PYRO ) return;
		engine->ClientCmd( "changeclass_pyro" );
	}
	
	if( !Q_stricmp( pcCommand, "changeclass_recon" ) )
	{
		tf2_showclassselection.SetValue( 0 );
		if ( C_BaseTFPlayer::GetLocalPlayer()->GetClass() == TFCLASS_RECON ) return;
		engine->ClientCmd( "changeclass_recon" );
	}

	if( !Q_stricmp( pcCommand, "changeclass_sapper" ) )
	{
		tf2_showclassselection.SetValue( 0 );
		if ( C_BaseTFPlayer::GetLocalPlayer()->GetClass() == TFCLASS_SAPPER ) return;
		engine->ClientCmd( "changeclass_sapper" );
	}
}

void CTF2ClassPanel::MsgFunc_ShowTFClassPanel( const char *pszName, int iSize, void *pbuf )
{
	tf2_showclassselection.SetValue( 1 );
}

CON_COMMAND(changeclass, "Toggles the class selection panel")
{
	tf2_showclassselection.SetValue(!tf2_showclassselection.GetBool());
};

Panel *CTF2ClassPanel::CreateControlByName(const char *controlName)
{
	if( !stricmp( "MouseOverPanelButton", controlName ) )
	{
		ClassHelperPanel *classPanel = new ClassHelperPanel( this, NULL );
		classPanel->SetVisible( false );

		int x,y,wide,tall;
		m_pPanel->GetBounds( x, y, wide, tall );
		classPanel->SetBounds( x, y, wide, tall );
		//classPanel->SetPos( 30, 50 );
		
		MouseOverPanelButton *newButton = new MouseOverPanelButton( this, NULL, classPanel );
		classPanel->SetAssociatedButton( newButton ); // class panel will use this to lookup the .res file

		if ( m_iFirst )
		{
			m_iFirst = 0;
			m_pFirstButton = newButton; // this first button's page is triggered when the panel is set visible
		}
		return newButton;
	}
	else
	{
		return BaseClass::CreateControlByName( controlName );
	}
}

// Purpose: causes the class panel to load the resource file for its class
//-----------------------------------------------------------------------------
void CTF2ClassPanel::ClassHelperPanel::ApplySchemeSettings( IScheme *pScheme )
{
	Assert( GetAssociatedButton() );
	LoadControlSettings( GetClassPage( GetAssociatedButton()->GetName() ) );
	
	BaseClass::ApplySchemeSettings( pScheme );
}


//-----------------------------------------------------------------------------
// Purpose: returns the filename of the class file for this class
//-----------------------------------------------------------------------------
const char *CTF2ClassPanel::ClassHelperPanel::GetClassPage( const char *className )
{
	static char classPanel[ _MAX_PATH ];
	Q_snprintf( classPanel, sizeof( classPanel ), "classes/%s.res", className );
	Msg( "ClassPage returning %s\n", classPanel );

	if ( vgui::filesystem()->FileExists( classPanel ) )
	{
	
	}
	else if (vgui::filesystem()->FileExists( "classes/defender.res" ) )
	{
		Q_snprintf( classPanel, sizeof( classPanel ), "classes/defender.res" );
	}
	else
	{
		return NULL;
	}

	return classPanel;
}