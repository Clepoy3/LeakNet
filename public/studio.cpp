/***
*
*	Copyright (c) 1999, Valve LLC. All rights reserved.
*	
*	This product contains software technology licensed from Id 
*	Software, Inc. ("Id Technology").  Id Technology (c) 1996 Id Software, Inc. 
*	All Rights Reserved.
*
*   Use, distribution, and modification of this source code and/or resulting
*   object code is restricted to non-commercial enhancements to products from
*   Valve LLC.  All other use, distribution, or modification is prohibited
*   without written permission from Valve LLC.
*
****/
//#undef PROTECT_FILEIO_FUNCTIONS
//#undef fopen
//#undef sprintf
//#undef use_Q_snprintf_instead_of_sprintf

#include "studio.h"
#include "vstdlib/strtools.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------

void Studio_ConvertSeqDescsToNewVersion( studiohdr_t *pStudioHdr )
{
	if ( pStudioHdr->numseq <= 0 )
		return;

	int iSeq;

	size_t size = pStudioHdr->numseq * sizeof( mstudioseqdesc_t );
	for ( iSeq = 0; iSeq < pStudioHdr->numseq; iSeq++ )
	{
		mstudioseqdesc_v36_t *pSeqVersion36 = (mstudioseqdesc_v36_t *)((byte *)pStudioHdr + pStudioHdr->seqindex) + iSeq;
		size += pSeqVersion36->groupsize[0] * pSeqVersion36->groupsize[1] * sizeof( short );
	}

	byte *pTempMemory = new byte[size];

	short *pBlends = (short *)(pTempMemory + pStudioHdr->numseq * sizeof( mstudioseqdesc_t ));

	for ( iSeq = 0; iSeq < pStudioHdr->numseq; iSeq++ )
	{
		mstudioseqdesc_t *pSeq = (mstudioseqdesc_t *)pTempMemory + iSeq;
		mstudioseqdesc_v36_t *pSeqVersion36 = (mstudioseqdesc_v36_t *)((byte *)pStudioHdr + pStudioHdr->seqindex) + iSeq;

		Q_memcpy( pSeq, pSeqVersion36, (size_t)((byte *)pSeqVersion36->anim - (byte *)pSeqVersion36) );

		// VXP: TODO
	//	pSeq->animindexindex = (int)((byte *)pBlends - (byte *)pSeq);
	//	Q_memcpy( &pSeq->movementindex, &pSeqVersion36->movementindex, (size_t)((byte *)&pSeqVersion36->seqgroup - (byte *)&pSeqVersion36->movementindex) );

		pSeq->blendindex = (int)((byte *)pBlends - (byte *)pSeq);
		Q_memcpy( &pSeq->seqgroup, &pSeqVersion36->blendindex, (size_t)((byte *)&pSeqVersion36->seqgroup - (byte *)&pSeqVersion36->blendindex) );

		Q_memcpy( &pSeq->fadeintime, &pSeqVersion36->fadeintime, (size_t)((byte *)&pSeqVersion36->unused[0] - (byte *)&pSeqVersion36->fadeintime) );

		int indexOffset = iSeq * (sizeof( mstudioseqdesc_v36_t ) - sizeof( mstudioseqdesc_t ));
		pSeq->szlabelindex += indexOffset;
		pSeq->szactivitynameindex += indexOffset;
		pSeq->eventindex += indexOffset;
		pSeq->autolayerindex += indexOffset;
		pSeq->weightlistindex += indexOffset;
		pSeq->posekeyindex += indexOffset;
		pSeq->iklockindex += indexOffset;
		pSeq->keyvalueindex += indexOffset;

		for ( int y = 0; y < pSeqVersion36->groupsize[1]; y++ )
		{
			int rowOffset = y * pSeqVersion36->groupsize[0];
			for ( int x = 0; x < pSeqVersion36->groupsize[0]; x++ )
				pBlends[rowOffset + x] = (short)pSeqVersion36->anim[x][y];
		}
		pBlends += pSeqVersion36->groupsize[0] * pSeqVersion36->groupsize[1];
	}

	Q_memcpy( (byte *)pStudioHdr + pStudioHdr->seqindex, pTempMemory, size );
	delete[] pTempMemory;
}
