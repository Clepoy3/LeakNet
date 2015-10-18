//========= Copyright © 1996-2001, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================

#ifndef GL_RMAIN_H
#define GL_RMAIN_H

#ifdef _WIN32
#pragma once
#endif


#include "vector.h"
#include "mathlib.h"

//-----------------------------------------------------------------------------
// Aspect ratio macros
//-----------------------------------------------------------------------------
#define ASPECT_4BY3			(4.0f/3.0f)
//#define ASPECT_4BY3_HACK	(1360.0f/1024.0f)
#define ASPECT_5BY4			(5.0f/4.0f)
#define ASPECT_16BY9		(16.0f/9.0f)
#define ASPECT_16BY9_HACK	(1366.0f/768.0f)
//#define ASPECT_16BY9_HACK2	(1280.0f/768.0f)
//#define ASPECT_16BY9_HACK3	(1360.0f/768.0f)
#define ASPECT_16BY10		(16.0f/10.0f)
//#define ASPECT_16BY10_HACK	(720.0f/480.0f)

class Frustum_t
{
public:
	void SetPlane( int i, int nType, const Vector &vecNormal, float dist )
	{
		m_Plane[i].normal = vecNormal;
		m_Plane[i].dist = dist;
		m_Plane[i].type = nType;
		m_Plane[i].signbits = SignbitsForPlane( &m_Plane[i] );
		m_AbsNormal[i].Init( fabs(vecNormal.x), fabs(vecNormal.y), fabs(vecNormal.z) );
	}

	inline const cplane_t *GetPlane( int i ) const { return &m_Plane[i]; }
	inline const Vector &GetAbsNormal( int i ) const { return m_AbsNormal[i]; }

private:
	cplane_t	m_Plane[FRUSTUM_NUMPLANES];
	Vector		m_AbsNormal[FRUSTUM_NUMPLANES];
};

extern Frustum_t g_Frustum;


// Cull the world-space bounding box to the specified (4-plane) frustum.
bool R_CullBox( const Vector& mins, const Vector& maxs, const Frustum_t &frustum );

// Cull to the full screen frustum.
inline bool R_CullBox( const Vector& mins, const Vector& maxs )
{
	return R_CullBox( mins, maxs, g_Frustum );
}

// Draw a rectangle in screenspace. The screen window is (-1,-1) to (1,1).
void R_DrawScreenRect( float left, float top, float right, float bottom );

void R_DrawPortals();


#endif // GL_RMAIN_H
