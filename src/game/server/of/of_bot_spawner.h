#ifndef OF_BOT_SPAWNER_H
#define OF_BOT_SPAWNER_H

#ifdef _WIN32
#pragma once
#endif

#define MAX_BOT_TEMPLATES 5

class CTFBot;

class CTFAutoHumanTeam : public CBaseEntity
{
public:
	DECLARE_CLASS(CTFAutoHumanTeam, CBaseEntity);
	DECLARE_DATADESC();
	
	CTFAutoHumanTeam();
	~CTFAutoHumanTeam();
	virtual void Spawn();
	int GetHumanTeam(){ return m_iAutoTeam; };
	bool ShouldExcludeBots(){ return m_bExcludeBots; };
	bool AllowSpectator(){ return !m_bDisableSpectating; };
	
	int UpdateTransmitState()	// always send to all clients
	{
		return SetTransmitState( FL_EDICT_ALWAYS );
	}
private:
	CNetworkVar( int, m_iAutoTeam );
	CNetworkVar( bool, m_bExcludeBots );
	CNetworkVar( bool, m_bDisableSpectating );
};

extern CTFAutoHumanTeam *TFAutoTeam();

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