#ifndef OF_BOT_SPAWNER_H
#define OF_BOT_SPAWNER_H

#ifdef _WIN32
#pragma once
#endif

#define MAX_BOT_TEMPLATES 5

class CTFBot;

class CTFBotSpawner : public CBaseEntity, public CGameEventListener
{
public:
	DECLARE_CLASS(CTFBotSpawner, CBaseEntity);
	DECLARE_DATADESC();
//	DECLARE_SERVERCLASS();

	CTFBotSpawner();
//	~CTFBotSpawner();
	virtual void Spawn();
	void GeneratorThink();
	int UpdateTransmitState()	// always send to all clients
	{
		return SetTransmitState( FL_EDICT_ALWAYS );
	}

	virtual void FireGameEvent( IGameEvent *event );

	// Input handlers
	void	Enable( inputdata_t &inputdata );
	void	Disable( inputdata_t &inputdata );
	
	void	SpawnWave( inputdata_t &inputdata );
	void	SpawnBot( int iTemplate );
	void	ResetInterval( inputdata_t &inputdata );
	void	SetInterval( inputdata_t &inputdata );
	void	SetTemplate( inputdata_t &inputdata );
	void	SetTarget( inputdata_t &inputdata );
	void	KillAllBots( inputdata_t &inputdata );
private:
	bool		m_bDisabled;

	string_t 	m_iszBotTemplate[MAX_BOT_TEMPLATES];
	bool 		m_bRandomizeTemplates;
	int			m_iTemplate;
	int			m_iMaxTemplates;
	int 		m_iWaveSize;
	float 		m_flWaveIntervals;
	bool 		m_bSpawnWhenCleared;
	string_t 	m_iszWavePreset;

	string_t 	m_iszTarget;
	string_t 	m_iszSpawnPoint;
	
	float		m_flSpawnRadius;
	
	float		m_flBotSpawnTick;
	int			m_iSpawnedBots;
	bool		m_bBotsSpawned;
	
	KeyValues	*inBotTemplate[MAX_BOT_TEMPLATES];
	KeyValues	*inWavePreset;
	
	CBaseEntity *pSpawnPoint;
	CBaseEntity *pTarget;
	
	COutputEvent 	m_OnAllBotsKilled;
	COutputEvent 	m_OnBotKilled;
	COutputEvent 	m_OnBotsSpawned;
	
	CUtlVector<int> m_iBots;
	CUtlVector<KeyValues*> m_hBotTemplate;
};

#endif // OF_BOT_SPAWNER_H