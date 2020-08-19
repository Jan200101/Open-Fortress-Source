// ========= Copyright Open Fortress Developers, CC-BY-NC-SA ============
// Purpose: Spawns bots via templates and assigns them different tasks
// Author(s): KaydemonLP
// Note: 
//

#include "cbase.h"
#include "tf_player.h"
#include "tf_bot.h"
#include "tf_bot_manager.h"
#include "tf_shareddefs.h"
#include "tf_gamerules.h"
#include "entitylist.h"
#include "KeyValues.h"
#include "filesystem.h"
#include "of_bot_spawner.h"
#include "of_shared_schemas.h"
#include "props_shared.h"
#include "entity_tools_server.h"

// memdbgon must be the last include file in a .cpp file!!!
// memdbgon is the worst show EVER it is the wo
#include "tier0/memdbgon.h"

BEGIN_DATADESC(CTFBotSpawner)

DEFINE_KEYFIELD( m_bDisabled, FIELD_BOOLEAN, "StartDisabled" ),

DEFINE_KEYFIELD( m_iszBotTemplate[0], FIELD_STRING, "BotTemplate1" ),
DEFINE_KEYFIELD( m_iszBotTemplate[1], FIELD_STRING, "BotTemplate2" ),
DEFINE_KEYFIELD( m_iszBotTemplate[2], FIELD_STRING, "BotTemplate3" ),
DEFINE_KEYFIELD( m_iszBotTemplate[3], FIELD_STRING, "BotTemplate4" ),
DEFINE_KEYFIELD( m_iszBotTemplate[4], FIELD_STRING, "BotTemplate5" ),

DEFINE_KEYFIELD( m_iTemplate,			FIELD_INTEGER,	"TemplateNum" 		 ),
DEFINE_KEYFIELD( m_bRandomizeTemplates, FIELD_BOOLEAN, 	"RandomizeTemplates" ),
DEFINE_KEYFIELD( m_iWaveSize, 			FIELD_INTEGER, 	"WaveSize"			 ),
DEFINE_KEYFIELD( m_flWaveIntervals, 	FIELD_FLOAT, 	"WaveIntervals"		 ),
DEFINE_KEYFIELD( m_bSpawnWhenCleared,	FIELD_BOOLEAN,	"SpawnWhenCleared"	 ),
DEFINE_KEYFIELD( m_iszWavePreset, 		FIELD_STRING, 	"WavePreset"		 ),

DEFINE_KEYFIELD( m_iszTarget, 		FIELD_STRING, "Target" ),
DEFINE_KEYFIELD( m_iszSpawnPoint, 	FIELD_STRING, "SpawnPoint" ),

DEFINE_KEYFIELD( m_flSpawnRadius,	FIELD_FLOAT, "SpawnRadius" ),
// Inputs.
DEFINE_INPUTFUNC( FIELD_VOID, 	 "SpawnWave",		SpawnWave		),
DEFINE_INPUTFUNC( FIELD_VOID, 	 "ResetInterval",	ResetInterval	),
DEFINE_INPUTFUNC( FIELD_FLOAT, 	 "SetInterval",		SetInterval		),
DEFINE_INPUTFUNC( FIELD_INTEGER, "SetTemplate",		SetTemplate		),
DEFINE_INPUTFUNC( FIELD_STRING,  "SetTarget",		SetTarget		),
DEFINE_INPUTFUNC( FIELD_VOID,	 "KillAllBots",		KillAllBots		),

DEFINE_INPUTFUNC( FIELD_VOID, "Enable", Enable	 ),
DEFINE_INPUTFUNC( FIELD_VOID, "Disable", Disable ),

// Outputs

DEFINE_OUTPUT( m_OnAllBotsKilled, "OnAllBotsKilled" ),
DEFINE_OUTPUT( m_OnBotKilled, "OnBotKilled" ),
DEFINE_OUTPUT( m_OnBotsSpawned, "OnBotsSpawned" ),

DEFINE_THINKFUNC( GeneratorThink ),
END_DATADESC()

LINK_ENTITY_TO_CLASS( of_bot_spawner, CTFBotSpawner );

extern void UTIL_PrecacheSchemaWeapon( const char *szName );

CTFBotSpawner::CTFBotSpawner()
{
	m_flBotSpawnTick = 0.0f;
	m_bBotsSpawned = true;
	m_bDisabled = false;
	m_iTemplate = 0;
	m_iMaxTemplates = 0;
	pSpawnPoint = NULL;

	SetThink( &CTFBotSpawner::GeneratorThink );
	
	SetNextThink( gpGlobals->curtime );
	
	ListenForGameEvent("player_death");
	
	for( int i = 0; i < MAX_BOT_TEMPLATES; i++ )
		inBotTemplate[i] = NULL;
}

void CTFBotSpawner::Spawn( void )
{
	if( STRING(m_iszWavePreset)[0] != '\0' )
	{
		KeyValues *inPresets = GetMapData()->FindKey("WavePresets");
		if( inPresets )
		{
			inWavePreset = inPresets->FindKey(STRING(m_iszWavePreset));
			KeyValues *inTemplates = GetMapData()->FindKey("BotTemplates");
			if( inWavePreset && inTemplates )
			{
				m_iWaveSize = 0;
				m_iTemplate = 0;
				int i = 0;
				FOR_EACH_VALUE( inWavePreset, kvValue )
				{
					inBotTemplate[i] = inTemplates->FindKey( kvValue->GetName() );

					if( inBotTemplate[i] )
					{
						PrecacheModel( inBotTemplate[i]->GetString("Model", "") );
						if( inBotTemplate[i]->GetString("Weapons") )
						{
							CCommand args;
							args.Tokenize(inBotTemplate[i]->GetString("Weapons"));
							for( int i = 0; i < args.ArgC(); i++ )
							{
								UTIL_PrecacheSchemaWeapon( args[i] );
							}
						}
						i++;
						m_iWaveSize += kvValue->GetInt();
					}
					else
						Warning("Bot template %s not found\n", kvValue->GetName());
				}
			}
			else
				Warning("Preset %s not found, aborting\n", STRING(m_iszWavePreset));
		}
		else
			Warning("Attempted to load preset without mapdata entries\n");
	}
	else
	{
		KeyValues *inTemplates = GetMapData()->FindKey("BotTemplates");
		if( inTemplates )
		{
			int y = 0;
			for( int i = 0; i < MAX_BOT_TEMPLATES; i++ )
			{
				if( STRING(m_iszBotTemplate[i])[0] == '\0' )
				{
					Warning("BotTemplate%d not specified, skipping\n", i+1);
					continue;
				}
				inBotTemplate[y] = inTemplates->FindKey(STRING(m_iszBotTemplate[i]));

				if( !inBotTemplate[y] ) // unless its the first template, copy the last template if we dont find the targeted one
				{
					Warning("Failed to find template %s\n", STRING(m_iszBotTemplate[i]));
					if( y )
						inBotTemplate[y] = inBotTemplate[y-1];
				}
				else
				{
					PrecacheModel( inBotTemplate[y]->GetString("Model", "") );
					if( inBotTemplate[y]->GetString("Weapons") )
					{
						CCommand args;
						args.Tokenize(inBotTemplate[y]->GetString("Weapons"));
						for( int i = 0; i < args.ArgC(); i++ )
						{
							UTIL_PrecacheSchemaWeapon( args[i] );
						}
					}
				}
				y++;
			}
			m_iMaxTemplates = y;
		}
		else
			Warning("Failed to find BotTemplates, aborting\n");
	}
	// if we dont manage to find a template, ABORT ABORT ABORT
	if( !inBotTemplate[0] )
	{
		Warning("Bot generator failed to find template, aborting\n");
		UTIL_Remove( this );
	}

	if( m_flWaveIntervals > -1 )
		m_flBotSpawnTick = gpGlobals->curtime + m_flWaveIntervals;
	else
		m_flBotSpawnTick = gpGlobals->curtime;
	
	m_bBotsSpawned = false;
	
	pSpawnPoint = gEntList.FindEntityByName( NULL, STRING(m_iszSpawnPoint) );
	if( !pSpawnPoint || !STRING(m_iszSpawnPoint) )
		pSpawnPoint = this;
	
	pTarget = gEntList.FindEntityByName( NULL, STRING(m_iszTarget) );
	if( !pTarget || STRING(m_iszTarget)[0] == '\0' )
		pTarget = NULL;
}

void CTFBotSpawner::GeneratorThink()
{
	if( m_bDisabled )
	{
		SetNextThink( gpGlobals->curtime + 0.1f );
		return;	
	}
	
	bool bShouldSpawn = !m_bBotsSpawned && 
						(m_iBots.Count() <= 0 || !m_bSpawnWhenCleared);
	
	if( bShouldSpawn && m_flBotSpawnTick < gpGlobals->curtime )
	{
		inputdata_t iTemp;
		SpawnWave( iTemp );

		if( m_flWaveIntervals > -1 )
			m_flBotSpawnTick = gpGlobals->curtime + m_flWaveIntervals;
		else
			m_bBotsSpawned = true;
	}
	SetNextThink( gpGlobals->curtime + 0.1f );
}

void CTFBotSpawner::FireGameEvent( IGameEvent *event )
{
	if( FStrEq(event->GetName(), "player_death" ) )
	{
		int iPlayer = event->GetInt("userid");
		int iBot = m_iBots.Find(iPlayer);
		if( iBot > -1 )
		{
			CTFBot *pBot = dynamic_cast<CTFBot*>(UTIL_PlayerByUserId( m_iBots[iBot] ));
			if( pBot )
			{
				QAngle angLaunch = QAngle( 0, 0 ,0 ); // the angle we will launch ourselves from
				Vector vecLaunch;	// final velocity used to launch the items
				
				KeyValues *kvItems = m_hBotTemplate[iBot]->FindKey("DropItems");
				if( kvItems )
				{
					FOR_EACH_VALUE( kvItems, kvSubKey )
					{
						CTFPowerup *pPowerup = dynamic_cast< CTFPowerup* >( CBaseEntity::CreateNoSpawn( kvSubKey->GetName(), pBot->WorldSpaceCenter(), angLaunch, pBot ) );

						if ( pPowerup )
						{
							CCommand args;
							args.Tokenize(kvSubKey->GetString());
							for( int i = 0; i < args.ArgC(); i += 2 )
							{
								g_ServerTools.SetKeyValue( pPowerup, args[i], args[i+1] );
							}
							// We trigger activate again because we need the team to properly change
							// Yes this causes asserts but its better than checking if the string is TeamNum for every argument
							pPowerup->Activate();
							pPowerup->Spawn();
							angLaunch[ PITCH ] += RandomInt( 5 , 15 );
							angLaunch[ YAW ] = RandomInt( 0 , 360 );
			
							AngleVectors( angLaunch, &vecLaunch );
			
							// boost it up baby
							vecLaunch *= RandomInt( 200, 500 );
							pPowerup->DropSingleInstance( vecLaunch, pBot, 30.0f, 0.2f );
						}
						
						QAngle angLaunch2 = QAngle( 0, 0 ,0 ); // the angle we will launch ourselves from
						Vector vecLaunch2;					  // final velocity used to launch the items
					}
				}
			}
			CBaseEntity *pKiller = UTIL_PlayerByUserId( event->GetInt("attacker") );
			
			m_OnBotKilled.FireOutput( pKiller ? pKiller : this, this );
			m_iBots.Remove(iBot);
			m_hBotTemplate.Remove(iBot);

			if( m_iBots.Count() <= 0 )
				m_OnAllBotsKilled.FireOutput( this, this );
		}
	}
}

void CTFBotSpawner::SpawnWave( inputdata_t &inputdata )
{
	m_OnBotsSpawned.FireOutput( this, this );
	m_iSpawnedBots = 0;

	if( inWavePreset )
	{
		int y = 0;
		FOR_EACH_VALUE( inWavePreset, kvValue )
		{
			for( int i = 0; i < kvValue->GetInt(); i++ )
			{
				SpawnBot( y );
			}
			y++;
		}
	}
	else
	{
		for ( int i = 0; i < m_iWaveSize; ++i )
		{
			SpawnBot(m_iTemplate);
		}
	}
}

void CTFBotSpawner::SpawnBot( int iTemplate )
{
	if( !inBotTemplate[iTemplate] )
	{
		Warning("TEMPLATE %d NOT FOUND\n", iTemplate);
		return;
	}
	char szBotName[32];
	Q_strncpy( szBotName, inBotTemplate[iTemplate]->GetString("Name", TheTFBots().GetRandomBotName()), sizeof(szBotName) );

	CTFBot *bot = NextBotCreatePlayerBot<CTFBot>( szBotName );
	if ( bot == nullptr )
		return;

	char szTeam[10];
	Q_strncpy( szTeam, inBotTemplate[iTemplate]->GetString("Team", "Mercenary"), sizeof(szTeam) );

	int iTeam = TF_TEAM_MERCENARY;

	for ( int i = TEAM_SPECTATOR; i < TF_TEAM_COUNT; ++i )
	{
		if ( stricmp( szTeam, g_aTeamNames[i] ) == 0 )
		{
			iTeam = i;
			break;
		}
	}
	bot->ChangeTeam(iTeam);

	// pick a random color!
	Vector m_vecPlayerColor = vec3_origin;

	if ( TFGameRules() && TFGameRules()->IsDMGamemode() )
	{
		float flColors[ 3 ];

		for ( int i = 0; i < ARRAYSIZE( flColors ); i++ )
			flColors[ i ] = RandomFloat( 1, 255 );

		m_vecPlayerColor.Init( flColors[0], flColors[1], flColors[2] );

		m_vecPlayerColor /= 255.0f;

		bot->m_vecPlayerColor = m_vecPlayerColor;
	}

	char szClassName[32];
	Q_strncpy( szClassName, inBotTemplate[iTemplate]->GetString("Class", "random"), sizeof(szClassName) );

	// Join class, so init everything class related before this
	bot->HandleCommand_JoinClass( szClassName, true );
	
	if( inBotTemplate[iTemplate]->GetInt("Health", -1) > -1 )
		bot->GetPlayerClass()->SetCustomHealth( inBotTemplate[iTemplate]->GetInt("Health") );
	
	if( inBotTemplate[iTemplate]->GetFloat("Speed", -1) > -1 )
		bot->GetPlayerClass()->SetCustomSpeed( inBotTemplate[iTemplate]->GetFloat("Speed") );

	engine->SetFakeClientConVarValue( bot->edict(), g_aLoadoutConvarNames[ bot->GetDesiredPlayerClassIndex() ], inBotTemplate[iTemplate]->GetString("Cosmetics", "") );

	// We set this twice so the cosmetics get set properly
	bot->m_nBotAttrs |= CTFBot::AttributeType::REMOVEONDEATH;
	
	KeyValues *inResponseRules = inBotTemplate[iTemplate]->FindKey("ResponseCriteria");
	if( inResponseRules )
	{
		FOR_EACH_SUBKEY( inResponseRules, kvSubKey )
		{
			bot->AddResponseCriteria( kvSubKey->GetName(), kvSubKey->GetString() );
		}
	}
	
	// PRE-SPAWN
	// SPAWN THE BOT
	bot->ForceRespawn();
	// The remove on death attribute gets removed here :(
	// POST-SPAWN

	// Post-Respawn modifiers
	char szModelName[128];
	Q_strncpy( szModelName, inBotTemplate[iTemplate]->GetString("Model", ""), sizeof(szModelName) );	
	
	if( szModelName[0] != '\0' )
	{
		bot->InvalidateMdlCache();
		int iModel = PrecacheModel( szModelName );
		PrecacheGibsForModel( iModel );	
		
		bot->GetPlayerClass()->SetCustomModel(szModelName);
		bot->UpdateModel();
	}
	
	bot->SetModelScale( inBotTemplate[iTemplate]->GetFloat("ModelScale", 1.0) );
	
	if( inBotTemplate[iTemplate]->GetString("Weapons") )
	{
		bot->StripWeapons();
		CCommand args;
		args.Tokenize(inBotTemplate[iTemplate]->GetString("Weapons"));
		for( int i = 0; i < args.ArgC(); i++ )
		{
			CTFWeaponBase *pGivenWeapon = (CTFWeaponBase *)bot->GiveNamedItem( args[i] );
			if( pGivenWeapon )
			{
				pGivenWeapon->DefaultTouch(bot);
				bot->Weapon_Switch( pGivenWeapon );
			}
		}
	}

	bot->m_nBotAttrs |= CTFBot::AttributeType::REMOVEONDEATH;
	
	QAngle angSpawnAngle( 0, 360.0f * ( (float)m_iSpawnedBots / (float)m_iWaveSize ), 0 );
	Vector vDir;
	AngleVectors( angSpawnAngle, &vDir, NULL, NULL );
	bot->SetAbsOrigin( pSpawnPoint->GetAbsOrigin() + ( vDir * m_flSpawnRadius ) );

	bot->SetTargetOverride( pTarget );
	
	m_iBots.AddToTail(bot->GetUserID());
	m_hBotTemplate.AddToTail(inBotTemplate[iTemplate]);
	
	m_iSpawnedBots++;
}

void CTFBotSpawner::ResetInterval( inputdata_t &inputdata )
{
	if( m_flWaveIntervals > -1 )
		m_flBotSpawnTick = gpGlobals->curtime + m_flWaveIntervals;
	else
	{
		m_flBotSpawnTick = gpGlobals->curtime;
		m_bBotsSpawned = true;
	}
}

void CTFBotSpawner::SetInterval( inputdata_t &inputdata )
{
	m_flWaveIntervals = inputdata.value.Float();
	ResetInterval(inputdata);
}

void CTFBotSpawner::SetTemplate( inputdata_t &inputdata )
{
	m_iTemplate = min( inputdata.value.Int(), m_iMaxTemplates );
}

void CTFBotSpawner::SetTarget( inputdata_t &inputdata )
{
	char szTemp[16];
	Q_strncpy( szTemp, inputdata.value.String(), sizeof(szTemp) );
	strlwr(szTemp);
	if( FStrEq( szTemp, "!activator" ) )
	{
		pTarget = inputdata.pActivator;
		m_iszTarget = pTarget->GetEntityName();
	}
	else if( FStrEq( szTemp, "!caller" ) )
	{
		pTarget = inputdata.pCaller;
		m_iszTarget = pTarget->GetEntityName();
	}
	else
	{
		m_iszTarget = MAKE_STRING(inputdata.value.String());
		pTarget = gEntList.FindEntityByName( NULL, STRING(m_iszTarget) );
		if( !pTarget || inputdata.value.String()[0] == '\0' )
			pTarget = NULL;
	}
	
	DevMsg("Settgin all bot targets to %s\n", inputdata.value.String());
	
	if( pTarget )
		DevMsg("Target class %s\n", pTarget->GetEntityName());
	
	for( int i = 0; i < m_iBots.Count(); i++ )
	{
		CTFBot *pPlayer = dynamic_cast<CTFBot*>(UTIL_PlayerByUserId( m_iBots[i] ));
		if( pPlayer )
		{
			pPlayer->SetTargetOverride( pTarget );
		}
		else
		{
			m_iBots.Remove(i);
			m_hBotTemplate.Remove(i);
			i--;
		}
	}
}

void CTFBotSpawner::KillAllBots( inputdata_t &inputdata )
{
	CUtlVector<CTFBot*> m_hBotsToKill;
	for( int i = 0; i < m_iBots.Count(); i++ )
	{
		CTFBot *pPlayer = dynamic_cast<CTFBot*>(UTIL_PlayerByUserId( m_iBots[i] ));
		if( pPlayer )
		{
			m_hBotsToKill.AddToTail(pPlayer);
		}
	}

	for( int i = 0; i < m_hBotsToKill.Count(); i++ )
	{
		m_hBotsToKill[i]->CommitSuicide( false, true );
	}
}


void CTFBotSpawner::Enable( inputdata_t &inputdata )
{
	m_bDisabled = false;
	ResetInterval(inputdata);
}

void CTFBotSpawner::Disable( inputdata_t &inputdata )
{
	m_bDisabled = true;
}
//pentLandmark = gEntList.FindEntityByName(pentLandmark, m_iLandmark, NULL, pOther, pOther );	