//========= Copyright © 1996-2001, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================

#ifndef OPTIONS_SUB_VIDEO_H
#define OPTIONS_SUB_VIDEO_H
#ifdef _WIN32
#pragma once
#endif

#include <vgui_controls/Panel.h>
#include <vgui_controls/ComboBox.h>
#include <vgui_controls/PropertyPage.h>
#include "EngineInterface.h"
#include "igameuifuncs.h"

namespace vgui
{
class CheckButton;
class ComboBox;
}

class CCvarSlider;

class CCvarToggleCheckButton;

//-----------------------------------------------------------------------------
// Purpose: Video Details, Part of OptionsDialog
//-----------------------------------------------------------------------------
class COptionsSubVideo : public vgui::PropertyPage
{
public:
	COptionsSubVideo(vgui::Panel *parent);
	~COptionsSubVideo();

	virtual void OnResetData();
	virtual void OnApplyChanges();

private:
	typedef vgui::PropertyPage BaseClass;

	struct CVidSettings
	{
		int			w, h;
		int			bpp;
		int			windowed;
		char		renderer[ 128 ];
		int			aniso;
		int			antialias;
		int			waterentreflect;
	};

	CVidSettings		m_OrigSettings;
	CVidSettings		m_CurrentSettings;

	void		GetVidSettings();
	void		RevertVidSettings();
	void		ApplyVidSettings(bool bForceRefresh);

//    void        SetCurrentRendererComboItem();
	void        SetCurrentAnisoComboItem();
	void        SetCurrentAntialiasComboItem();
    void        SetCurrentResolutionComboItem();

	void		OnButtonChecked(KeyValues *data);
	void		OnTextChanged(vgui::Panel *pPanel, const char *pszText);
    void        SetRendererComboItem();
	void		PrepareResolutionList( void );

	vgui::ComboBox *m_pMode;
//	vgui::ComboBox *m_pRenderer;
	vgui::ComboBox *m_pAniso;
	vgui::ComboBox *m_pAntialias;
	vgui::ComboBox *m_pColorDepth;
	vgui::CheckButton *m_pWindowed;
	vgui::CheckButton *m_pWaterEntReflect;
	vgui::ComboBox *m_pAspectRatio;

	CCvarSlider		*m_pBrightnessSlider;
	CCvarSlider		*m_pGammaSlider;

//    char            m_pszRenderNames[3][32];
	char            m_pszAnisoNames[5][32];
	char            m_pszAntialiasNames[4][32];
	char            m_pszAspectName[2][32];

//    int             m_iStartRenderer;
	int             m_iStartAniso;
	int             m_iStartAntialias;
    int             m_iStartResolution;
	bool			m_bStartWidescreen;
	
	CCvarToggleCheckButton  *m_pMotionBlurEnableCheckButton;


	DECLARE_PANELMAP();
    void OnDataChanged();
};



#endif // OPTIONS_SUB_VIDEO_H