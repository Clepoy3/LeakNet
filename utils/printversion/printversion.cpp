//========= Copyright © 1996-2001, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================

#include <stdio.h>
#include <string.h>
#include "studio.h"

void Usage( void )
{
	fprintf( stderr, "Usage: printversion blah.mdl\n" );
	exit( -1 );
}

int main( int argc, char **argv )
{
	if( argc != 2 )
	{
		Usage();
	}

	FILE *fp = fopen( argv[1], "rb" );
	if( !fp )
	{
		fprintf( stderr, "Can't open: %s\n", argv[1] );
		Usage();
	}
	fseek( fp, 0, SEEK_END );
	int len = ftell( fp );
	rewind( fp );
	studiohdr_t *pStudioHdr = ( studiohdr_t * )malloc( len );
	fread( pStudioHdr, 1, len, fp );
	fprintf( stderr, "%s has %d version\n", argv[1], ( int )pStudioHdr->version );
	fclose( fp );
	return 0;
}