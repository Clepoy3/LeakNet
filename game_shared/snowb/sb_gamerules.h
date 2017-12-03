#ifndef SB_GAMERULES_H
#define SB_GAMERULES_H

#ifdef _WIN32
#pragma once
#endif

#include "teamplay_gamerules.h"
//#include "sb_shareddefs.h" // VXP: TODO

#ifdef CLIENT_DLL
	#define CSBGameRules C_SBGameRules
#endif

class CSBGameRules : public CTeamplayRules
{
public:
	DECLARE_CLASS( CSBGameRules, CTeamplayRules );
	DECLARE_NETWORKCLASS();

#ifndef CLIENT_DLL

	CSBGameRules();
	virtual ~CSBGameRules();

#endif
};

//-----------------------------------------------------------------------------
// Gets us at the Snowball Fight game rules
//-----------------------------------------------------------------------------
inline CSBGameRules* SBGameRules()
{
	return static_cast<CSBGameRules*>(g_pGameRules);
}

#endif // SB_GAMERULES_H