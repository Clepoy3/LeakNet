#include "OptionsSubVideo.h"
#include "CvarSlider.h"
#include "EngineInterface.h"
#include "igameuifuncs.h"
#include "modes.h"

#include "iregistry.h"

#include "materialsystem/IMaterialSystem.h"
#include "materialsystem/IMaterialSystemHardwareConfig.h"

#include <vgui_controls/CheckButton.h>
#include <vgui_controls/ComboBox.h>
#include "CvarToggleCheckButton.h"
#include <KeyValues.h>
#include <vgui/ILocalize.h>
// memdbgon must be the last include file in a .cpp file!!!
#include <tier0/memdbgon.h>

//IMaterialSystem *materials = NULL;
//extern IMaterialSystem *materials;
//extern class IMaterialSystem *materials;

using namespace vgui;
	
inline bool IsWideScreen ( int width, int height )
{
	// VXP: Passes 1360x768 and 1366x768
	// 16:9 or 16:10 is widescreen :)
//	if ( (width * 9) == ( height * 16.0f ) || (width * 5.0) == ( height * 8.0 ))
//		return true;
//
//	return false;

	// VXP: Passes 1360x768 and 1366x768
//	float screenaspect = ( float )width / ( float ) height;
//	if ( screenaspect == ( 16.0f / 9.0f ) || screenaspect == ( 16.0f / 10.0f ) )
//		return true;
//
//	return false;

	float aspectRatio = (float)width/(float)height;
	bool bIsWidescreen = aspectRatio >= 1.5999f;
	return bIsWidescreen;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
COptionsSubVideo::COptionsSubVideo(vgui::Panel *parent) : PropertyPage(parent, NULL)
{
	memset( &m_OrigSettings, 0, sizeof( m_OrigSettings ) );
	memset( &m_CurrentSettings, 0, sizeof( m_CurrentSettings ) );
	
	registry->Init();

	m_pBrightnessSlider = new CCvarSlider( this, "Brightness", "#GameUI_Brightness",
		0.0f, 2.0f, "brightness" );

	m_pGammaSlider = new CCvarSlider( this, "Gamma", "#GameUI_Gamma",
		1.0f, 3.0f, "gamma" );

	GetVidSettings();

	m_pMode = new ComboBox(this, "Resolution", 6, false);

	m_pAspectRatio = new ComboBox( this, "AspectRatio", 2, false );

	wchar_t *unicodeText = localize()->Find("#GameUI_AspectNormal");
    localize()->ConvertUnicodeToANSI(unicodeText, m_pszAspectName[0], 32);
    unicodeText = localize()->Find("#GameUI_AspectWide");
    localize()->ConvertUnicodeToANSI(unicodeText, m_pszAspectName[1], 32);

	int iNormalItemID = m_pAspectRatio->AddItem( m_pszAspectName[0], NULL );
	int iWideItemID = m_pAspectRatio->AddItem( m_pszAspectName[1], NULL );
		
	m_bStartWidescreen = IsWideScreen( m_CurrentSettings.w, m_CurrentSettings.h );
	if ( m_bStartWidescreen )
	{
		m_pAspectRatio->ActivateItem( iWideItemID );
	}
	else
	{
		m_pAspectRatio->ActivateItem( iNormalItemID );
	}

	PrepareResolutionList();
/*
    // load up the renderer display names
    unicodeText = localize()->Find("#GameUI_Software");
    localize()->ConvertUnicodeToANSI(unicodeText, m_pszRenderNames[0], 32);
    unicodeText = localize()->Find("#GameUI_OpenGL");
    localize()->ConvertUnicodeToANSI(unicodeText, m_pszRenderNames[1], 32);
    unicodeText = localize()->Find("#GameUI_D3D");
    localize()->ConvertUnicodeToANSI(unicodeText, m_pszRenderNames[2], 32);

	m_pRenderer = new ComboBox( this, "Renderer", 3, false ); // "#GameUI_Renderer"
	int i;
    for ( i = 0; i < 3; i++)
    {
        m_pRenderer->AddItem( m_pszRenderNames[i], NULL );
    }
*/
/*	strcpy( m_pszAnisoNames[0], "Off" );
	strcpy( m_pszAnisoNames[1], "2x" );
	strcpy( m_pszAnisoNames[2], "4x" );
	strcpy( m_pszAnisoNames[3], "8x" );
	strcpy( m_pszAnisoNames[4], "16x" );*/
	localize()->ConvertUnicodeToANSI(L"Off", m_pszAnisoNames[0], 32);
	localize()->ConvertUnicodeToANSI(L"2x", m_pszAnisoNames[1], 32);
	localize()->ConvertUnicodeToANSI(L"4x", m_pszAnisoNames[2], 32);
	localize()->ConvertUnicodeToANSI(L"8x", m_pszAnisoNames[3], 32);
	localize()->ConvertUnicodeToANSI(L"16x", m_pszAnisoNames[4], 32);

	m_pAniso = new ComboBox( this, "Anisotroping", 5, false ); // "#GameUI_Aniso"
	int i;
    for ( i = 0; i < 5; i++)
    {
        m_pAniso->AddItem( m_pszAnisoNames[i], NULL );
    }

/*	strcpy( m_pszAntialiasNames[0], "Off" );
	strcpy( m_pszAntialiasNames[1], "2x" );
	strcpy( m_pszAntialiasNames[2], "4x" );
	strcpy( m_pszAntialiasNames[3], "8x" );*/
/*	localize()->ConvertUnicodeToANSI(L"Off", m_pszAntialiasNames[0], 32);
	localize()->ConvertUnicodeToANSI(L"2x", m_pszAntialiasNames[1], 32);
	localize()->ConvertUnicodeToANSI(L"4x", m_pszAntialiasNames[2], 32);
	localize()->ConvertUnicodeToANSI(L"8x", m_pszAntialiasNames[3], 32);

	m_pAntialias = new ComboBox( this, "Antialiasing", 4, false ); // "#GameUI_Antialias"
    for ( i = 0; i < 4; i++)
    {
        m_pAntialias->AddItem( m_pszAntialiasNames[i], NULL );
    }*/
	
	m_nNumAAModes = 0;
	m_pAntialias = new ComboBox( this, "Antialiasing", 10, false );
	m_pAntialias->AddItem("#GameUI_None", NULL);
	m_nAAModes[m_nNumAAModes].m_nNumSamples = 1;
	m_nAAModes[m_nNumAAModes].m_nQualityLevel = 0;
	m_nNumAAModes++;
	
	Assert( materials );

	if ( materials->SupportsMSAAMode(2) )
	{
		m_pAntialias->AddItem("#GameUI_2X", NULL);
		m_nAAModes[m_nNumAAModes].m_nNumSamples = 2;
		m_nAAModes[m_nNumAAModes].m_nQualityLevel = 0;
		m_nNumAAModes++;
	}

	if ( materials->SupportsMSAAMode(4) )
	{
		m_pAntialias->AddItem("#GameUI_4X", NULL);
		m_nAAModes[m_nNumAAModes].m_nNumSamples = 4;
		m_nAAModes[m_nNumAAModes].m_nQualityLevel = 0;
		m_nNumAAModes++;
	}

	if ( materials->SupportsMSAAMode(6) )
	{
		m_pAntialias->AddItem("#GameUI_6X", NULL);
		m_nAAModes[m_nNumAAModes].m_nNumSamples = 6;
		m_nAAModes[m_nNumAAModes].m_nQualityLevel = 0;
		m_nNumAAModes++;
	}

	if ( materials->SupportsCSAAMode(4, 2) )							// nVidia CSAA			"8x"
	{
		m_pAntialias->AddItem("#GameUI_8X_CSAA", NULL);
		m_nAAModes[m_nNumAAModes].m_nNumSamples = 4;
		m_nAAModes[m_nNumAAModes].m_nQualityLevel = 2;
		m_nNumAAModes++;
	}

	if ( materials->SupportsCSAAMode(4, 4) )							// nVidia CSAA			"16x"
	{
		m_pAntialias->AddItem("#GameUI_16X_CSAA", NULL);
		m_nAAModes[m_nNumAAModes].m_nNumSamples = 4;
		m_nAAModes[m_nNumAAModes].m_nQualityLevel = 4;
		m_nNumAAModes++;
	}

	if ( materials->SupportsMSAAMode(8) )
	{
		m_pAntialias->AddItem("#GameUI_8X", NULL);
		m_nAAModes[m_nNumAAModes].m_nNumSamples = 8;
		m_nAAModes[m_nNumAAModes].m_nQualityLevel = 0;
		m_nNumAAModes++;
	}

	if ( materials->SupportsCSAAMode(8, 2) )							// nVidia CSAA			"16xQ"
	{
		m_pAntialias->AddItem("#GameUI_16XQ_CSAA", NULL);
		m_nAAModes[m_nNumAAModes].m_nNumSamples = 8;
		m_nAAModes[m_nNumAAModes].m_nQualityLevel = 2;
		m_nNumAAModes++;
	}

	// VXP: Temporary solution...
//	m_pColorDepth = new ComboBox( this, "ColorDepth", 2, false );
//	m_pColorDepth->AddItem("Medium (16 bit)", NULL);
//	m_pColorDepth->AddItem("Highest (32 bit)", NULL);

//    SetCurrentRendererComboItem();
	SetCurrentAnisoComboItem();
	SetCurrentAntialiasComboItem();

	m_pWindowed = new vgui::CheckButton( this, "Windowed", "#GameUI_Windowed" );
	m_pWindowed->SetSelected( m_CurrentSettings.windowed ? true : false);

//	m_pWaterEntReflect = new vgui::CheckButton( this, "WaterEntReflect", "Water Entity Reflections" ); // VXP: #GameUI_WaterEntReflect
//	m_pWaterEntReflect->SetSelected( m_CurrentSettings.waterentreflect ? true : false);
	m_pWaterEntReflect = new CCvarToggleCheckButton( this, "WaterEntReflect", "Water Entity Reflections", "r_WaterEntReflection" );
	
	m_pMotionBlurEnableCheckButton = new CCvarToggleCheckButton( this, "pp_motionblur", "Enable Motion Blur", "pp_motionblur" );

	LoadControlSettings("Resource\\OptionsSubVideo.res");
}

void COptionsSubVideo::PrepareResolutionList( void )
{
	vmode_t *plist = NULL;
	int count = 0;

	gameuifuncs->GetVideoModes( &plist, &count );

	// Clean up before filling the info again.
	m_pMode->DeleteAllItems();

	int selectedItemID = -1;
	for (int i = 0; i < count; i++, plist++)
	{
		char sz[ 256 ];
		sprintf( sz, "%i x %i", plist->width, plist->height );

		int itemID = -1;
		if ( IsWideScreen( plist->width, plist->height ) )
		{
			if ( m_bStartWidescreen == true )
			{
				 itemID = m_pMode->AddItem( sz, NULL );
			}
		}
		else
		{
			if ( m_bStartWidescreen == false )
			{
				 itemID = m_pMode->AddItem( sz, NULL);
			}
		}
		if ( plist->width == m_CurrentSettings.w && 
			 plist->height == m_CurrentSettings.h )
		{
			selectedItemID = itemID;
		}
	}

	if ( selectedItemID != -1 )
	{
		m_pMode->ActivateItem( selectedItemID );
	}
	else
	{
		// just activate the first item
		m_pMode->ActivateItem( 0 );
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
COptionsSubVideo::~COptionsSubVideo()
{
	registry->Shutdown();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void COptionsSubVideo::OnResetData()
{
    // reset data
	RevertVidSettings();

    // reset UI elements
	m_pBrightnessSlider->Reset();
	m_pGammaSlider->Reset();
    m_pWindowed->SetSelected(m_CurrentSettings.windowed);
//	m_pWaterEntReflect->SetSelected(m_CurrentSettings.waterentreflect);
	
	m_pWaterEntReflect->Reset();
	m_pMotionBlurEnableCheckButton->Reset();

//    SetCurrentRendererComboItem();
	SetCurrentAnisoComboItem();
	SetCurrentAntialiasComboItem();
    SetCurrentResolutionComboItem();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
/*
void COptionsSubVideo::SetCurrentRendererComboItem()
{
	if ( !stricmp( m_CurrentSettings.renderer, "software" ) )
	{
        m_iStartRenderer = 0;
	}
	else if ( !stricmp( m_CurrentSettings.renderer, "gl" ) )
	{
        m_iStartRenderer = 1;
	}
	else if ( !stricmp( m_CurrentSettings.renderer, "d3d" ) )
	{
        m_iStartRenderer = 2;
	}
	else
	{
		// opengl by default
        m_iStartRenderer = 1;
	}
    m_pRenderer->ActivateItemByRow( m_iStartRenderer );
}
*/
void COptionsSubVideo::SetCurrentAnisoComboItem()
{
	const char* item = "";
	ConVar *var = (ConVar *)cvar->FindVar( "mat_forceaniso" );
	if ( var )
	{
		item = var->GetString();
	}

//	if( atof(item) <= 0 )
//	{
		m_iStartAniso = 0;
//	}
//	else
//	{
		int maxAnisoItems = 5;
		for( int i = 0; i < maxAnisoItems; i++ )
		{
		//	if( strcmp( item, m_pszAnisoNames[i] ) == 0 )
		//	Msg( "First: %i\n", strstr( m_pszAnisoNames[i], item ) );
			if( strstr( m_pszAnisoNames[i], item ) > 0 )
				m_iStartAniso = i;
		}
//	}

    m_pAniso->ActivateItemByRow( m_iStartAniso );
}

void COptionsSubVideo::SetComboItemAsRecommended( vgui::ComboBox *combo, int iItem )
	{
		// get the item text
		wchar_t text[512];
	//	combo->GetItemText(iItem, text, sizeof(text));
		combo->GetText(text, sizeof(text));

		// append the recommended flag
		wchar_t newText[512];
		_snwprintf( newText, sizeof(newText) / sizeof(wchar_t), L"%s *", text );

		// reset
		combo->UpdateItem(iItem, newText, NULL);
	}

int COptionsSubVideo::FindMSAAMode( int nAASamples, int nAAQuality )
{
	// Run through the AA Modes supported by the device
	for ( int nAAMode = 0; nAAMode < m_nNumAAModes; nAAMode++ )
	{
		// If we found the mode that matches what we're looking for, return the index
		if ( ( m_nAAModes[nAAMode].m_nNumSamples == nAASamples) && ( m_nAAModes[nAAMode].m_nQualityLevel == nAAQuality)  )
		{
			return nAAMode;
		}
	}

	return 0;	// Didn't find what we're looking for, so no AA
}

void COptionsSubVideo::SetCurrentAntialiasComboItem()
{
//	const char* item = "";
//	char buffer[33];
//	item = itoa( registry->ReadInt( "ScreenAntialias", -1 ), buffer, 10 );
	
	int nAASamples = registry->ReadInt( "ScreenAntialias", -1 );
	int nAAQuality = registry->ReadInt( "ScreenAntialiasQuality", 0 );
	int nMSAAMode = FindMSAAMode( nAASamples, nAAQuality );
	m_pAntialias->ActivateItem( nMSAAMode );

//	if( atof(item) < 2 )
//	{
		//m_iStartAntialias = 0;
//	}
//	else
//	{
	//	for( int i = 0; i < 4; i++ )
	//	{
		//	if( strcmp( item, m_pszAnisoNames[i] ) == 0 )
		//	Msg( "First: %i\n", strstr( m_pszAnisoNames[i], item ) );
			//if( strstr( m_pszAntialiasNames[i], item ) > 0 )
				//m_iStartAntialias = i;
	//	}
//	}

    //m_pAntialias->ActivateItemByRow( m_iStartAntialias );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void COptionsSubVideo::SetCurrentResolutionComboItem()
{
	vmode_t *plist = NULL;
	int count = 0;
	gameuifuncs->GetVideoModes( &plist, &count );

    int resolution = -1;
    for ( int i = 0; i < count; i++, plist++ )
	{
		if ( plist->width == m_CurrentSettings.w && 
			 plist->height == m_CurrentSettings.h )
		{
            resolution = i;
			break;
		}
	}

    if (resolution != -1)
	{
		char sz[256];
		sprintf(sz, "%i x %i", plist->width, plist->height);
        m_pMode->SetText(sz);
	}

	// if (m_CurrentSettings.bpp > 16)
	// {
		// m_pColorDepth->ActivateItemByRow(1);
	// }
	// else
	// {
		// m_pColorDepth->ActivateItemByRow(0);
	// }
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void COptionsSubVideo::OnApplyChanges()
{
    bool bChanged = m_pBrightnessSlider->HasBeenModified() || m_pGammaSlider->HasBeenModified();

	m_pBrightnessSlider->ApplyChanges();
	m_pGammaSlider->ApplyChanges();

	ApplyVidSettings(bChanged);
	
	m_pWaterEntReflect->ApplyChanges();
	m_pMotionBlurEnableCheckButton->ApplyChanges();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void COptionsSubVideo::GetVidSettings()
{
	// Get original settings
	CVidSettings *p = &m_OrigSettings;

	gameuifuncs->GetCurrentVideoMode( &p->w, &p->h, &p->bpp );
	gameuifuncs->GetCurrentRenderer( p->renderer, 128, &p->windowed );
	_strlwr( p->renderer );

	ConVar *var1 = (ConVar *)cvar->FindVar( "mat_forceaniso" );
	if( var1 )
	{
		p->aniso = var1->GetInt();
	}
//	p->antialias = registry->ReadInt( "ScreenAntialias", -1 );
	p->antialias = registry->ReadInt( "ScreenAntialias", -1 ); // VXP: Better
	p->antialiasquality = registry->ReadInt( "ScreenAntialiasQuality", 0 );
	// ConVar *var2 = (ConVar *)cvar->FindVar( "r_WaterEntReflection" );
	// if( var2 )
	// {
		// p->waterentreflect = var2->GetInt();
	// }

	m_CurrentSettings = m_OrigSettings;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void COptionsSubVideo::RevertVidSettings()
{
	m_CurrentSettings = m_OrigSettings;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void COptionsSubVideo::ApplyVidSettings(bool bForceRefresh)
{
	// Retrieve text from active controls and parse out strings
	if ( m_pMode )
	{
		char sz[256]/*, colorDepth[256]*/;
		m_pMode->GetText(sz, 256);
	//	m_pColorDepth->GetText(colorDepth, sizeof(colorDepth));

		int w, h;
		sscanf( sz, "%i x %i", &w, &h );
		m_CurrentSettings.w = w;
		m_CurrentSettings.h = h;
		// if (strstr(colorDepth, "32"))
		// {
			// m_CurrentSettings.bpp = 32;
		// }
		// else
		// {
			// m_CurrentSettings.bpp = 16;
		// }
	}
/*
	if ( m_pRenderer )
	{
		char sz[ 256 ];
		m_pRenderer->GetText(sz, sizeof(sz));

		if ( !stricmp( sz, m_pszRenderNames[0] ) )
		{
			strcpy( m_CurrentSettings.renderer, "software" );
		}
		else if ( !stricmp( sz, m_pszRenderNames[1] ) )
		{
			strcpy( m_CurrentSettings.renderer, "gl" );
		}
		else if ( !stricmp( sz, m_pszRenderNames[2] ) )
		{
			strcpy( m_CurrentSettings.renderer, "d3d" );
		}
	}
*/
//	Msg( "Before: %s\n", m_CurrentSettings.aniso );
	if ( m_pAniso )
	{
		char sz[ 256 ];
		m_pAniso->GetText(sz, sizeof(sz));

		if ( !_stricmp( sz, m_pszAnisoNames[0] ) )
		{
		//	strcpy( m_CurrentSettings.aniso, "0" );
			m_CurrentSettings.aniso = 0;
		}
		else if ( !_stricmp( sz, m_pszAnisoNames[1] ) )
		{
		//	strcpy( m_CurrentSettings.aniso, "2" );
			m_CurrentSettings.aniso = 2;
		}
		else if ( !_stricmp( sz, m_pszAnisoNames[2] ) )
		{
		//	strcpy( m_CurrentSettings.aniso, "4" );
			m_CurrentSettings.aniso = 4;
		}
		else if ( !_stricmp( sz, m_pszAnisoNames[3] ) )
		{
		//	strcpy( m_CurrentSettings.aniso, "8" );
			m_CurrentSettings.aniso = 8;
		}
		else if ( !_stricmp( sz, m_pszAnisoNames[4] ) )
		{
		//	strcpy( m_CurrentSettings.aniso, "16" );
			m_CurrentSettings.aniso = 16;
		}
	}
//	Msg( "After: %s\n", m_CurrentSettings.aniso );

	if ( m_pAntialias )
	{
	/*	char sz[ 256 ];
		m_pAntialias->GetText(sz, sizeof(sz));

		if ( !stricmp( sz, m_pszAntialiasNames[0] ) )
		{
			m_CurrentSettings.antialias = -1;
		}
		else if ( !stricmp( sz, m_pszAntialiasNames[1] ) )
		{
			m_CurrentSettings.antialias = 2;
		}
		else if ( !stricmp( sz, m_pszAntialiasNames[2] ) )
		{
			m_CurrentSettings.antialias = 4;
		}
		else if ( !stricmp( sz, m_pszAntialiasNames[3] ) )
		{
			m_CurrentSettings.antialias = 8;
		}*/
		// Set the AA convars according to the menu item chosen
		int nActiveAAItem = m_pAntialias->GetActiveItem();
	//	ApplyChangesToConVar( "mat_antialias", m_nAAModes[nActiveAAItem].m_nNumSamples );
		m_CurrentSettings.antialias = m_nAAModes[nActiveAAItem].m_nNumSamples;
		m_CurrentSettings.antialiasquality = m_nAAModes[nActiveAAItem].m_nQualityLevel;
	}

	if ( m_pWindowed )
	{
		bool checked = m_pWindowed->IsSelected();
		m_CurrentSettings.windowed = checked ? 1 : 0;
	}

	// if ( m_pWaterEntReflect )
	// {
		// char szCmd[ 256 ];
		// bool checked = m_pWaterEntReflect->IsSelected();

		// m_CurrentSettings.waterentreflect = checked ? 1 : 0;
		// sprintf( szCmd, "r_WaterEntReflection %i\n", m_CurrentSettings.waterentreflect );
		// engine->ClientCmd( szCmd );
	// }

	if ( memcmp( &m_OrigSettings, &m_CurrentSettings, sizeof( CVidSettings ) ) == 0 && !bForceRefresh)
	{
		return;
	}

	CVidSettings *p = &m_CurrentSettings;

	char szCmd[ 256 ];
	// Set mode
	sprintf( szCmd, "_setvideomode %i %i %i\n", p->w, p->h, p->bpp );
	engine->ClientCmd( szCmd );

	// Set renderer
//	sprintf( szCmd, "_setrenderer %s %s\n", p->renderer, p->windowed ? "windowed" : "fullscreen" ); // GoldSrc string
	sprintf( szCmd, "_setrenderer %s\n", /*p->renderer, */p->windowed ? "windowed" : "fullscreen" ); // SRC string
	engine->ClientCmd( szCmd );
	
	sprintf( szCmd, "mat_forceaniso %i\n", p->aniso );
	engine->ClientCmd( szCmd );

	registry->WriteInt( "ScreenAntialias", p->antialias );
	registry->WriteInt( "ScreenAntialiasQuality", p->antialiasquality );

	// Force restart of entire engine
//	engine->ClientCmd( "_restart\n" );
	if( m_CurrentSettings.w != m_OrigSettings.w ||
		m_CurrentSettings.h != m_OrigSettings.h ||
	//	m_CurrentSettings.bpp != m_OrigSettings.bpp ||
		m_CurrentSettings.windowed != m_OrigSettings.windowed ||
		m_CurrentSettings.antialias != m_OrigSettings.antialias ||
		m_CurrentSettings.antialiasquality != m_OrigSettings.antialiasquality )
	{
		engine->ClientCmd( "_restart\n" );
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void COptionsSubVideo::OnButtonChecked(KeyValues *data)
{
	int state = data->GetInt("state");
	Panel* pPanel = (Panel*) data->GetPtr("panel", NULL);

	if (pPanel == m_pWindowed)
	{
		if (state != m_CurrentSettings.windowed)
		{
            OnDataChanged();
		}
	}

	// if (pPanel == m_pWaterEntReflect)
	// {
		// if (state != m_CurrentSettings.waterentreflect)
		// {
            // OnDataChanged();
		// }
	// }
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void COptionsSubVideo::OnTextChanged(Panel *pPanel, const char *pszText)
{
	if (pPanel == m_pMode)
    {
		char sz[ 256 ];
		sprintf(sz, "%i x %i", m_CurrentSettings.w, m_CurrentSettings.h);

        if (strcmp(pszText, sz))
        {
            OnDataChanged();
        }
    }
	/*
    else if (pPanel == m_pRenderer)
    {
        if (strcmp(pszText, m_pszRenderNames[m_iStartRenderer]))
        {
            OnDataChanged();
        }
    }
	*/
	else if (pPanel == m_pAniso)
    {
    //    if (strcmp(pszText, m_pszAnisoNames[m_iStartAniso]))
		if (Q_strcmp(pszText, m_pszAnisoNames[m_iStartAniso]))
        {
            OnDataChanged();
        }
    }
	else if (pPanel == m_pAntialias)
    {
    //    if (strcmp(pszText, m_pszAntialiasNames[m_iStartAntialias]))
	//	if (Q_strcmp(pszText, m_pszAntialiasNames[m_iStartAntialias]))
		if (m_CurrentSettings.antialias != m_pAntialias->GetActiveItem() || m_CurrentSettings.antialiasquality != m_pAntialias->GetActiveItem()) // VXP: Wut?
        {
		//	Msg( "OnDataChanged\n" );
            OnDataChanged();
        }
    }
	else if (pPanel == m_pAspectRatio )
    {
		if ( strcmp(pszText, m_pszAspectName[m_bStartWidescreen] ) )
		{
			m_bStartWidescreen = !m_bStartWidescreen;
			PrepareResolutionList();
		}
    }
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void COptionsSubVideo::OnDataChanged()
{
	PostActionSignal(new KeyValues("ApplyButtonEnable"));
}

//-----------------------------------------------------------------------------
// Purpose: Message map
//-----------------------------------------------------------------------------
MessageMapItem_t COptionsSubVideo::m_MessageMap[] =
{
	MAP_MESSAGE_PTR_CONSTCHARPTR( COptionsSubVideo, "TextChanged", OnTextChanged, "panel", "text" ),
	//MAP_MESSAGE( COptionsSubVideo, "SliderMoved", OnDataChanged),
	MAP_MESSAGE_PARAMS( COptionsSubVideo, "CheckButtonChecked", OnButtonChecked),
	MAP_MESSAGE( COptionsSubVideo, "ControlModified", OnDataChanged),
};

IMPLEMENT_PANELMAP(COptionsSubVideo, BaseClass);
