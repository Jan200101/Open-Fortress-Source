//====== Copyright � 1996-2005, Valve Corporation, All rights reserved. =======//
//
// Purpose: Music Player, used for Handling Music in Maps
//
//=============================================================================//

#include "cbase.h"
#include "c_of_music_player.h"
#include "tf_gamerules.h"
#include "of_shared_schemas.h"

#include "tier0/memdbgon.h"

extern System *pSystem;

// Inputs.
LINK_ENTITY_TO_CLASS( of_music_player, C_TFMusicPlayer );

IMPLEMENT_CLIENTCLASS_DT( C_TFMusicPlayer, DT_MusicPlayer, CTFMusicPlayer )
	RecvPropString( RECVINFO( szLoopingSong ) ),
	RecvPropBool( RECVINFO( m_bShouldBePlaying ) ),
	RecvPropBool( RECVINFO( m_bHardTransition ) ),
	RecvPropFloat( RECVINFO( m_flDelay ) ),
	RecvPropFloat( RECVINFO( m_flVolume ) ),
END_RECV_TABLE()

C_TFMusicPlayer::C_TFMusicPlayer()
{
	bIsPlaying = false;
	bInLoop = false;
	bParsed = false;
	m_flDelay = 0;
	m_iPhase = 0;
	iSoundIndex = -1;
	pSound = NULL;
}

C_TFMusicPlayer::~C_TFMusicPlayer()
{
	bIsPlaying = false;
	bInLoop = false;
	bParsed = false;
	m_iPhase = 0;

	if (iSoundIndex != -1)
		FMODManager()->StopSound(szLoopingSong, iSoundIndex);
}

void C_TFMusicPlayer::Spawn( void )
{	
	BaseClass::Spawn();
	ClientThink();
}

void C_TFMusicPlayer::ClientThink( void )
{
// Called every frame when the client is in-game

	if ( !bParsed )
	{
		SetNextClientThink( gpGlobals->curtime + 0.1f );
		return;
	}
	
	C_TFPlayer *pLocalPlayer = C_TFPlayer::GetLocalTFPlayer();
	if ( !pLocalPlayer )
	{
		SetNextClientThink( gpGlobals->curtime + 0.1f );
		return;
	}
	if ( pLocalPlayer->m_Shared.InState( TF_STATE_WELCOME ) )
	{
		SetNextClientThink( gpGlobals->curtime + 0.1f );
		return;
	}

	if ( bIsPlaying != m_bShouldBePlaying )
	{
		if ( !m_bShouldBePlaying )
		{
			bIsPlaying = false;
			m_iPhase = 0;
			if( iSoundIndex != -1 )
			{
				if( m_bHardTransition )
				{
					FMODManager()->SetLoopSound(szLoopingSong, false, iSoundIndex);
					FMODManager()->ToEndSound(szLoopingSong, iSoundIndex);
				}
				else
					FMODManager()->StopSound(szLoopingSong, iSoundIndex);
			}
		}
		else
		{
			if( iSoundIndex != -1 )
				FMODManager()->StopSound(szLoopingSong, iSoundIndex);

			bIsPlaying = true;
			bInLoop = false;
			pSound = FMODManager()->PlaySound(szLoopingSong);
			if( pSound != NULL )
			{
				pSound->SetLooping(true);
				iSoundIndex = pSound->m_iIndex;
			}
		}
	}

	HandleVolume();
	
	SetNextClientThink( gpGlobals->curtime + 0.1f );
}

static const ConVar *snd_musicvolume = NULL;
static const ConVar *snd_mute_losefocus = NULL;

extern ConVar of_winscreenratio;

void C_TFMusicPlayer::HandleVolume( void )
{
	C_TFPlayer *pLocalPlayer = C_TFPlayer::GetLocalTFPlayer();

	float flVolumeMod = 1.0f;
	
	if( pLocalPlayer && !pLocalPlayer->IsAlive() )
		flVolumeMod *= 0.4f;

	if( iSoundIndex != -1 )
	{
		FMODManager()->SetSoundVolume( m_flVolume * flVolumeMod, szLoopingSong, iSoundIndex );
	}

}

void C_TFMusicPlayer::OnDataChanged(DataUpdateType_t updateType)
{
	BaseClass::OnDataChanged(updateType);
	if ( updateType == DATA_UPDATE_CREATED )
	{
		if ( bParsed )
			return;

		bParsed = true;			
	}
}

static ConCommand fm_parse_soundmanifest( "fm_parse_soundmanifest", ParseSoundManifest, "Parses the global Sounscript File.", FCVAR_NONE );
static ConCommand fm_check_globalsoundmanifest( "fm_check_globalsoundmanifest", CheckGlobalSounManifest, "Prints out all Manifest files.", FCVAR_NONE );
static ConCommand fm_parse_level_sounds( "fm_parse_level_sounds", ParseLevelSoundManifest, "Parses the level_souns file for the current map.", FCVAR_NONE );

// Inputs.
LINK_ENTITY_TO_CLASS( dm_music_manager, C_TFDMMusicManager );

IMPLEMENT_CLIENTCLASS_DT( C_TFDMMusicManager, DT_DMMusicManager, CTFDMMusicManager )
	RecvPropString( RECVINFO( szWaitingForPlayerMusic ) ),
	RecvPropString( RECVINFO( szRoundMusic ) ),
	RecvPropString( RECVINFO( szWaitingMusicPlayer ) ),
	RecvPropString( RECVINFO( szRoundMusicPlayer ) ),	
	RecvPropInt( RECVINFO( m_iIndex ) ),
	RecvPropEHandle( RECVINFO( pWaitingMusicPlayer ) ),
	RecvPropEHandle( RECVINFO( pRoundMusicPlayer ) ),
END_RECV_TABLE()

C_TFDMMusicManager *gDMMusicManager;
C_TFDMMusicManager* DMMusicManager()
{
	return gDMMusicManager;
}

C_TFDMMusicManager::C_TFDMMusicManager()
{
	gDMMusicManager = this;
}

C_TFDMMusicManager::~C_TFDMMusicManager()
{
	gDMMusicManager = NULL;
}