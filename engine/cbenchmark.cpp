//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

//#include "client_pch.h"
#include "tier0/platform.h"
#include "basetypes.h"
#include "tier0/vprof.h"
#include "vstdlib/icommandline.h"
//#include "tier1/tier1.h"
#include "utlbuffer.h"
#include "utlsymbol.h"
#include "mathlib.h"
#include "fmtstr.h"
#include "convar.h"

#include "common.h"
#include "qlimits.h"
#include "iprediction.h"
#include "icliententitylist.h"

#include "sysexternal.h"
#include "cmd.h"
#include "protocol.h"
#include "render.h"
#include "screen.h"

#include "gl_shader.h"

#include "cdll_engine_int.h"
#include "client_class.h"
#include "client.h"
#include "cl_main.h"
#include "cl_pred.h"

#include "con_nprint.h"
#include "debugoverlay.h"
#include "demo.h"
#include "host_state.h"
#include "host.h"
#include "filesystem.h"
#include "filesystem_engine.h"
#include "proto_version.h"
#include "sys.h"

#ifdef IS_WINDOWS_PC
#include "winlite.h"
#include <winsock2.h> // INADDR_ANY defn
#endif
#include "cbenchmark.h"
#include "tier0/vcrmode.h"
#include "filesystem_engine.h"
#include "sys.h"
#include "KeyValues.h"
//#include "sv_uploaddata.h"
//#include "FindSteamServers.h"
#include "vstdlib/random.h"
//#include "steam/steam_api.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

#define DEFAULT_RESULTS_FOLDER "results"
#define DEFAULT_RESULTS_FILENAME "results.txt"

CBenchmarkResults g_BenchmarkResults;
extern ConVar host_framerate;
//extern void GetMaterialSystemConfigForBenchmarkUpload(KeyValues *dataToUpload);

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CBenchmarkResults::CBenchmarkResults()
{
	m_bIsTestRunning = false;
	m_szFilename[0] = 0;

	m_szOutput[0] = 0;
	fileHandle = NULL;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
bool CBenchmarkResults::IsBenchmarkRunning()
{
	return m_bIsTestRunning;
}
	
//-----------------------------------------------------------------------------
// Purpose: starts recording data
//-----------------------------------------------------------------------------
void CBenchmarkResults::StartBenchmark( /*const CCommand &args*/ )
{
	const char *pszFilename = DEFAULT_RESULTS_FILENAME;
	
//	if ( args.ArgC() > 1 )
	if ( Cmd_Argc() > 1 )
	{
	//	pszFilename = args[1];
		pszFilename = Cmd_Argv(1);
	}

	// check path first
	if ( !COM_IsValidPath( pszFilename ) )
	{
		Msg( "bench_start %s: invalid path.\n", pszFilename );
		return;
	}

	m_bIsTestRunning = true;

	SetResultsFilename( pszFilename );

	// set any necessary settings
//	host_framerate.SetValue( (float)(1.0f / host_state.interval_per_tick) );
//	host_framerate.SetValue( (float)(1.0f / TICK_RATE) );

	// get the current frame and time
	m_iStartFrame = host_framecount;
	m_flStartTime = realtime;

	m_szOutput[0] = 0;

	char buf[256];
	Q_strcat( m_szOutput, "\n------------------------------------------------------------------\n" );

//	Q_snprintf( buf, sizeof( buf ), "Benchmark Began at <TIME> - <DATE>\n" );
	Q_snprintf( buf, sizeof( buf ), "Benchmark Began\n" );
//	Q_snprintf( buf, sizeof( buf ), "%02d:%02d\n", (int)(cl.gettime()/60.0), (int)fmod(cl.gettime(), 60.0) );
	Q_strcat( m_szOutput, buf );

	Q_snprintf( buf, sizeof( buf ), "%s load done in: %f seconds\n", cl.levelname, cl.gettime() );
	Q_strcat( m_szOutput, buf );
	// g_HostState.m_levelName, g_ServerGlobalVariables.mapname, MAKE_STRING( sv.name )
	// char mapname[ MAX_OSPATH ];
	// COM_FileBase( modelloader->GetName( host_state.worldmodel ), mapname );
}

//-----------------------------------------------------------------------------
// Purpose: writes out results to file
//-----------------------------------------------------------------------------
void CBenchmarkResults::StopBenchmark()
{
	m_bIsTestRunning = false;

	// reset
	host_framerate.SetValue( 0 );

	// print out some stats
	int numticks = host_framecount - m_iStartFrame;
	float framerate = numticks / ( realtime - m_flStartTime );
	Msg( "Average framerate: %.2f\n", framerate );
	
	// work out where to write the file
	g_pFileSystem->CreateDirHierarchy( DEFAULT_RESULTS_FOLDER, "MOD" );
	char szFilename[256];
	Q_snprintf( szFilename, sizeof( szFilename ), "%s\\%s", DEFAULT_RESULTS_FOLDER, m_szFilename );

	/* VXP: This is good, but I'll do this later
	// write out the data as keyvalues
	KeyValues *kv = new KeyValues( "benchmark" );
	kv->SetFloat( "framerate", framerate );
	kv->SetInt( "build", build_number() );

	// get material system info
//	GetMaterialSystemConfigForBenchmarkUpload( kv );

	// save
	kv->SaveToFile( g_pFileSystem, szFilename, "MOD" );
	kv->deleteThis();*/

	fileHandle = g_pFileSystem->Open( szFilename, "a+" );
	g_pFileSystem->Seek( fileHandle, 0, FILESYSTEM_SEEK_TAIL );
	g_pFileSystem->FPrintf( fileHandle, "%s", m_szOutput );
	g_pFileSystem->Close( fileHandle );
}

//-----------------------------------------------------------------------------
// Purpose: Sets which file the results will be written to
//-----------------------------------------------------------------------------
void CBenchmarkResults::SetResultsFilename( const char *pFilename )
{
	Q_strncpy( m_szFilename, pFilename, sizeof( m_szFilename ) );
//	Q_DefaultExtension( m_szFilename, ".txt", sizeof( m_szFilename ) );
	COM_DefaultExtension( m_szFilename, ".txt", sizeof( m_szFilename ) );
}

//-----------------------------------------------------------------------------
// Purpose: uploads the most recent results to Steam
//-----------------------------------------------------------------------------
void CBenchmarkResults::Upload()
{
/*
#ifndef NO_STEAM
	if ( !m_szFilename[0] || !SteamUtils() )
		return;
	uint32 cserIP = 0;
	uint16 cserPort = 0;
	while ( cserIP == 0 )
	{
		SteamUtils()->GetCSERIPPort( &cserIP, &cserPort );
		if ( !cserIP )
			Sys_Sleep( 10 );
	}

	netadr_t netadr_CserIP( cserIP, cserPort );
	// upload
	char szFilename[256];
	Q_snprintf( szFilename, sizeof( szFilename ), "%s\\%s", DEFAULT_RESULTS_FOLDER, m_szFilename );
	KeyValues *kv = new KeyValues( "benchmark" );
	if ( kv->LoadFromFile( g_pFileSystem, szFilename, "MOD" ) )
	{
		// this sends the data to the Steam CSER
		UploadData( netadr_CserIP.ToString(), "benchmark", kv );
	}

	kv->deleteThis();
#endif
*/
}

//-----------------------------------------------------------------------------
// Purpose: Prints some information about benchmark
//-----------------------------------------------------------------------------
void CBenchmarkResults::BenchmarkEcho( const char *pszString, bool bPrint )
{
//	char szOutput[256];
//	Q_snprintf( szOutput, sizeof( szOutput ), "%s\n", pszString );

	Q_strcat( m_szOutput, pszString );

	if ( bPrint )
		Con_Printf ( pszString );
}

CON_COMMAND/*_F*/( bench_start, "Starts gathering of info. Arguments: filename to write results into"/*, FCVAR_CHEAT*/ )
{
	GetBenchResultsMgr()->StartBenchmark( /*args*/ );
}

CON_COMMAND/*_F*/( bench_end, "Ends gathering of info."/*, FCVAR_CHEAT*/ )
{
	GetBenchResultsMgr()->StopBenchmark();
}

CON_COMMAND/*_F*/( bench_upload, "Uploads most recent benchmark stats to the Valve servers."/*, FCVAR_CHEAT*/ )
{
	GetBenchResultsMgr()->Upload();
}

CON_COMMAND( bench_echo, "Prints some information about benchmark." )
{
	if ( Cmd_Argc() <= 1 )
	{
		return;
	}

	GetBenchResultsMgr()->BenchmarkEcho( Cmd_Argv(1) );
}

