//=========== (C) Copyright 1999 Valve, L.L.C. All rights reserved. ===========
//
// The copyright to the contents herein is the property of Valve, L.L.C.
// The contents may be used and/or copied only with the written permission of
// Valve, L.L.C., or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: 
// Implements local hooks into named renderable textures.
// See matrendertexture.cpp in material system for list of available RT's
//
//=============================================================================

#include "materialsystem/imesh.h"
#include "materialsystem/ITexture.h"
#include "materialsystem/IMaterialSystem.h"

#include "cdll_util.h"

ConVar pp_motionblur_downscale("pp_motionblur_downscale", "1", FCVAR_ARCHIVE, "Downscale for motion blur, 1 = normal, 2 - half of game resolution. Values should be from 1 to 3"); 

//=============================================================================
// 
//=============================================================================

/*static CTextureReference s_pMotionBlurTex0;

//ITexture *GetMotionBlurTex0( void )
ITexture *GetMotionBlurTex0( int w, int h )
{
	if( !s_pMotionBlurTex0 )
	{
	//	s_pMotionBlurTex0.InitRenderTarget( TEXTURE_PAGE_SIZE, TEXTURE_PAGE_SIZE, IMAGE_FORMAT_ARGB8888, false );
		s_pMotionBlurTex0.InitRenderTarget( w, h, IMAGE_FORMAT_ARGB8888, false );
  		Assert( s_pMotionBlurTex0 );
	}
	
	return s_pMotionBlurTex0;
}*/

CTextureReference m_MotionBlurTexture;

static float m_flMotionBlurDownscaleValue = 1.0f;
ITexture *GetMotionBlurTexture()
{
	float downscale = pp_motionblur_downscale.GetFloat();
	downscale = clamp( downscale, 1.0f, 3.0f );

	if(!m_MotionBlurTexture || downscale != m_flMotionBlurDownscaleValue)
	{
		// VXP: Shutting down existing texture
		if ( m_MotionBlurTexture != NULL && downscale != m_flMotionBlurDownscaleValue )
		{
			m_MotionBlurTexture.Shutdown();
		}

	//	m_MotionBlurTexture.InitRenderTarget(256, 256, RT_SIZE_FULL_FRAME_BUFFER, IMAGE_FORMAT_ARGB8888, MATERIAL_RT_DEPTH_NONE, false);
	//	m_MotionBlurTexture.InitRenderTarget(256, 256, IMAGE_FORMAT_ARGB8888, false);
	
		int width, height;
		materials->GetBackBufferDimensions( width, height );

		// Storing current downscale value in cache
		m_flMotionBlurDownscaleValue = downscale;

		width /= downscale;
		height /= downscale;

		Msg( "Getting motion blur overlay of %ix%i size\n", width, height );
		m_MotionBlurTexture.InitRenderTarget(width, height, IMAGE_FORMAT_ARGB8888, false);
	
	//	Assert(!IsErrorTexture(m_MotionBlurTexture));
		Assert( m_MotionBlurTexture );
	}
 
	return m_MotionBlurTexture;
} 
/*static ITexture *m_MotionBlurTexture = NULL;
//static CTextureReference m_MotionBlurTexture;
ITexture *GetMotionBlurTexture( void )
{
	if( !m_MotionBlurTexture )
	{
		bool bFound;
		m_MotionBlurTexture = materials->FindTexture( "_rt_MotionBlur", &bFound );
	//	m_MotionBlurTexture.Init( materials->FindTexture( "_rt_MotionBlur", &bFound ) );
		Assert( bFound );
	}
	
	return m_MotionBlurTexture;
}*/

//=============================================================================
// 
//=============================================================================
static ITexture *s_pPowerOfTwoFrameBufferTexture = NULL;

static void PowerOfTwoFrameBufferTextureRestoreFunc( void )
{
	s_pPowerOfTwoFrameBufferTexture = NULL;
}

ITexture *GetPowerOfTwoFrameBufferTexture( void )
{
	if( !s_pPowerOfTwoFrameBufferTexture )
	{
		bool bFound;
		s_pPowerOfTwoFrameBufferTexture = materials->FindTexture( "_rt_PowerOfTwoFB", &bFound );
		Assert( bFound );

		static bool bAddedRestoreFunc = false;
		if( !bAddedRestoreFunc )
		{
			materials->AddRestoreFunc( PowerOfTwoFrameBufferTextureRestoreFunc );
			bAddedRestoreFunc = true;
		}
	}
	
	return s_pPowerOfTwoFrameBufferTexture;
}

//=============================================================================
// 
//=============================================================================
static ITexture *s_pCameraTexture = NULL;

static void CameraTextureRestoreFunc( void )
{
	s_pCameraTexture = NULL;
}

ITexture *GetCameraTexture( void )
{
	if( !s_pCameraTexture )
	{
		bool bFound;
		s_pCameraTexture = materials->FindTexture( "_rt_Camera", &bFound );
		Assert( bFound );

		static bool bAddedRestoreFunc = false;
		if( !bAddedRestoreFunc )
		{
			materials->AddRestoreFunc( CameraTextureRestoreFunc );
			bAddedRestoreFunc = true;
		}
	}
	
	return s_pCameraTexture;
}

//=============================================================================
// 
//=============================================================================
/*static ITexture *s_pFullFrameFrameBufferTexture = NULL;

static void FullFrameFrameBufferTextureRestoreFunc( void )
{
	if (s_pFullFrameFrameBufferTexture)
	{
		s_pFullFrameFrameBufferTexture = NULL;
	}
}

ITexture *GetFullFrameFrameBufferTexture( void )
{
	if( !s_pFullFrameFrameBufferTexture )
	{
		bool bFound;
		s_pFullFrameFrameBufferTexture = materials->FindTexture( "_rt_FullFrameFB", &bFound );
		Assert( bFound );

		static bool bAddedRestoreFunc = false;
		if( !bAddedRestoreFunc )
		{
			materials->AddRestoreFunc( FullFrameFrameBufferTextureRestoreFunc );
			bAddedRestoreFunc = true;
		}
	}
	
	return s_pFullFrameFrameBufferTexture;
}*/

// VXP: Multiple
static CTextureReference s_pFullFrameFrameBufferTexture[MAX_FB_TEXTURES];
static void FullFrameFrameBufferTextureRestoreFunc( void )
{
	if (s_pFullFrameFrameBufferTexture)
	{
		s_pFullFrameFrameBufferTexture->Shutdown();
	//	delete[] s_pFullFrameFrameBufferTexture;
	}
}
ITexture *GetFullFrameFrameBufferTexture( int textureIndex /*= 0*/ )
{
	if ( !s_pFullFrameFrameBufferTexture[textureIndex] )
	{
		char name[256];
		if( textureIndex != 0 )
		{
			sprintf( name, "_rt_FullFrameFB%d", textureIndex );
		}
		else
		{
			Q_strcpy( name, "_rt_FullFrameFB" );
		}
	//	s_pFullFrameFrameBufferTexture[textureIndex].Init( materials->FindTexture( name, TEXTURE_GROUP_RENDER_TARGET ) );
		s_pFullFrameFrameBufferTexture[textureIndex].Init( materials->FindTexture( name, NULL ) );
	//	Assert( !IsErrorTexture( s_pFullFrameFrameBufferTexture[textureIndex] ) );
		Assert( s_pFullFrameFrameBufferTexture[textureIndex] );
	//	AddReleaseFunc();
		static bool bAddedRestoreFunc = false;
		if( !bAddedRestoreFunc )
		{
			materials->AddRestoreFunc( FullFrameFrameBufferTextureRestoreFunc );
			bAddedRestoreFunc = true;
		}
	}
	
	return s_pFullFrameFrameBufferTexture[textureIndex];
}

//=============================================================================
// Water reflection
//=============================================================================
static ITexture *s_pWaterReflectionTexture = NULL;

static void WaterReflectionTextureRestoreFunc( void )
{
	s_pWaterReflectionTexture = NULL;
}

ITexture *GetWaterReflectionTexture( void )
{
	if( !s_pWaterReflectionTexture )
	{
		bool bFound;
		s_pWaterReflectionTexture = materials->FindTexture( "_rt_WaterReflection", &bFound );
		Assert( bFound );

		static bool bAddedRestoreFunc = false;
		if( !bAddedRestoreFunc )
		{
			materials->AddRestoreFunc( WaterReflectionTextureRestoreFunc );
			bAddedRestoreFunc = true;
		}
	}
	
	return s_pWaterReflectionTexture;
}

//=============================================================================
// Water refraction
//=============================================================================
static ITexture *s_pWaterRefractionTexture = NULL;

static void WaterRefractionTextureRestoreFunc( void )
{
	s_pWaterRefractionTexture = NULL;
}

ITexture *GetWaterRefractionTexture( void )
{
	if( !s_pWaterRefractionTexture )
	{
		bool bFound;
		s_pWaterRefractionTexture = materials->FindTexture( "_rt_WaterRefraction", &bFound );
		Assert( bFound );

		static bool bAddedRestoreFunc = false;
		if( !bAddedRestoreFunc )
		{
			materials->AddRestoreFunc( WaterRefractionTextureRestoreFunc );
			bAddedRestoreFunc = true;
		}
	}
	
	return s_pWaterRefractionTexture;
}


//=============================================================================
// 
//=============================================================================
static ITexture *s_pSmallBufferHDR0 = NULL;

static void SmallBufferHDR0RestoreFunc( void )
{
	s_pSmallBufferHDR0 = NULL;
}

ITexture *GetSmallBufferHDR0( void )
{
	if( !s_pSmallBufferHDR0 )
	{
		bool bFound;
		s_pSmallBufferHDR0 = materials->FindTexture( "_rt_SmallHDR0", &bFound );
		Assert( bFound );

		static bool bAddedRestoreFunc = false;
		if( !bAddedRestoreFunc )
		{
			materials->AddRestoreFunc( SmallBufferHDR0RestoreFunc );
			bAddedRestoreFunc = true;
		}
	}
	
	return s_pSmallBufferHDR0;
}

//=============================================================================
// 
//=============================================================================
static ITexture *s_pSmallBufferHDR1 = NULL;

static void SmallBufferHDR1RestoreFunc( void )
{
	s_pSmallBufferHDR0 = NULL;
}

ITexture *GetSmallBufferHDR1( void )
{
	if( !s_pSmallBufferHDR1 )
	{
		bool bFound;
		s_pSmallBufferHDR1 = materials->FindTexture( "_rt_SmallHDR1", &bFound );
		Assert( bFound );

		static bool bAddedRestoreFunc = false;
		if( !bAddedRestoreFunc )
		{
			materials->AddRestoreFunc( SmallBufferHDR1RestoreFunc );
			bAddedRestoreFunc = true;
		}
	}
	
	return s_pSmallBufferHDR1;
}

