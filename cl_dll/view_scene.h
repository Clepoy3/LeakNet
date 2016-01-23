//========= Copyright © 1996-2002, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================

#ifndef VIEW_SCENE_H
#define VIEW_SCENE_H
#ifdef _WIN32
#pragma once
#endif


#include "convar.h"

/*#include "materialsystem/ITexture.h"
#include "iviewrender.h"
#include "view_shared.h"
#include "view.h"
#include "rendertexture.h"*/
#include "iviewrender.h"
#include "view_shared.h"
#include "rendertexture.h"
#include "materialsystem/ITexture.h"

extern ConVar mat_wireframe;


// Transform into view space (translate and rotate the camera into the origin).
void ViewTransform( const Vector &worldSpace, Vector &viewSpace );

// Transform a world point into normalized screen space (X and Y from -1 to 1).
// Returns 0 if the point is behind the viewer.
int ScreenTransform( const Vector& point, Vector& screen );

inline void UpdateRefractTexture( void )
{
	ITexture *pTexture = GetPowerOfTwoFrameBufferTexture();
	materials->CopyRenderTargetToTexture( pTexture );
//	materials->SetFrameBufferCopyTexture( pTexture );
	materials->SetFrameBufferCopyTexture( pTexture, 0 );
}

/*inline void UpdateScreenEffectTexture( void )
{
	ITexture *pTexture = GetFullFrameFrameBufferTexture();
	materials->CopyRenderTargetToTexture( pTexture );
	materials->SetFrameBufferCopyTexture( pTexture );
}*/
inline void UpdateScreenEffectTexture( int textureIndex, int x, int y, int w, int h, bool bDestFullScreen = false, Rect_t *pActualRect = NULL )
{
	Rect_t srcRect;
	srcRect.x = x;
	srcRect.y = y;
	srcRect.width = w;
	srcRect.height = h;

//	CMatRenderContextPtr pRenderContext( materials );
	ITexture *pTexture = GetFullFrameFrameBufferTexture( textureIndex );
//	ITexture *pTexture = GetFullFrameFrameBufferTexture();
	int nSrcWidth, nSrcHeight;
//	pRenderContext->GetRenderTargetDimensions( nSrcWidth, nSrcHeight );
	materials->GetRenderTargetDimensions( nSrcWidth, nSrcHeight );
	int nDestWidth = pTexture->GetActualWidth();
	int nDestHeight = pTexture->GetActualHeight();

	Rect_t destRect = srcRect;
	if( !bDestFullScreen && ( nSrcWidth > nDestWidth || nSrcHeight > nDestHeight ) )
	{
		// the source and target sizes aren't necessarily the same (specifically in dx7 where 
		// nonpow2 rendertargets aren't supported), so lets figure it out here.
		float scaleX = ( float )nDestWidth / ( float )nSrcWidth;
		float scaleY = ( float )nDestHeight / ( float )nSrcHeight;
		destRect.x = srcRect.x * scaleX;
		destRect.y = srcRect.y * scaleY;
		destRect.width = srcRect.width * scaleX;
		destRect.height = srcRect.height * scaleY;
		destRect.x = clamp( destRect.x, 0, nDestWidth );
		destRect.y = clamp( destRect.y, 0, nDestHeight );
		destRect.width = clamp( destRect.width, 0, nDestWidth - destRect.x );
		destRect.height = clamp( destRect.height, 0, nDestHeight - destRect.y );
	}

//	pRenderContext->CopyRenderTargetToTextureEx( pTexture, 0, &srcRect, bDestFullScreen ? NULL : &destRect );
//	pRenderContext->SetFrameBufferCopyTexture( pTexture, textureIndex );
	materials->CopyRenderTargetToTextureEx( pTexture, 0, &srcRect, bDestFullScreen ? NULL : &destRect );
//	materials->CopyRenderTargetToTexture( pTexture );
	materials->SetFrameBufferCopyTexture( pTexture, textureIndex );

	if ( pActualRect )
	{
		pActualRect->x = destRect.x;
		pActualRect->y = destRect.y;
		pActualRect->width = destRect.width;
		pActualRect->height = destRect.height;
	}
}

inline void UpdateScreenEffectTexture( void )
{
//	Assert( !DrawingShadowDepthView() );

	const CViewSetup *pViewSetup = view->GetViewSetup();
	UpdateScreenEffectTexture( 0, pViewSetup->x, pViewSetup->y, pViewSetup->width, pViewSetup->height);
}

#endif // VIEW_SCENE_H
