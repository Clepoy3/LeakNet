//========= Copyright © 1996-2001, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================
#include "cbase.h"
#include "c_sprite.h"
#include "model_types.h"
#include "iviewrender.h"
#include "view.h"
#include "enginesprite.h"
#include "engine/ivmodelinfo.h"
#include "tier0/vprof.h"

void DrawSpriteModel( IClientEntity *baseentity, CEngineSprite *psprite, const Vector &origin, float fscale, float frame, 
	int rendermode, int r, int g, int b, int a, const Vector& forward, const Vector& right, const Vector& up );

// VXP: Was 0, but then sprites shows up even behind the walls
//static ConVar	r_traceglow( "r_traceglow","1");
ConVar	r_drawsprites( "r_drawsprites", "1" );

class CTraceFilterGlow : public CTraceFilterSimple
{
public:
	DECLARE_CLASS( CTraceFilterGlow, CTraceFilterSimple );
	
	CTraceFilterGlow( const IHandleEntity *passentity, int collisionGroup ) : CTraceFilterSimple(passentity, collisionGroup) {}
	virtual bool ShouldHitEntity( IHandleEntity *pHandleEntity, int contentsMask )
	{
	//	IClientUnknown *pUnk = (IClientUnknown*)pHandleEntity;
	//	ICollideable *pCollide = pUnk->GetCollideable();
		Assert( dynamic_cast<CBaseEntity*>(pHandleEntity) );
		CBaseEntity *pTestEntity = static_cast<CBaseEntity*>(pHandleEntity);
		if ( pTestEntity->GetSolid() != SOLID_VPHYSICS && pTestEntity->GetSolid() != SOLID_BSP )
			return false;
		return BaseClass::ShouldHitEntity( pHandleEntity, contentsMask );
	}
};
float GlowSightDistance( const Vector &glowOrigin, bool bShouldTrace )
{
#if 0 // VXP: Old shitty sprite traces
	float dist = (glowOrigin - CurrentViewOrigin()).Length();

	int traceFlags = r_traceglow.GetBool() ? (MASK_OPAQUE|CONTENTS_MONSTER|CONTENTS_HITBOX) : MASK_OPAQUE;
	
	trace_t tr;
	UTIL_TraceLine( CurrentViewOrigin(), glowOrigin, traceFlags, NULL, COLLISION_GROUP_NONE, &tr );
	if ( (1.0-tr.fraction) * dist > 8 )
		return -1;

	return dist;

#else // VXP: From Source 2007

	Vector viewOrigin = CurrentViewOrigin();
	float dist = (glowOrigin - viewOrigin).Length();
//	bool bShouldTrace = r_traceglow.GetBool();
	if ( bShouldTrace )
	{
		Vector end = glowOrigin;
		// VXP: HACKHACK: trace 4" from destination in case the glow is inside some parent object
		//			allow a little error...
		if ( dist > 4 )
		{
			end -= CurrentViewForward()*4;
		}
		int traceFlags = (MASK_OPAQUE|CONTENTS_MONSTER|CONTENTS_HITBOX);

		CTraceFilterGlow filter(NULL, COLLISION_GROUP_NONE);
		trace_t tr;
		UTIL_TraceLine( viewOrigin, end, traceFlags, &filter, &tr );
		if ( tr.fraction != 1.0f )
			return -1;
	}

	return dist;
#endif
}

//-----------------------------------------------------------------------------
// Purpose: Determine glow brightness/scale based on distance to render origin and trace results
// Input  : entorigin - 
//			rendermode - 
//			renderfx - 
//			alpha - 
//			pscale - Pointer to the value for scale, will be changed based on distance and rendermode.
//-----------------------------------------------------------------------------
#if 0
float C_SpriteRenderer::GlowBlend( Vector& entorigin, int rendermode, int renderfx, int alpha, float *pscale )
{
	float dist = GlowSightDistance( entorigin );
	if ( dist <= 0 )
		return 0;

	if ( renderfx == kRenderFxNoDissipation )
	{
		return (float)alpha * (1.0f/255.0f);
	}

	// UNDONE: Tweak these magic numbers (19000 - falloff & 200 - sprite size)
	float brightness = 19000.0 / (dist*dist);
	if ( brightness < 0.05 )
	{
		brightness = 0.05;
	}
	else if ( brightness > 1.0 )
	{
		brightness = 1.0;
	}

	if (rendermode != kRenderWorldGlow)
	{
		// Make the glow fixed size in screen space, taking into consideration the scale setting.
		if (*pscale == 0)
		{
			*pscale = 1;
		}

		*pscale *= dist * (1.0/200.0);
	}

	return brightness;
}
#else
float C_SpriteRenderer::GlowBlend( Vector& entorigin, int rendermode, int renderfx, int alpha, float *pscale )
{
	float dist;
	float brightness;

	brightness = GlowSightDistance( entorigin, true ) > 0.0f ? 1.0f : 0.0f;
	if ( brightness <= 0.0f )
	{
		return 0.0f;
	}

	// VXP: Prevents from fluctuating sprite's brightness at DrawSprite()
	// Already fixed by "... > 0.0f ? 1.0f : 0.0f" statement a few lines above
//	brightness = clamp( brightness, 0.05f, 1.0f );

	dist = GlowSightDistance( entorigin, false );
	if ( dist <= 0.0f )
	{
		return 0.0f;
	}

	if ( renderfx == kRenderFxNoDissipation )
	{
	//	return (float)alpha * (1.0f/255.0f);
		return (float)alpha * (1.0f/255.0f) * brightness;
	}

/*
	// UNDONE: Tweak these magic numbers (19000 - falloff & 200 - sprite size)
	float brightness = 19000.0 / (dist*dist);
	if ( brightness < 0.05 )
	{
		brightness = 0.05;
	}
	else if ( brightness > 1.0 )
	{
		brightness = 1.0;
	}
*/

	// UNDONE: Tweak these magic numbers (1200 - distance at full brightness)
	float fadeOut = (1200.0f*1200.0f) / (dist*dist);
	fadeOut = clamp( fadeOut, 0.0f, 1.0f );

	if (rendermode != kRenderWorldGlow)
	{
		// Make the glow fixed size in screen space, taking into consideration the scale setting.
		if (*pscale == 0.0f)
		{
			*pscale = 1.0f;
		}

		*pscale *= dist * (1.0f/200.0f);
	}

	return fadeOut * brightness;
}
#endif


//-----------------------------------------------------------------------------
// Purpose: Determine sprite orientation axes
// Input  : type - 
//			forward - 
//			right - 
//			up - 
//-----------------------------------------------------------------------------
void C_SpriteRenderer::GetSpriteAxes( SPRITETYPE type, 
	const Vector& origin,
	const QAngle& angles,
	Vector& forward, 
	Vector& right, 
	Vector& up )
{
	int				i;
	float			dot, angle, sr, cr;
	Vector			tvec;

	// Automatically roll parallel sprites if requested
	if ( angles[2] != 0 && type == SPR_VP_PARALLEL )
	{
		type = SPR_VP_PARALLEL_ORIENTED;
	}

	switch( type )
	{
	case SPR_FACING_UPRIGHT:
		{
			// generate the sprite's axes, with vup straight up in worldspace, and
			// r_spritedesc.vright perpendicular to modelorg.
			// This will not work if the view direction is very close to straight up or
			// down, because the cross product will be between two nearly parallel
			// vectors and starts to approach an undefined state, so we don't draw if
			// the two vectors are less than 1 degree apart
			tvec[0] = -origin[0];
			tvec[1] = -origin[1];
			tvec[2] = -origin[2];
			VectorNormalize (tvec);
			dot = tvec[2];	// same as DotProduct (tvec, r_spritedesc.vup) because
			//  r_spritedesc.vup is 0, 0, 1
			if ((dot > 0.999848f) || (dot < -0.999848f))	// cos(1 degree) = 0.999848
				return;
			up[0] = 0;
			up[1] = 0;
			up[2] = 1;
			right[0] = tvec[1];
			// CrossProduct(r_spritedesc.vup, -modelorg,
			right[1] = -tvec[0];
			//              r_spritedesc.vright)
			right[2] = 0;
			VectorNormalize (right);
			forward[0] = -right[1];
			forward[1] = right[0];
			forward[2] = 0;
			// CrossProduct (r_spritedesc.vright, r_spritedesc.vup,
			//  r_spritedesc.vpn)
		}
		break;
	case SPR_VP_PARALLEL:
		{
			// generate the sprite's axes, completely parallel to the viewplane. There
			// are no problem situations, because the sprite is always in the same
			// position relative to the viewer
			for (i=0 ; i<3 ; i++)
			{
				up[i]		= CurrentViewUp()[i];
				right[i]	= CurrentViewRight()[i];
				forward[i]	= CurrentViewForward()[i];
			}
		}
		break;
	case SPR_VP_PARALLEL_UPRIGHT:
		{
			// generate the sprite's axes, with g_vecVUp straight up in worldspace, and
			// r_spritedesc.vright parallel to the viewplane.
			// This will not work if the view direction is very close to straight up or
			// down, because the cross product will be between two nearly parallel
			// vectors and starts to approach an undefined state, so we don't draw if
			// the two vectors are less than 1 degree apart
			dot = CurrentViewForward()[2];	// same as DotProduct (vpn, r_spritedesc.g_vecVUp) because
			//  r_spritedesc.vup is 0, 0, 1
			if ((dot > 0.999848f) || (dot < -0.999848f))	// cos(1 degree) = 0.999848
				return;
			up[0] = 0;
			up[1] = 0;
			up[2] = 1;
			right[0] = CurrentViewForward()[1];
			// CrossProduct (r_spritedesc.vup, vpn,
			right[1] = -CurrentViewForward()[0];	//  r_spritedesc.vright)
			right[2] = 0;
			VectorNormalize (right);
			forward[0] = -right[1];
			forward[1] = right[0];
			forward[2] = 0;
			// CrossProduct (r_spritedesc.vright, r_spritedesc.vup,
			//  r_spritedesc.vpn)
		}
		break;
	case SPR_ORIENTED:
		{
			// generate the sprite's axes, according to the sprite's world orientation
			AngleVectors( angles, &forward, &right, &up );
		}
		break;
		
	case SPR_VP_PARALLEL_ORIENTED:
		{
			// generate the sprite's axes, parallel to the viewplane, but rotated in
			// that plane around the center according to the sprite entity's roll
			// angle. So vpn stays the same, but vright and vup rotate
			angle = angles[ROLL] * (M_PI*2.0f / 360.0f);
			SinCos( angle, &sr, &cr );
			
			for (i=0 ; i<3 ; i++)
			{
				forward[i] = CurrentViewForward()[i];
				right[i] = CurrentViewRight()[i] * cr + CurrentViewUp()[i] * sr;
				up[i] = CurrentViewRight()[i] * -sr + CurrentViewUp()[i] * cr;
			}
		}
		break;
	default:
		Warning( "GetSpriteAxes: Bad sprite type %d\n", type );
		break;
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
// Output : int
//-----------------------------------------------------------------------------
int C_SpriteRenderer::DrawSprite( 
	IClientEntity *entity,
	const model_t *model, 
	const Vector& origin, 
	const QAngle& angles,
	float frame,
	IClientEntity *attachedto,
	int attachmentindex,
	int rendermode,
	int renderfx,
	int alpha,
	int r, 
	int g, 
	int b,
	float scale
	)
{
	VPROF_BUDGET( "C_SpriteRenderer::DrawSprite", VPROF_BUDGETGROUP_PARTICLE_RENDERING );
	if( !r_drawsprites.GetBool() || !model || modelinfo->GetModelType( model ) != mod_sprite )
	{
		return 0;
	}

	// Get extra data
	CEngineSprite *psprite = (CEngineSprite *)modelinfo->GetModelExtraData( model );
	if ( !psprite )
	{
		return 0;
	}

	Vector		effect_origin;
	VectorCopy( origin, effect_origin );

	// Use attachment point
	if ( attachedto )
	{
		C_BaseEntity *ent = attachedto->GetBaseEntity();
		if ( ent )
		{
			QAngle temp;
			ent->GetAttachment( attachmentindex, effect_origin, temp );
		}
	}

	if ( rendermode != kRenderNormal )
	{
		float blend = render->GetBlend();

		// kRenderGlow and kRenderWorldGlow have a special blending function
		if (( rendermode == kRenderGlow ) || ( rendermode == kRenderWorldGlow ))
		{
			blend *= GlowBlend( effect_origin, rendermode, renderfx, alpha, &scale );

			//Fade out the sprite depending on distance from the view origin.
			alpha *= blend;
		//	r *= blend;
		//	g *= blend;
		//	b *= blend;
		}

		render->SetBlend( blend );
		if ( blend <= 0.0f )
		{
			return 0;
		}
	}

	// Get orthonormal bases
	Vector forward, right, up;
	GetSpriteAxes( (SPRITETYPE)psprite->GetOrientation(), origin, angles, forward, right, up );
	
	// Draw
	DrawSpriteModel( 
		entity,
		psprite, 
		effect_origin,
		scale,
		frame, 
		rendermode, 
		r, 
		g, 
		b,
		alpha, 
		forward, right ,up );

	return 1;
}
