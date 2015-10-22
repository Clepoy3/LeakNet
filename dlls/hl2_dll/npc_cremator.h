#include "AI_Default.h"
#include "AI_Task.h"
#include "AI_Schedule.h"
#include "AI_Hull.h"
#include "soundent.h"
#include "game.h"
#include "NPCEvent.h"
#include "EntityList.h"
#include "activitylist.h"
#include "AI_BaseNPC.h"
#include "engine/IEngineSound.h"

//=========================================================
//=========================================================
class CNPC_Cremator : public CAI_BaseNPC
{
	DECLARE_CLASS( CNPC_Cremator, CAI_BaseNPC );

public:
	void	Precache( void );
	void	Spawn( void );
	Class_T Classify( void );

	DECLARE_DATADESC();

	// overrides
	void	Event_Killed( const CTakeDamageInfo &info );

	void	DeathSound( void );
	/*void	AlertSound( void );
	void	IdleSound( void );
	void	PainSound( void );
	void	FearSound( void );
	void	LostEnemySound( void );*/
	void	FoundEnemySound( void );

	// This is a dummy field. In order to provide save/restore
	// code in this file, we must have at least one field
	// for the code to operate on. Delete this field when
	// you are ready to do your own save/restore for this
	// character.
	int		m_iDeleteThisField;

	DEFINE_CUSTOM_AI;
};