//====== Copyright © 2008-2009, Axel Project Team, all rights reserved ======//
// 
// Purpose: Studio models header file
//
//===========================================================================//

#ifndef STUDIO_H
#define STUDIO_H

#ifdef _WIN32
#	pragma once
#endif

#include "basetypes.h"
#include "vector2d.h"
#include "vector.h"
#include "vector4d.h"
#include "tier0/dbg.h"
#include "mathlib.h"
#include "utlvector.h"
#include "bspflags.h"

//-----------------------------------------------------------------------------
// Forward declarations
//-----------------------------------------------------------------------------
class IMaterial;
class IMesh;

namespace OptimizedModel
{
	struct StripHeader_t;
}

//-----------------------------------------------------------------------------
// Macros
//-----------------------------------------------------------------------------
#define STUDIO_VERSION 37

#if STUDIO_VERSION > 37
#	error "Unsupported STUDIO_VERSION. When updating STUDIO_VERSION please search for where it is referenced via macros as updating will break many version-specific functions!"
#endif

//=============================================================================
// Max. macros
//=============================================================================
#define MAXSTUDIOTRIANGLES	25000
#define MAXSTUDIOVERTS		25000
#define MAXSTUDIOSKINS		32		// total textures
#define MAXSTUDIOBONES		128		// total bones actually used
#define MAXSTUDIOBONEBITS	7		// this must be base-2 logarithm of MAXSTUDIOBONES
#define MAXSTUDIOBLENDS		32
#define MAXSTUDIOFLEXDESC	1024	// maximum number of low level flexes (actual morph targets)
#define MAXSTUDIOFLEXCTRL	64		// maximum number of flexcontrollers (input sliders)
#define MAXSTUDIOPOSEPARAM	24
#define MAXSTUDIOBONECTRLS	4
#define MAX_NUM_LODS		8

//=============================================================================
// Interpolation macros
//=============================================================================
#define STUDIO_PROC_AXISINTERP	1
#define STUDIO_PROC_QUATINTERP	2

//=============================================================================
// Bone macros
//=============================================================================
#define BONE_CALCULATE_MASK			0x1F
#define BONE_PHYSICALLY_SIMULATED	0x01	// bone is physically simulated when physics are active
#define BONE_PHYSICS_PROCEDURAL		0x02	// procedural when physics is active
#define BONE_ALWAYS_PROCEDURAL		0x04	// bone is always procedurally animated
#define BONE_SCREEN_ALIGN_SPHERE	0x08	// bone aligns to the screen, not constrained in motion.
#define BONE_SCREEN_ALIGN_CYLINDER	0x10	// bone aligns to the screen, constrained by it's own axis.

#define BONE_USED_MASK				0x0003FF00
#define BONE_USED_BY_ANYTHING		0x0003FF00
#define BONE_USED_BY_HITBOX			0x00000100	// bone (or child) is used by a hit box
#define BONE_USED_BY_ATTACHMENT		0x00000200	// bone (or child) is used by an attachment point
#define BONE_USED_BY_VERTEX_MASK	0x0003FC00
#define BONE_USED_BY_VERTEX_LOD0	0x00000400	// bone (or child) is used by the toplevel model via skinned vertex
#define BONE_USED_BY_VERTEX_LOD1	0x00000800
#define BONE_USED_BY_VERTEX_LOD2	0x00001000
#define BONE_USED_BY_VERTEX_LOD3	0x00002000
#define BONE_USED_BY_VERTEX_LOD4	0x00004000
#define BONE_USED_BY_VERTEX_LOD5	0x00008000
#define BONE_USED_BY_VERTEX_LOD6	0x00010000
#define BONE_USED_BY_VERTEX_LOD7	0x00020000
#define BONE_USED_BY_BONE_MERGE		0x00040000	// bone is available for bone merge to occur against it

#define BONE_USED_BY_VERTEX_AT_LOD(lod) ( BONE_USED_BY_VERTEX_LOD0 << (lod) )
#define BONE_USED_BY_ANYTHING_AT_LOD(lod) ( ( BONE_USED_BY_ANYTHING & ~BONE_USED_BY_VERTEX_MASK ) | BONE_USED_BY_VERTEX_AT_LOD(lod) )

#define BONE_TYPE_MASK				0x00F00000
#define BONE_FIXED_ALIGNMENT		0x00100000	// bone can't spin 360 degrees, all interpolation is normalized around a fixed orientation

#define BONE_USED_BY_VERTEX_VERSION_32		0x0400	// bone (or child) is used by skinned vertex
#define BONE_USED_BY_VERTEX_LOD2_VERSION_32 0x0800	// ???? There will be N of these, maybe the LOD info returns the mask??
#define BONE_USED_BY_VERTEX_LOD3_VERSION_32 0x1000  // FIXME: these are currently unassigned....
#define BONE_USED_BY_VERTEX_LOD4_VERSION_32	0x2000
#define BONE_FIXED_ALIGNMENT_VERSION_32		0x10000	// bone can't spin 360 degrees, all interpolation is normalized around a fixed orientation

//=============================================================================
// Inverse kinematic macros
//=============================================================================
#define IK_SELF 1
#define IK_WORLD 2
#define IK_GROUND 3
#define IK_RELEASE 4

//=============================================================================
// Animation flag macros
//=============================================================================
#define STUDIO_POS_ANIMATED		0x0001
#define STUDIO_ROT_ANIMATED		0x0002

//=============================================================================
// Flag macros
//=============================================================================
// This flag is set if no hitbox information was specified
#define STUDIOHDR_FLAGS_AUTOGENERATED_HITBOX	( 1 << 0 )

// NOTE: This flag is set at load time, not MDL build time so that we don't have
//		 to rebuild models when we change materials.
#define STUDIOHDR_FLAGS_USES_ENV_CUBEMAP	( 1 << 1 )

// Use this when there are translucent parts to the model but we're not going
// to sort it 
#define STUDIOHDR_FLAGS_FORCE_OPAQUE		( 1 << 2 )

// Use this when we want to render the opaque parts during the opaque pass
// and the translucent parts during the translucent pass
#define STUDIOHDR_FLAGS_TRANSLUCENT_TWOPASS	( 1 << 3 )

// This is set any time the QC files has $staticprop in it
// Means there's no bones and no transforms
#define STUDIOHDR_FLAGS_STATIC_PROP		( 1 << 4 )

// NOTE: This flag is set at load time, not MDL build time so that we don't
//		 have to rebuild models when we change materials.
#define STUDIOHDR_FLAGS_USES_FB_TEXTURE		( 1 << 5 )

// This flag is set by studiomdl.exe if a separate "$shadowlod" entry was
// present for the .mdl (the shadow lod is the last entry in the lod list if
// present)
#define STUDIOHDR_FLAGS_HASSHADOWLOD		( 1 << 6 )

// NOTE: This flag is set at load time, not MDL build time so that we don't
//		 have to rebuild models when we change materials.
#define STUDIOHDR_FLAGS_USES_BUMPMAPPING	( 1 << 7 )

// NOTE: This flag is set when we should use the actual materials on the
//		 shadow LOD instead of overriding them with the default one (necessary
//		 for translucent shadows)
#define STUDIOHDR_FLAGS_USE_SHADOWLOD_MATERIALS	( 1 << 8 )

//=============================================================================
// Op-code macros
//=============================================================================
#define STUDIO_CONST	1	// get float
#define STUDIO_FETCH1	2	// get flex controller value
#define STUDIO_FETCH2	3	// get flex weight
#define STUDIO_ADD		4
#define STUDIO_SUB		5
#define STUDIO_MUL		6
#define STUDIO_DIV		7
#define STUDIO_NEG		8	// not implemented
#define STUDIO_EXP		9	// not implemented
#define STUDIO_OPEN		10	// only used in token parsing
#define STUDIO_CLOSE	11

//=============================================================================
// Motion flag macros
//=============================================================================
#define STUDIO_X	0x00000001
#define STUDIO_Y	0x00000002
#define STUDIO_Z	0x00000004
#define STUDIO_XR	0x00000008
#define STUDIO_YR	0x00000010
#define STUDIO_ZR	0x00000020

#define STUDIO_LX	0x00000040
#define STUDIO_LY	0x00000080
#define STUDIO_LZ	0x00000100
#define STUDIO_LXR	0x00000200
#define STUDIO_LYR	0x00000400
#define STUDIO_LZR	0x00000800

#define STUDIO_TYPES	0x0003FFFF
#define STUDIO_RLOOP	0x00040000	// controller that wraps shortest distance

//=============================================================================
// Sequence flag macros
//=============================================================================
#define STUDIO_LOOPING	0x0001		// ending frame should be the same as the starting frame
#define STUDIO_SNAP		0x0002		// do not interpolate between previous animation and this one
#define STUDIO_DELTA	0x0004		// this sequence "adds" to the base sequences, not slerp blends
#define STUDIO_AUTOPLAY	0x0008		// temporary flag that forces the sequence to always play
#define STUDIO_POST		0x0010		// 
#define STUDIO_ALLZEROS	0x0020		// this animation/sequence has no real animation data
#define STUDIO_WEIGHT	0x0040		// 
#define STUDIO_SPLINE	0x0080		// 
#define STUDIO_REALTIME	0x0100		// cycle index is taken from a real-time clock, not the animations cycle index

#define STUDIO_HIDDEN	0x0400		// VXP: don't show in default selection views

//-----------------------------------------------------------------------------
// Studio model structures
//-----------------------------------------------------------------------------
struct mstudiodata_t
{
	int	count;
	int	offset;
};

struct mstudioaxisinterpbone_t
{
	int		control; // local transformation of this bone used to calc 3 point blend
	int		axis;	 // axis to check
	Vector		pos[6];	 // X+, X-, Y+, Y-, Z+, Z-
	Quaternion	quat[6]; // X+, X-, Y+, Y-, Z+, Z-
};


struct mstudioquatinterpinfo_t
{
	float		inv_tolerance;	// 1 / radian angle of trigger influence
	Quaternion	trigger;	// angle to match
	Vector		pos;		// new position
	Quaternion	quat;		// new angle
};

struct mstudioquatinterpbone_t
{
	int				control;// local transformation to check
	int				numtriggers;
	int				triggerindex;
	inline mstudioquatinterpinfo_t *pTrigger( int i ) const { return  (mstudioquatinterpinfo_t *)(((byte *)this) + triggerindex) + i; };
};

// bones
struct mstudiobone_t
{
	int		sznameindex;
	inline char * const pszName( void ) const { return ((char *)this) + sznameindex; }
	int		parent;		// parent bone
	int		bonecontroller[6];	// bone controller index, -1 == none
	// FIXME: remove the damn default value fields and put in pos
	float		value[6];	// default DoF values
	float		scale[6];   // scale for delta DoF values
	matrix3x4_t	poseToBone;
	Quaternion	qAlignment;
	int		flags;
	int		proctype;
	int		procindex;	// procedural rule
	mutable int	physicsbone;	// index into physically simulated bone
	inline void *pProcedure( ) const { if (procindex == 0) return NULL; else return  (void *)(((byte *)this) + procindex); };
	int		surfacepropidx;	// index into string table for property name
	inline char * const pszSurfaceProp( void ) const { return ((char *)this) + surfacepropidx; }
	Quaternion	quat;
	int		contents;		// See BSPFlags.h for the contents flags
	int		unused[3];		// remove as appropriate
};

// bone controllers
struct mstudiobonecontroller_t
{
	int	bone;	// -1 == 0
	int	type;	// X, Y, Z, XR, YR, ZR, M
	float	start;
	float	end;
	int	rest;	// byte index value at rest
	int	inputfield;	// 0-3 user set controller, 4 mouth
	char	padding[32];	// future expansion.
};

// intersection boxes
struct mstudiobbox_t
{
	int	bone;
	int	group;			// intersection group
	Vector	bbmin;			// bounding box
	Vector	bbmax;	
	int	szhitboxnameindex;	// offset to the name of the hitbox.
	char	padding[32];		// future expansion.

	char* pszHitboxName()
	{
		if( szhitboxnameindex == 0 )
			return "";

		return ((char*)this) + szhitboxnameindex;
	}
};

// demand loaded sequence groups
enum
{
	Cache_Invalid = -1,
	Cache_NeedsLoading,
};

struct mstudioseqgroup_t
{
	int	szlabelindex;	// textual name
	inline char * const pszLabel( void ) const { return ((char *)this) + szlabelindex; }

	int	sznameindex;	// file name
	inline char * const pszName( void ) const { return ((char *)this) + sznameindex; }
	
	int					cache;			// cache index in the shared model cache
	int					data;			// hack for group 0
};


// events
struct mstudioevent_t
{
	float	cycle;
	int	event;
	int	type;
	char	options[64];
};


// attachment
struct mstudioattachment_t
{
	int		sznameindex;
	inline char * const pszName( void ) const { return ((char *)this) + sznameindex; }
	int		type;
	int		bone;
	matrix3x4_t	local; // attachment point
};

struct mstudioikerror_t
{
	Vector		pos;
	Quaternion	q;
};

struct mstudioikrule_t
{
	int		index;

	int		type;
	int		chain;

	int		bone;

	int			slot;	// iktarget slot.  Usually same as chain.
	float		height;
	float		radius;
	float		floor;
	Vector		pos;
	Quaternion	q;

	float		flWeight;

	int		group; // match sub-sequence IK rules together

	int		iStart;
	int		ikerrorindex;
	inline mstudioikerror_t *pError( int i ) const { return  (mstudioikerror_t *)(((byte *)this) + ikerrorindex) + (i - iStart); };

	float		start;	// beginning of influence
	float		peak;	// start of full influence
	float		tail;	// end of full influence
	float		end;	// end of all influence

	float		commit;		// unused: frame footstep target should be committed
	float		contact;	// unused: frame footstep makes ground concact
	float		pivot;		// unused: frame ankle can begin rotation from latched orientation
	float		release;	// unused: frame ankle should end rotation from latched orientation
};


struct mstudioiklock_t
{
	int		chain;
	float		flPosWeight;
	float		flLocalQWeight;
};

// animation frames
union mstudioanimvalue_t
{
	struct 
	{
		byte	valid;
		byte	total;
	} num;
	short		value;
};

// per bone per animation DOF and weight pointers
struct mstudioanim_t
{
	// float		weight;		// bone influence
	int	flags;		// weighing options
	union
	{
		int	offset[6];	// pointers to animation 
		struct
		{
			float	pos[3];
			float	q[4];
		} pose;
	} u;
	inline mstudioanimvalue_t *pAnimvalue( int i ) const { return  (mstudioanimvalue_t *)(((byte *)this) + u.offset[i]); };
};

struct mstudiomovement_t
{
	int	endframe;				
	int	motionflags;
	float	v0;		// velocity at start of block
	float	v1;		// velocity at end of block
	float	angle;		// YAW rotation at end of this blocks movement
	Vector	vector;		// movement vector relative to this blocks initial angle
	Vector	position;	// relative to start of animation???
};

struct mstudioanimdesc_t
{
	int	sznameindex;
	inline char * const pszName( void ) const { return ((char *)this) + sznameindex; }

	float fps;		// frames per second	
	int	flags;		// looping/non-looping flags

	int	numframes;

	// piecewise movement
	int	nummovements;
	int	movementindex;
	inline mstudiomovement_t * const pMovement( int i ) const { return (mstudiomovement_t *)(((byte *)this) + movementindex) + i; };

	Vector	bbmin;		// per animation bounding box
	Vector	bbmax;		

	int	animindex;	// mstudioanim_t pointer relative to start of mstudioanimdesc_t data
					// [bone][X, Y, Z, XR, YR, ZR]
	inline mstudioanim_t		*pAnim( int i ) const { return  (mstudioanim_t *)(((byte *)this) + animindex) + i; };

	int	numikrules;
	int	ikruleindex;
	inline mstudioikrule_t *pIKRule( int i ) const { return (mstudioikrule_t *)(((byte *)this) + ikruleindex) + i; };

	int	unused[8];	// remove as appropriate
};

struct mstudioikrule_t;

struct mstudioautolayer_t
{
	int	iSequence;
	int	flags;
	float	start;	// beginning of influence
	float	peak;	// start of full influence
	float	tail;	// end of full influence
	float	end;	// end of all influence
};

// sequence descriptions
struct mstudioseqdesc_t
{
	int	szlabelindex;
	inline char * const pszLabel( void ) const { return ((char *)this) + szlabelindex; }

	int	szactivitynameindex;
	inline char * const pszActivityName( void ) const { return ((char *)this) + szactivitynameindex; }

	int	flags;		// looping/non-looping flags

	int	activity;	// initialized at loadtime to game DLL values
	int	actweight;

	int	numevents;
	int	eventindex;
	inline mstudioevent_t *pEvent( int i ) const { return (mstudioevent_t *)(((byte *)this) + eventindex) + i; };

	Vector	bbmin;		// per sequence bounding box
	Vector	bbmax;		

	//-------------------------------------------------------------------------
	// Purpose: returns a model animation from the sequence group size and
	//          blend index
	// Note: this replaces GetAnimValue() that was previously in bone_setup
	// Note: this also acts as a SetAnimValue() as it returns a reference to
	//       the anim value in question
	//-------------------------------------------------------------------------
	inline unsigned short& pAnimValue( int nIndex0, int nIndex1 ) const
	{
		// Clamp indexes
		if ( nIndex0 >= groupsize[0] )
			nIndex0 = groupsize[0] - 1;

		if ( nIndex1 >= groupsize[1] )
			nIndex1 = groupsize[1] - 1;
		
		return *pBlend(nIndex1 * groupsize[0] + nIndex0);
	}

	int	numblends;

	int blendindex;
	inline unsigned short *pBlend( int i ) const { return (unsigned short *)(((byte *)this) + blendindex) + i; };

	int seqgroup; // sequence group for demand loading

	int	groupsize[2];
	int	paramindex[2];	// X, Y, Z, XR, YR, ZR
	float	paramstart[2];	// local (0..1) starting value
	float	paramend[2];	// local (0..1) ending value
	int	paramparent;

	float	fadeintime;	// ideal cross fate in time (0.2 default)
	float	fadeouttime;	// ideal cross fade out time (0.2 default)

	int	entrynode;	// transition node at entry
	int	exitnode;	// transition node at exit
	int	nodeflags;	// transition rules

	float	entryphase;	// used to match entry gait
	float	exitphase;	// used to match exit gait
	
	float	lastframe;	// frame that should generation EndOfSequence

	int	nextseq;	// auto advancing sequences
	int	pose;		// index of delta animation between end and nextseq

	int	numikrules;

	int	numautolayers;
	int	autolayerindex;
	inline mstudioautolayer_t *pAutolayer( int i ) const { return (mstudioautolayer_t *)(((byte *)this) + autolayerindex) + i; };

	int	weightlistindex;
	float	*pBoneweight( int i ) const { return ((float *)(((byte *)this) + weightlistindex) + i); };
	float	weight( int i ) const { return *(pBoneweight( i)); };

	int	posekeyindex;
	float				*pPoseKey( int iParam, int iAnim ) const { return (float *)(((byte *)this) + posekeyindex) + iParam * groupsize[0] + iAnim; }
	float				poseKey( int iParam, int iAnim ) const { return *(pPoseKey( iParam, iAnim )); }

	int	numiklocks;
	int	iklockindex;
	inline mstudioiklock_t *pIKLock( int i ) const { return (mstudioiklock_t *)(((byte *)this) + iklockindex) + i; };

	// Key values
	int	keyvalueindex;
	int	keyvaluesize;
	inline const char * KeyValueText( void ) const { return keyvaluesize != 0 ? ((char *)this) + keyvalueindex : NULL; }
	
	int	unused[3];		// remove/add as appropriate
};

struct mstudioseqdesc_v36_t
{
	int	szlabelindex;
	inline char * const pszLabel( void ) const { return ((char *)this) + szlabelindex; }

	int	szactivitynameindex;
	inline char * const pszActivityName( void ) const { return ((char *)this) + szactivitynameindex; }

	int	flags;		// looping/non-looping flags

	int	activity;	// initialized at loadtime to game DLL values
	int	actweight;

	int	numevents;
	int	eventindex;
	inline mstudioevent_t *pEvent( int i ) const { return (mstudioevent_t *)(((byte *)this) + eventindex) + i; };

	Vector	bbmin;		// per sequence bounding box
	Vector	bbmax;		

	//-------------------------------------------------------------------------
	// Purpose: returns a model animation from the sequence group size and
	//          blend index
	// Note: this replaces GetAnimValue() that was previously in bone_setup
	// Note: this also acts as a SetAnimValue() as it returns a reference to
	//       the anim value in question
	//-------------------------------------------------------------------------
	inline int& pAnimValue( int nIndex0, int nIndex1 )
	{
		return anim[nIndex0][nIndex1];
	}

	int	numblends;

	int anim[MAXSTUDIOBLENDS][MAXSTUDIOBLENDS]; // animation number

	int blendindex; // [blend] float array for blended movement
	inline unsigned short *pBlend( int i ) const { return (unsigned short *)(((byte *)this) + blendindex) + i; };

	int	groupsize[2];
	int	paramindex[2];	// X, Y, Z, XR, YR, ZR
	float	paramstart[2];	// local (0..1) starting value
	float	paramend[2];	// local (0..1) ending value
	int	paramparent;

	int	seqgroup;	// sequence group for demand loading

	float	fadeintime;	// ideal cross fate in time (0.2 default)
	float	fadeouttime;	// ideal cross fade out time (0.2 default)

	int	entrynode;	// transition node at entry
	int	exitnode;	// transition node at exit
	int	nodeflags;	// transition rules

	float	entryphase;	// used to match entry gait
	float	exitphase;	// used to match exit gait

	float	lastframe;	// frame that should generation EndOfSequence

	int	nextseq;	// auto advancing sequences
	int	pose;		// index of delta animation between end and nextseq

	int	numikrules;

	int	numautolayers;
	int	autolayerindex;
	inline mstudioautolayer_t *pAutolayer( int i ) const { return (mstudioautolayer_t *)(((byte *)this) + autolayerindex) + i; };

	int	weightlistindex;
	float	*pBoneweight( int i ) const { return ((float *)(((byte *)this) + weightlistindex) + i); };
	float	weight( int i ) const { return *(pBoneweight( i)); };

	int	posekeyindex;
	float				*pPoseKey( int iParam, int iAnim ) const { return (float *)(((byte *)this) + posekeyindex) + iParam * groupsize[0] + iAnim; }
	float				poseKey( int iParam, int iAnim ) const { return *(pPoseKey( iParam, iAnim )); }

	int	numiklocks;
	int	iklockindex;
	inline mstudioiklock_t *pIKLock( int i ) const { return (mstudioiklock_t *)(((byte *)this) + iklockindex) + i; };

	// Key values
	int	keyvalueindex;
	int	keyvaluesize;
	inline const char * KeyValueText( void ) const { return keyvaluesize != 0 ? ((char *)this) + keyvalueindex : NULL; }

	int	unused[3];		// remove/add as appropriate
};


struct mstudioposeparamdesc_t
{
	int	sznameindex;
	inline char * const pszName( void ) const { return ((char *)this) + sznameindex; }
	int	flags;	// ????
	float	start;	// starting value
	float	end;	// ending value
	float	loop;	// looping range, 0 for no looping, 360 for rotations, etc.
};

struct mstudioflexdesc_t
{
	int	szFACSindex;
	inline char * const pszFACS( void ) const { return ((char *)this) + szFACSindex; }
};


struct mstudioflexcontroller_t
{
	int	sztypeindex;
	inline char * const pszType( void ) const { return ((char *)this) + sztypeindex; }
	int	sznameindex;
	inline char * const pszName( void ) const { return ((char *)this) + sznameindex; }
	mutable int  link;	// remapped at load time to master list
	float	min;
	float	max;
};

struct mstudiovertanim_t
{
	int	index;
	Vector	delta;
	Vector	ndelta;
};


struct mstudioflex_t
{
	int	flexdesc;	// input value

	float	target0;	// zero
	float	target1;	// one
	float	target2;	// one
	float	target3;	// zero

	int	numverts;
	int	vertindex;
	inline	mstudiovertanim_t *pVertanim( int i ) const { return  (mstudiovertanim_t *)(((byte *)this) + vertindex) + i; };
};


struct mstudioflexop_t
{
	int	op;
	union 
	{
		int	index;
		float	value;
	} d;
};

struct mstudioflexrule_t
{
	int	flex;
	int	numops;
	int	opindex;
	inline mstudioflexop_t *iFlexOp( int i ) const { return  (mstudioflexop_t *)(((byte *)this) + opindex) + i; };
};

// 16 bytes
struct mstudioboneweight_t
{
	float	weight[4];
	short	bone[4]; 

	short	numbones;
	short	material;

	short	firstref;
	short	lastref;
};

// NOTE: This is exactly 64 bytes, two cache lines
struct mstudiovertex_t
{
	mstudioboneweight_t	m_BoneWeights;
	Vector			m_vecPosition;
	Vector			m_vecNormal;
	Vector2D		m_vecTexCoord;
};

// skin info
struct mstudiotexture_t
{
	int	sznameindex;
	inline char * const		pszName( void ) const { return ((char *)this) + sznameindex; }
	int	flags;
	float	width;		// portion used
	float	height;		// portion used
	mutable IMaterial  *material;  // fixme: this needs to go away . .isn't used by the engine, but is used by studiomdl
	mutable void	   *clientmaterial;	// gary, replace with client material pointer if used
	float	dPdu;		// world units per u
	float	dPdv;		// world units per v
};

// eyeball
struct mstudioeyeball_t
{
	int	sznameindex;
	inline char * const pszName( void ) const { return ((char *)this) + sznameindex; }
	int	bone;
	Vector	org;
	float	zoffset;
	float	radius;
	Vector	up;
	Vector	forward;
	int	texture;

	int	iris_material;
	float	iris_scale;
	int	glint_material;	// !!!

	int	upperflexdesc[3];	// index of raiser, neutral, and lowerer flexdesc that is set by flex controllers
	int	lowerflexdesc[3];
	float	uppertarget[3];		// angle (radians) of raised, neutral, and lowered lid positions
	float	lowertarget[3];
	//int		upperflex;	// index of actual flex
	//int		lowerflex;

	int	upperlidflexdesc;	// index of flex desc that actual lid flexes look to
	int	lowerlidflexdesc;

	float	pitch[2];	// min/max pitch
	float	yaw[2];		// min/max yaw
};


// ikinfo
struct mstudioiklink_t
{
	int	bone;
	Vector	contact;
	Vector	limits;
};

struct mstudioikchain_t
{
	int	sznameindex;
	inline char * const pszName( void ) const { return ((char *)this) + sznameindex; }
	int	linktype;
	int	numlinks;
	int	linkindex;
	inline mstudioiklink_t *pLink( int i ) const { return (mstudioiklink_t *)(((byte *)this) + linkindex) + i; };
};


struct mstudioiface_t
{
	unsigned short	a, b, c;		// Indices to vertices
};


struct mstudiomodel_t;

struct mstudiomesh_t
{
	int		material;

	int		modelindex;
	mstudiomodel_t *pModel() const; // { return (mstudiomodel_t *)(((byte *)this) + modelindex); }

	int		numvertices;		// number of unique vertices/normals/texcoords
	int		vertexoffset;		// vertex mstudiovertex_t

	Vector		*Position( int i ) const;
	Vector		*Normal( int i ) const;
	Vector4D	*TangentS( int i ) const;
	Vector2D	*Texcoord( int i ) const;
	mstudioboneweight_t 	*BoneWeights( int i ) const;
	mstudiovertex_t	 	*Vertex( int i ) const;

	int		numflexes;			// vertex animation
	int		flexindex;
	inline mstudioflex_t *pFlex( int i ) const { return (mstudioflex_t *)(((byte *)this) + flexindex) + i; };

	//int		numresolutionupdates;
	//int		resolutionupdateindex;

	//int		numfaceupdates;
	//int		faceupdateindex;

	// special codes for material operations
	int		materialtype;
	int		materialparam;

	// a unique ordinal for this mesh
	int		meshid;

	Vector		center;

	int		unused[5]; // remove as appropriate
};

// studio models
struct mstudiomodel_t
{
	char		name[64];

	int		type;

	float		boundingradius;

	int		nummeshes;
	int		meshindex;
	inline mstudiomesh_t *pMesh( int i ) const { return (mstudiomesh_t *)(((byte *)this) + meshindex) + i; };

	// cache purposes
	int		numvertices;		// number of unique vertices/normals/texcoords
	int		vertexindex;		// vertex Vector
	int		tangentsindex;		// tangents Vector

	Vector		*Position( int i ) const;
	Vector		*Normal( int i ) const;
	Vector4D	*TangentS( int i ) const;
	Vector2D	*Texcoord( int i ) const;
	mstudioboneweight_t 	*BoneWeights( int i ) const;
	mstudiovertex_t		*Vertex( int i ) const;

	int		numattachments;
	int		attachmentindex;

	int		numeyeballs;
	int		eyeballindex;
	inline  mstudioeyeball_t *pEyeball( int i ) { return (mstudioeyeball_t *)(((byte *)this) + eyeballindex) + i; };

	int		unused[8];		// remove as appropriate
};


inline mstudiovertex_t *mstudiomodel_t::Vertex( int i ) const 
{ 
	return (mstudiovertex_t *)(((byte *)this) + vertexindex) + i; 
}

inline Vector *mstudiomodel_t::Position( int i ) const 
{
	return &Vertex(i)->m_vecPosition;
}

inline Vector *mstudiomodel_t::Normal( int i ) const 
{ 
	return &Vertex(i)->m_vecNormal;
}

inline Vector4D *mstudiomodel_t::TangentS( int i ) const 
{
	// NOTE: The tangents vector is in a separate array
	// because it only exists on the high end, and if I leave it out
	// of the mstudiovertex_t, the vertex is 64-bytes (good for low end)
	return (Vector4D *)(((byte *)this) + tangentsindex) + i; 
}

inline Vector2D *mstudiomodel_t::Texcoord( int i ) const 
{ 
	return &Vertex(i)->m_vecTexCoord;
}

inline mstudioboneweight_t *mstudiomodel_t::BoneWeights( int i ) const 
{
	return &Vertex(i)->m_BoneWeights;
}

inline mstudiomodel_t *mstudiomesh_t::pModel() const 
{ 
	return (mstudiomodel_t *)(((byte *)this) + modelindex); 
}

inline Vector *mstudiomesh_t::Position( int i ) const 
{ 
	return pModel()->Position( vertexoffset + i ); 
};

inline Vector *mstudiomesh_t::Normal( int i ) const 
{ 
	return pModel()->Normal( vertexoffset + i ); 
};

inline Vector4D *mstudiomesh_t::TangentS( int i ) const
{
	return pModel()->TangentS( vertexoffset + i ); 
}

inline Vector2D *mstudiomesh_t::Texcoord( int i ) const 
{ 
	return pModel()->Texcoord( vertexoffset + i ); 
};

inline mstudioboneweight_t *mstudiomesh_t::BoneWeights( int i ) const 
{ 
	return pModel()->BoneWeights( vertexoffset + i ); 
};

inline mstudiovertex_t *mstudiomesh_t::Vertex( int i ) const
{
	return pModel()->Vertex( vertexoffset + i );
}

// a group of studio model data
enum studiomeshgroupflags_t
{
	MESHGROUP_IS_FLEXED = 0x1,
	MESHGROUP_IS_HWSKINNED = 0x2
};


//-----------------------------------------------------------------------------
// Runtime stuff
//-----------------------------------------------------------------------------
struct studiomeshgroup_t
{
	IMesh*	m_pMesh;
	IMesh	*m_pColorMesh;
	int	m_NumStrips;
	int	m_Flags;		// see studiomeshgroupflags_t
	OptimizedModel::StripHeader_t*	m_pStripData;
	unsigned short*			m_pGroupIndexToMeshIndex;
	int	m_NumVertices;
	int*	m_pUniqueTris;	// for performance measurements
	unsigned short*	m_pIndices;
	bool	m_MeshNeedsRestore;
	short	m_ColorMeshID;

	inline unsigned short MeshIndex( int i ) const { return m_pGroupIndexToMeshIndex[m_pIndices[i]]; }
};


// studio model data
struct studiomeshdata_t
{
	int			m_NumGroup;
	studiomeshgroup_t*	m_pMeshGroup;
};

struct studioloddata_t
{
	// not needed - this is really the same as studiohwdata_t.m_NumStudioMeshes
	//int			m_NumMeshes; 
	studiomeshdata_t	*m_pMeshData; // there are studiohwdata_t.m_NumStudioMeshes of these.
	float			m_SwitchPoint;
	// one of these for each lod since we can switch to simpler materials on lower lods.
	int			numMaterials; 
	IMaterial		**ppMaterials; /* will have studiohdr_t.numtextures elements allocated */
	// hack - this needs to go away.
	int			*pMaterialFlags; /* will have studiohdr_t.numtextures elements allocated */
};

struct studiohwdata_t
{
	int			m_NumLODs;
	studioloddata_t		*m_pLODs;
	int			m_NumStudioMeshes;
};

// body part index
struct mstudiobodyparts_t
{
	int	sznameindex;
	inline char * const pszName( void ) const { return ((char *)this) + sznameindex; }
	int	nummodels;
	int	base;
	int	modelindex; // index into models array
	inline mstudiomodel_t *pModel( int i ) const { return (mstudiomodel_t *)(((byte *)this) + modelindex) + i; };
};


struct mstudiomouth_t
{
	int	bone;
	Vector	forward;
	int	flexdesc;
};


struct mstudiohitboxset_t
{
	int	sznameindex;
	inline char * const	pszName( void ) const { return ((char *)this) + sznameindex; }
	int	numhitboxes;
	int	hitboxindex;
	inline mstudiobbox_t *pHitbox( int i ) const { return (mstudiobbox_t *)(((byte *)this) + hitboxindex) + i; };
};

#if STUDIO_VERSION == 37
struct mstudioanimgroup_t
{
	int group;	// Sequence group
	int index;	// Animation index
};

struct mstudiobonedesc_t
{
	int 	sznameindex;
	inline  char * const	pszName( void ) const {	return ((char *)this) + sznameindex; };

	int parent;		// parent bone

	// FIXME: remove the damn default value fields and put in pos
	float value[6]; // default DoF values
	float scale[6]; // scale for delta DoF values
	matrix3x4_t poseToBone;

	float fivefloat[5];
//	Quaternion	qAlignment;

//	int		unused[3];		// remove as appropriate
};
#endif

// demand loaded sequence groups
struct mstudiomodelgroup_t
{
	int					szlabelindex;	// textual name
	inline char * const pszLabel( void ) const { return ((char *)this) + szlabelindex; }

	int					sznameindex;	// file name
	inline char * const pszName( void ) const { return ((char *)this) + sznameindex; }
};

struct studiohdr_t
{
	int	id;
	int	version;

	long checksum;	// this has to be the same in the phy and vtx files to load!

	char name[64];
	int	length;

	Vector	eyeposition;	// ideal eye position

	Vector	illumposition;	// illumination center

	Vector	hull_min;	// ideal movement hull size
	Vector	hull_max;			

	Vector	view_bbmin;	// clipping bounding box
	Vector	view_bbmax;	

	int	flags;

	int	numbones;	// bones
	int	boneindex;
	inline mstudiobone_t *pBone( int i ) const { return (mstudiobone_t *)(((byte *)this) + boneindex) + i; };

	int	numbonecontrollers;	// bone controllers
	int	bonecontrollerindex;
	inline mstudiobonecontroller_t *pBonecontroller( int i ) const { return (mstudiobonecontroller_t *)(((byte *)this) + bonecontrollerindex) + i; };

	int	numhitboxsets;
	int	hitboxsetindex; 

	// Look up hitbox set by index
	mstudiohitboxset_t  *pHitboxSet( int i ) const 
	{ 
		return (mstudiohitboxset_t *)(((byte *)this) + hitboxsetindex ) + i; 
	};

	// Calls through to hitbox to determine size of specified set
	inline mstudiobbox_t *pHitbox( int i, int set ) const 
	{ 
		mstudiohitboxset_t const *s = pHitboxSet( set );
		if ( !s )
			return NULL;

		return s->pHitbox( i );
	};

	// Calls through to set to get hitbox count for set
	inline int  iHitboxCount( int set ) const
	{
		mstudiohitboxset_t const *s = pHitboxSet( set );
		if ( !s )
			return 0;

		return s->numhitboxes;
	};

	int	numanim;	// animations/poses
	int	animdescindex;	// animation descriptions
	inline mstudioanimdesc_t *pAnimdesc( int i ) const { return (mstudioanimdesc_t *)(((byte *)this) + animdescindex) + i; };

	int 	numanimgroup;
	int 	animgroupindex;
	inline  mstudioanimgroup_t *pAnimGroup(int i) const	{ return (mstudioanimgroup_t *)(((byte *)this) + animgroupindex) + i; };

	int 	numbonedesc;
	int 	bonedescindex;
	inline  mstudiobonedesc_t *pBoneDesc(int i) const { return (mstudiobonedesc_t *)(((byte *)this) + bonedescindex) + i; };

	int	numseq;		// sequences
	int	seqindex;
	inline mstudioseqdesc_t *pSeqdesc( int i ) const { if (i < 0 || i >= numseq) i = 0; return (mstudioseqdesc_t *)(((byte *)this) + seqindex) + i; };
	int	sequencesindexed;	// initialization flag - have the sequences been indexed?

	int	numseqgroups;		// demand loaded sequences
	int	seqgroupindex;
	inline  mstudioseqgroup_t *pSeqgroup(int i) const { return (mstudioseqgroup_t *)(((byte *)this) + seqgroupindex) + i; };

	int	numtextures;		// raw textures
	int	textureindex;
	inline mstudiotexture_t *pTexture( int i ) const { return (mstudiotexture_t *)(((byte *)this) + textureindex) + i; }; 

	int	numcdtextures;		// raw textures search paths
	int	cdtextureindex;
	inline char			*pCdtexture( int i ) const { return (((char *)this) + *((int *)(((byte *)this) + cdtextureindex) + i)); };

	int	numskinref;		// replaceable textures tables
	int	numskinfamilies;
	int	skinindex;
	inline short		*pSkinref( int i ) const { return (short *)(((byte *)this) + skinindex) + i; };

	int	numbodyparts;		
	int	bodypartindex;
	inline mstudiobodyparts_t	*pBodypart( int i ) const { return (mstudiobodyparts_t *)(((byte *)this) + bodypartindex) + i; };

	int	numattachments;		// queryable attachable points
	int	attachmentindex;
	inline mstudioattachment_t	*pAttachment( int i ) const { return (mstudioattachment_t *)(((byte *)this) + attachmentindex) + i; };

	int	numtransitions;		// animation node to animation node transition graph
	int	transitionindex;
	inline byte	*pTransition( int i ) const { return (byte *)(((byte *)this) + transitionindex) + i; };

	int	numflexdesc;
	int	flexdescindex;
	inline mstudioflexdesc_t *pFlexdesc( int i ) const { return (mstudioflexdesc_t *)(((byte *)this) + flexdescindex) + i; };

	int	numflexcontrollers;
	int	flexcontrollerindex;
	inline mstudioflexcontroller_t *pFlexcontroller( int i ) const { return (mstudioflexcontroller_t *)(((byte *)this) + flexcontrollerindex) + i; };

	int	numflexrules;
	int	flexruleindex;
	inline mstudioflexrule_t *pFlexRule( int i ) const { return (mstudioflexrule_t *)(((byte *)this) + flexruleindex) + i; };

	int	numikchains;
	int	ikchainindex;
	inline mstudioikchain_t *pIKChain( int i ) const { return (mstudioikchain_t *)(((byte *)this) + ikchainindex) + i; };

	int	nummouths;
	int	mouthindex;
	inline mstudiomouth_t *pMouth( int i ) const { return (mstudiomouth_t *)(((byte *)this) + mouthindex) + i; };

	int	numposeparameters;
	int	poseparamindex;
	inline mstudioposeparamdesc_t *pPoseParameter( int i ) const { return (mstudioposeparamdesc_t *)(((byte *)this) + poseparamindex) + i; };

	int	surfacepropindex;
	inline char * const pszSurfaceProp( void ) const { return ((char *)this) + surfacepropindex; }

	// Key values
	int	keyvalueindex;
	int	keyvaluesize;
	inline const char * KeyValueText( void ) const { return keyvaluesize != 0 ? ((char *)this) + keyvalueindex : NULL; }

	int	numikautoplaylocks;
	int	ikautoplaylockindex;
	inline mstudioiklock_t *pIKAutoplayLock( int i ) const { return (mstudioiklock_t *)(((byte *)this) + ikautoplaylockindex) + i; };

	float mass;		// The collision model mass that jay wanted
	int	contents;
	int	unused[9];	// remove as appropriate
};

struct studiohdr_v36_t
{
	int	id;
	int	version;

	long checksum;	// this has to be the same in the phy and vtx files to load!

	char name[64];
	int	length;

	Vector	eyeposition;	// ideal eye position

	Vector	illumposition;	// illumination center

	Vector	hull_min;	// ideal movement hull size
	Vector	hull_max;			

	Vector	view_bbmin;	// clipping bounding box
	Vector	view_bbmax;	

	int	flags;

	int	numbones;	// bones
	int	boneindex;
	inline mstudiobone_t *pBone( int i ) const { return (mstudiobone_t *)(((byte *)this) + boneindex) + i; };

	int	numbonecontrollers;	// bone controllers
	int	bonecontrollerindex;
	inline mstudiobonecontroller_t *pBonecontroller( int i ) const { return (mstudiobonecontroller_t *)(((byte *)this) + bonecontrollerindex) + i; };

	int	numhitboxsets;
	int	hitboxsetindex; 

	// Look up hitbox set by index
	mstudiohitboxset_t  *pHitboxSet( int i ) const 
	{ 
		return (mstudiohitboxset_t *)(((byte *)this) + hitboxsetindex ) + i; 
	};

	// Calls through to hitbox to determine size of specified set
	inline mstudiobbox_t *pHitbox( int i, int set ) const 
	{ 
		mstudiohitboxset_t const *s = pHitboxSet( set );
		if ( !s )
			return NULL;

		return s->pHitbox( i );
	};

	// Calls through to set to get hitbox count for set
	inline int  iHitboxCount( int set ) const
	{
		mstudiohitboxset_t const *s = pHitboxSet( set );
		if ( !s )
			return 0;

		return s->numhitboxes;
	};

	int	numanim;	// animations/poses
	int	animdescindex;	// animation descriptions
	inline mstudioanimdesc_t *pAnimdesc( int i ) const { return (mstudioanimdesc_t *)(((byte *)this) + animdescindex) + i; };

	int	numseq;		// sequences
	int	seqindex;
	inline mstudioseqdesc_v36_t *pSeqdesc( int i ) const { if (i < 0 || i >= numseq) i = 0; return (mstudioseqdesc_v36_t *)(((byte *)this) + seqindex) + i; };
	int	sequencesindexed;	// initialization flag - have the sequences been indexed?

	int	numseqgroups;		// demand loaded sequences
	int	seqgroupindex;
	inline  mstudioseqgroup_t *pSeqgroup(int i) const { return (mstudioseqgroup_t *)(((byte *)this) + seqgroupindex) + i; };

	int	numtextures;		// raw textures
	int	textureindex;
	inline mstudiotexture_t *pTexture( int i ) const { return (mstudiotexture_t *)(((byte *)this) + textureindex) + i; }; 

	int	numcdtextures;		// raw textures search paths
	int	cdtextureindex;
	inline char			*pCdtexture( int i ) const { return (((char *)this) + *((int *)(((byte *)this) + cdtextureindex) + i)); };

	int	numskinref;		// replaceable textures tables
	int	numskinfamilies;
	int	skinindex;
	inline short		*pSkinref( int i ) const { return (short *)(((byte *)this) + skinindex) + i; };

	int	numbodyparts;		
	int	bodypartindex;
	inline mstudiobodyparts_t	*pBodypart( int i ) const { return (mstudiobodyparts_t *)(((byte *)this) + bodypartindex) + i; };

	int	numattachments;		// queryable attachable points
	int	attachmentindex;
	inline mstudioattachment_t	*pAttachment( int i ) const { return (mstudioattachment_t *)(((byte *)this) + attachmentindex) + i; };

	int	numtransitions;		// animation node to animation node transition graph
	int	transitionindex;
	inline byte	*pTransition( int i ) const { return (byte *)(((byte *)this) + transitionindex) + i; };

	int	numflexdesc;
	int	flexdescindex;
	inline mstudioflexdesc_t *pFlexdesc( int i ) const { return (mstudioflexdesc_t *)(((byte *)this) + flexdescindex) + i; };

	int	numflexcontrollers;
	int	flexcontrollerindex;
	inline mstudioflexcontroller_t *pFlexcontroller( int i ) const { return (mstudioflexcontroller_t *)(((byte *)this) + flexcontrollerindex) + i; };

	int	numflexrules;
	int	flexruleindex;
	inline mstudioflexrule_t *pFlexRule( int i ) const { return (mstudioflexrule_t *)(((byte *)this) + flexruleindex) + i; };

	int	numikchains;
	int	ikchainindex;
	inline mstudioikchain_t *pIKChain( int i ) const { return (mstudioikchain_t *)(((byte *)this) + ikchainindex) + i; };

	int	nummouths;
	int	mouthindex;
	inline mstudiomouth_t *pMouth( int i ) const { return (mstudiomouth_t *)(((byte *)this) + mouthindex) + i; };

	int	numposeparameters;
	int	poseparamindex;
	inline mstudioposeparamdesc_t *pPoseParameter( int i ) const { return (mstudioposeparamdesc_t *)(((byte *)this) + poseparamindex) + i; };

	int	surfacepropindex;
	inline char * const pszSurfaceProp( void ) const { return ((char *)this) + surfacepropindex; }

	// Key values
	int	keyvalueindex;
	int	keyvaluesize;
	inline const char * KeyValueText( void ) const { return keyvaluesize != 0 ? ((char *)this) + keyvalueindex : NULL; }

	int	numikautoplaylocks;
	int	ikautoplaylockindex;
	inline mstudioiklock_t *pIKAutoplayLock( int i ) const { return (mstudioiklock_t *)(((byte *)this) + ikautoplaylockindex) + i; };

	float mass;		// The collision model mass that jay wanted
	int	contents;
	int	unused[9];	// remove as appropriate
};

// header for demand loaded sequence group data
struct studioseqhdr_t
{
	int	id;
	int	version;

	char name[64];
	int	length;
};

#if STUDIO_VERSION == 37
// header for shared sequence data (e.g. male_0101.mdl, cs_shared.mdl)
struct studiosharehdr_t
{
	int id;
	int version;

	char name[64];
	int length;

	int data;

	int 	numbonedesc;
	int 	bonedescindex;
	inline  mstudiobonedesc_t *pBoneDesc(int i) const { return (mstudiobonedesc_t *)(((byte *)this) + bonedescindex) + i; };

	int	numanim;		// animations/poses
	int	animdescindex;	// animation descriptions
	inline mstudioanimdesc_t *pAnimdesc( int i ) const { return (mstudioanimdesc_t *)(((byte *)this) + animdescindex) + i; };
};
#endif

struct flexweight_t
{
	int	key;
	float	weight;
	float	influence;
};

//-----------------------------------------------------------------------------
// Purpose: A markov group basically indexes another flex setting and includes
// a weighting factor which is used to factor how likely the specified member
// is to be picked
//-----------------------------------------------------------------------------
struct flexmarkovgroup_t
{
	int	settingnumber;
	int	weight;
};

#define FS_NORMAL 0
#define FS_MARKOV 1

struct flexsetting_t
{
	int	nameindex;

	inline char *pszName( void ) const
	{ 
		return (char *)(((byte *)this) + nameindex); 
	}

	inline bool IsMarkov( void ) const
	{ 
		return type == FS_MARKOV ? true : false; 
	}

	// FS_NORMAL or FS_MARKOV
	int	type;

	// Number of flex settings for FS_NORMAL or Number of 
	//  Markov group members for FS_MARKOV
	int	numsettings;
	int	index;
	
	// For FS_MARKOV only, the client .dll writes the current index into here so that
	//  it can retain which markov group is being followed. This is reset every time the expression
	//  starts to play back during scene playback
	int	currentindex;

	// Index of start of contiguous array of flexweight_t or 
	//  flexmarkovgroup_t structures
	int	settingindex;

	//-----------------------------------------------------------------------------
	// Purpose: Retrieves the specified markov group header if the entry is a markov entry
	// Input  : member - 
	// Output : flexmarkovgroup_t *
	//-----------------------------------------------------------------------------
	inline flexmarkovgroup_t *pMarkovGroup( int member ) const
	{
		// type must be FS_MARKOV to return this pointer
		if ( !IsMarkov() )
			return NULL;

		if ( member < 0 || 
			member >= numsettings )
			return NULL;

		flexmarkovgroup_t *group = ( flexmarkovgroup_t * )(((byte *)this) + settingindex ) + member;
		return group;
	};

	//-----------------------------------------------------------------------------
	// Purpose: 
	// Input  : member - 
	// Output : int
	//-----------------------------------------------------------------------------
	inline int GetMarkovSetting( int member ) const
	{
		flexmarkovgroup_t *group = pMarkovGroup( member );
		if ( !group )
			 return -1;

		return group->settingnumber;
	};

	//-----------------------------------------------------------------------------
	// Purpose: Retrieves a pointer to the flexweight_t, including resolving
	//  any markov chain hierarchy.  Because of this possibility, we return
	//  the number of settings in the weights array returned.  We'll generally
	//  call this function with i == 0
	// Input  : *base - 
	//			i - 
	//			**weights - 
	// Output : int
	//-----------------------------------------------------------------------------
	inline int psetting( byte *base, int i, flexweight_t **weights ) const;
};


struct flexsettinghdr_t
{
	int	id;
	int	version;

	char	name[64];
	int	length;

	int	numflexsettings;
	int	flexsettingindex;
	inline flexsetting_t *pSetting( int i ) const { return (flexsetting_t *)(((byte *)this) + flexsettingindex) + i; };
	int	nameindex;

	// look up flex settings by "index"
	int	numindexes;
	int	indexindex;

	inline flexsetting_t *pIndexedSetting( int index ) const 
	{ 
		if ( index < 0 || index >= numindexes )
		{
			return NULL;
		}

		int i = *((int *)(((byte *)this) + indexindex) + index);
		
		if (i == -1) 
		{
			return NULL;
		}

		return pSetting( i );
	}

	// index names of "flexcontrollers"
	int		numkeys;
	int		keynameindex;
	inline char			*pLocalName( int i ) const { return (char *)(((byte *)this) + *((int *)(((byte *)this) + keynameindex) + i)); };

	int		keymappingindex;
	inline int	*pLocalToGlobal( int i ) const { return (int *)(((byte *)this) + keymappingindex) + i; };
	inline int	LocalToGlobal( int i ) const { return *pLocalToGlobal( i ); };

	//-----------------------------------------------------------------------------
	// Purpose: Same as pSetting( int i ) above, except it translates away any markov groups first
	// Input  : i - index to retrieve
	// Output : flexsetting_t * - a non-markov underlying flexsetting_t
	//-----------------------------------------------------------------------------
	inline flexsetting_t *pTranslatedSetting( int i ) const
	{ 
		flexsetting_t *setting = (flexsetting_t *)(((byte *)this) + flexsettingindex) + i;
		// If this one is not a markov setting, return it
		if ( !setting->IsMarkov() )
		{
			return setting;
		}

		int newindex = setting->GetMarkovSetting( setting->currentindex );
		// Ack, this should never happen (the markov references something that is gone)
		//  Since there was a problem, 
		//  just return this setting anyway, sigh.
		if ( newindex == -1 )
		{
			return setting;
		}

		// Otherwise, recurse on the translated index
		// NOTE:  It's theoretically possible to have an infinite recursion if two markov
		//  groups reference each other.  The faceposer shouldn't create such groups,
		//  so I don't think this will ever actually occur -- ywb
		return pTranslatedSetting( newindex );
	}
};

//-----------------------------------------------------------------------------
// Purpose: Retrieves a pointer to the flexweight_t, including resolving
//  any markov chain hierarchy.  Because of this possibility, we return
//  the number of settings in the weights array returned.  We'll generally
//  call this function with i == 0
// Input  : *base - flexsettinghdr_t * pointer
//			i - index of flex setting to retrieve
//			**weights - destination for weights array starting at index i.
// Output : int
//-----------------------------------------------------------------------------
inline int flexsetting_t::psetting( byte *base, int i, flexweight_t **weights ) const
{ 
	// Assume failure to find index
	*weights = NULL;

	// Recurse if this is a markov setting
	if ( IsMarkov() )
	{
		// Find the current redirected index
		int settingnum = GetMarkovSetting( currentindex );
		if ( settingnum == -1 )
		{
			// Couldn't find currentindex in the markov list for this flex setting
			return -1;
		}

		// Follow the markov link instead
		flexsetting_t *setting = ( (flexsettinghdr_t *)base)->pSetting( settingnum );
		if ( !setting )
		{
			return -1;
		}

		// Recurse ( could support more than one level of markov chains this way )
		return setting->psetting( base, i, weights );
	}

	// Grab array pointer
	*weights = (flexweight_t *)(((byte *)this) + settingindex) + i;

	// Return true number of settings
	return numsettings;
};

// Insert this code anywhere that you need to allow for conversion from an old STUDIO_VERSION
// to a new one.
// If we only support the current version, this function should be empty.
inline void Studio_ConvertStudioHdrToNewVersion( studiohdr_t *pStudioHdr )
{
	COMPILE_TIME_ASSERT( STUDIO_VERSION == 37 ); //  put this to make sure this code is updated upon changing version.
	int version = pStudioHdr->version;

	return;
	
	if( version == STUDIO_VERSION )
	{
		return;
	}
	
	if( version <= 31 )
	{
		pStudioHdr->version = STUDIO_VERSION;
		
		int i;
		for( i = 0; i < pStudioHdr->numseq; i++ )
		{
		//	mstudioseqdesc_t *pSeqdesc = (mstudioseqdesc_t *)hdr->pSeqdesc( i );
			mstudioseqdesc_t *pSeqdesc = pStudioHdr->pSeqdesc( i );
			pSeqdesc->numiklocks = -1;
		}
	}

	if( version <= 32 )
	{
		pStudioHdr->version = STUDIO_VERSION;
		pStudioHdr->numhitboxsets = -1;
	}

	// Slam all bone contents to SOLID for versions <= 35
	if( version <= 35 )
	{
		pStudioHdr->contents = CONTENTS_SOLID;

		int i;
		for( i = 0; i < pStudioHdr->numbones; i++ )
		{
			mstudiobone_t *pBone = pStudioHdr->pBone( i );
			pBone->contents = CONTENTS_SOLID;
		}
	}

	if( version >= 35 )
	{
		// Don't remove this!!!!  This code has to be inspected everytime the studio format changes.
		COMPILE_TIME_ASSERT( STUDIO_VERSION == 37 ); //  put this to make sure this code is updated upon changing version.
		pStudioHdr->version = STUDIO_VERSION;
		
		pStudioHdr->numanimgroup = 1;
		pStudioHdr->animgroupindex = 1;
		
		pStudioHdr->numbonedesc = -1;
		pStudioHdr->bonedescindex = -1;
		
		Assert( 0 );
	}
}

#endif