#include "cbase.h"
#include "fmod_manager.h"
#include "of_shared_schemas.h"
#include "filesystem.h"

#define FOR_EACH_SOUND(pManager,pElement)																				\
for( int i = pManager->m_Sounds.First(); i != pManager->m_Sounds.InvalidIndex(); i = pManager->m_Sounds.Next(i) )		\
{																														\
	int iElementCount = pManager->m_Sounds[i]->Tail()->m_iIndex;														\
	for (int y = 0; y <= iElementCount; y++)																			\
	{																													\
		if (pManager->m_Sounds[i]->IsValidIndex(y))																		\
		{																												\
			SoundData_t *pElement = pManager->m_Sounds[i]->Element(y);											

#define END_EACH_SOUND	\
		}				\
	}					\
}
																				   

using namespace FMOD;

System			*pSystem;
ChannelGroup	*pChannelGroup;
FMOD_RESULT		result;

CFMODManager gFMODMng;
CFMODManager* FMODManager()
{
	return &gFMODMng;
}

CFMODManager::CFMODManager()
{
	m_fFadeDelay = 0.0;
	m_flSongStart = 0.0f;
	newSoundFileToTransitionTo = "NULL";
	currentSound = "NULL";
	m_bShouldTransition = false;
	m_bFadeIn = false;
	m_bFadeOut = false;
	m_bUnfocused = false;
}

CFMODManager::~CFMODManager()
{
	m_fFadeDelay = 0.0;
	m_flSongStart = 0.0f;
	newSoundFileToTransitionTo = "NULL";
	currentSound = "NULL";
	m_bShouldTransition = false;
	m_bFadeIn = false;
	m_bFadeOut = false;
}

// Starts FMOD
void CFMODManager::InitFMOD(void)
{
	result = System_Create(&pSystem); // Create the main system object.

	if (result != FMOD_OK)
		Warning("FMOD ERROR: System creation failed!\n");
	else
		DevMsg("FMOD system successfully created.\n");

	result = pSystem->init(100, FMOD_INIT_NORMAL, 0);   // Initialize FMOD system.

	if (result != FMOD_OK)
		Warning("FMOD ERROR: Failed to initialize properly!\n");
	else
		DevMsg("FMOD initialized successfully.\n");
}

// Stops FMOD
void CFMODManager::ExitFMOD(void)
{
	result = pSystem->release();

	if (result != FMOD_OK)
		Warning("FMOD ERROR: System did not terminate properly!\n");
	else
		DevMsg("FMOD system terminated successfully.\n");
}

// Returns the raw sound data, required to be able to load packed sounds
void *CFMODManager::GetRawSoundPointer(const char* pathToFileFromModFolder, int *pLength )
{
	char fullpath[512];
	Q_snprintf(fullpath, sizeof(fullpath), "sound/%s", pathToFileFromModFolder);

	void *buffer = NULL;

	int length = filesystem->ReadFileEx( fullpath, "GAME", &buffer, true, true );

	if ( pLength )
	{
		*pLength = length;
	}

	return buffer;
}

// Called every frame when the client is in-game
void CFMODManager::Think(void)
{
	const ConVar *snd_mute_losefocus = NULL;
	snd_mute_losefocus = g_pCVar->FindVar("snd_mute_losefocus");
	static const ConVar *snd_musicvolume = NULL;
	snd_musicvolume = g_pCVar->FindVar("snd_musicvolume");

	if( m_bUnfocused != engine->IsActiveApp() )
	{
		m_bUnfocused = engine->IsActiveApp();

		if( snd_mute_losefocus->GetBool() )
		{
			for( int i = m_Sounds.First(); i != m_Sounds.InvalidIndex(); i = m_Sounds.Next(i) )
			{
				int iElementCount = m_Sounds[i]->Tail()->m_iIndex;
				for (int y = 0; y <= iElementCount; y++)
				{
					if( m_Sounds[i]->IsValidIndex(y) )
						m_Sounds[i]->Element(y)->pChannelGroupPtr->setMute(!engine->IsActiveApp());
				}
			}
		}
	}

	if( snd_musicvolume->GetFloat() != m_flVolume )
	{
		m_flVolume = snd_musicvolume->GetFloat();
		UpdateSoundVolumes();
	}
	pSystem->update();
}

// Compares specified ambient sound with the current ambient sound being played
// Returns true if they match, false if they do not or if no sound is being played
unsigned int CFMODManager::GetSoundLengthPCM(Sound *sound)
{
	unsigned int templength = 0;
	sound->getLength(&templength, FMOD_TIMEUNIT_PCM);
	return templength - 1;
}

// Abruptly starts playing a specified ambient sound
// In most cases, we'll want to use TransitionAmbientSounds instead
void CFMODManager::PlayLoopingMusic( ChannelGroup *pNewChannelGroup, const char* pLoopingMusic, const char* pIntroMusic, float flDelay , bool fadeIn)
{
	Channel *pTempChannel;
	Channel *pTempLoopChannel;
	
	if ( !pNewChannelGroup )
		return;
	
	int  outputrate = 0;
	result = pSystem->getSoftwareFormat(&outputrate, 0, 0);
	unsigned int dsp_block_len = 0;
	result = pSystem->getDSPBufferSize(&dsp_block_len, 0);
	unsigned long long clock_start = 0;
	float freq = 0;
	unsigned int slen = 0;
	
	if( pIntroMusic )
	{
		Sound *pIntroSound = NULL;
		Sound *pLoopingSound = NULL;
		
		int iLengthIntro = 0;
		void *vBufferIntro = GetRawSoundPointer(pIntroMusic, &iLengthIntro);
		
		FMOD_CREATESOUNDEXINFO infoIntro;
		memset(&infoIntro, 0, sizeof(infoIntro));
		infoIntro.length = iLengthIntro;
		infoIntro.cbsize = sizeof(infoIntro);
		
		result = pSystem->createStream((const char *)vBufferIntro, FMOD_OPENMEMORY | FMOD_CREATESTREAM , &infoIntro, &pIntroSound);
		pSystem->playSound(pIntroSound, pNewChannelGroup, true, &pTempChannel);

		result = pIntroSound->getDefaults(&freq, 0);
		
		result = pTempChannel->getDSPClock(0, &clock_start);

		clock_start += (dsp_block_len * 2);

		slen = (unsigned int) ( flDelay * 30000.0f );
		slen = (unsigned int)((float)slen / freq * outputrate);
		clock_start += slen;
		
		result = pTempChannel->setDelay(clock_start, 0, false);
		result = pTempChannel->setPaused(false);

		int iLengthLoop = 0;
		void *vBufferLoop = GetRawSoundPointer(pLoopingMusic, &iLengthLoop);
		
		FMOD_CREATESOUNDEXINFO infoLoop;
		memset(&infoLoop, 0, sizeof(infoLoop));
		infoLoop.length = iLengthLoop;
		infoLoop.cbsize = sizeof(infoLoop);

		result = pSystem->createStream((const char *)vBufferLoop, FMOD_OPENMEMORY | FMOD_LOOP_NORMAL | FMOD_CREATESTREAM, &infoLoop, &pLoopingSound);
		result = pLoopingSound->setLoopPoints(0, FMOD_TIMEUNIT_PCM, GetSoundLengthPCM(pLoopingSound), FMOD_TIMEUNIT_PCM);
		pSystem->playSound(pLoopingSound, pNewChannelGroup, true, &pTempLoopChannel);

		result = pIntroSound->getLength(&slen, FMOD_TIMEUNIT_PCM);
		result = pIntroSound->getDefaults(&freq, 0);

		slen = (unsigned int)((float)slen / freq * outputrate);
		clock_start += slen;

		result = pTempLoopChannel->setDelay(clock_start, 0, false);
		result = pTempLoopChannel->setPaused(false);
		pTempChannel->setChannelGroup(pNewChannelGroup);
		pTempLoopChannel->setChannelGroup(pNewChannelGroup);
	}
	else
	{
		int iLengthLoop = 0;
		void *vBufferLoop = GetRawSoundPointer(pLoopingMusic, &iLengthLoop);
		
		FMOD_CREATESOUNDEXINFO infoLoop;
		memset(&infoLoop, 0, sizeof(infoLoop));
		infoLoop.length = iLengthLoop;
		infoLoop.cbsize = sizeof(infoLoop);

		Sound *pSound = NULL;

		result = pSystem->createStream((const char *)vBufferLoop, FMOD_OPENMEMORY | FMOD_LOOP_NORMAL | FMOD_CREATESTREAM , &infoLoop, &pSound);
		if (result != FMOD_OK)
		{
			Warning("FMOD: Failed to create stream of sound '%s' ! (ERROR NUMBER: %i)\n", pLoopingMusic, result);
			return;
		}
		result = pSound->setLoopPoints(0, FMOD_TIMEUNIT_PCM, GetSoundLengthPCM(pSound), FMOD_TIMEUNIT_PCM);
		result = pSystem->playSound(pSound, pNewChannelGroup, true, &pTempChannel);
		
		result = pTempChannel->getDSPClock(0, &clock_start);

		clock_start += (dsp_block_len * 2);
		pSound->getDefaults(&freq, 0);
		slen = (unsigned int) ( flDelay * 30000.0f );
		slen = (unsigned int)((float)slen / freq * outputrate);
		clock_start += slen;

		result = pTempChannel->setDelay(clock_start, 0, false);
		
		pTempChannel->setPosition(0, FMOD_TIMEUNIT_PCM);
		pTempChannel->setPaused(false);
		if (result != FMOD_OK)
		{
			Warning("FMOD: Failed to play sound '%s' ! (ERROR NUMBER: %i)\n", pLoopingMusic, result);
			return;
		}
		pTempChannel->setChannelGroup(pNewChannelGroup);
	}
	//	result = pSound->setLoopCount( -1 );
	//	
	m_flSongStart = gpGlobals->realtime;
	currentSound = pLoopingMusic;
}

// Handle Callbacks for intro/loop/outro
FMOD_RESULT F_CALLBACK SoundHandleCallback(FMOD_CHANNELCONTROL *channelcontrol,
	FMOD_CHANNELCONTROL_TYPE controltype,
	FMOD_CHANNELCONTROL_CALLBACK_TYPE callbacktype,
	void *commanddata1, void *commanddata2)
{
	if ( callbacktype == FMOD_CHANNELCONTROL_CALLBACK_SYNCPOINT )
	{
		// Get the sync point
		int iSyncIndex = (int)commanddata1;
		// Assume we only ever use channel control with this callback
		Channel *pChannel = (Channel*)channelcontrol;
		Sound *pSound;
		pChannel->getCurrentSound(&pSound);
		if( !pSound )
			return FMOD_OK;

		FMOD_SYNCPOINT *pPoint;

		pSound->getSyncPoint(iSyncIndex, &pPoint);

		char szTarget[32];
		unsigned int iOffset = 0;
		int iOffsetType = 0;
		pSound->getSyncPointInfo(pPoint, szTarget, sizeof(szTarget), &iOffset, iOffsetType);
		CCommand args;
		args.Tokenize(szTarget);
		int iSoundscript = atoi(args[0]);
		int iIndex = atoi(args[1]);
		int iStage = atoi(args[2]);
		
		switch( iStage )
		{
			// Intro
			case 0:
			// Transition to loop when you're done with the intro
			FMODManager()->m_Sounds[iSoundscript]->Element(iIndex)->TransitionToLoop();
			break;

			// Loop
			case 1:
			// Check if we should transition to end
			FMODManager()->m_Sounds[iSoundscript]->Element(iIndex)->TransitionToEnd();
			break;

			// Outro
			case 2:
			// Clear out all buffers and remove the song from our dictionary
			FMODManager()->m_Sounds[iSoundscript]->Element(iIndex)->EndSound();
			break;
		}
	}
	return FMOD_OK;
}

SoundData_t::SoundData_t()
{
	pChannelGroupPtr = NULL;
	pChannelPtr = NULL;
	pIntroPtr = NULL;
	pBasePtr = NULL;
	pOutroPtr = NULL;
	
	pIntroBuf = NULL;
	pBaseBuf = NULL;
	pOutroBuf = NULL;
	
	m_bIsLooping = false;
	
	if (pSystem)
		pSystem->createChannelGroup("Parent", &pChannelGroupPtr);
}

SoundData_t::~SoundData_t()
{
	DevWarning("Sound data cleared\n");

	// We dont necessary have a start and end, so check first
	if( pIntroPtr )
		pIntroPtr->release();
	if( pOutroPtr )
		pOutroPtr->release();
	
	pChannelGroupPtr->stop();
	pChannelPtr->stop();

	pBasePtr->release();

	if( pIntroBuf )
		delete pIntroBuf;
	if( pOutroBuf )
		delete pOutroBuf;

	delete pBaseBuf;
};

void SoundData_t::TransitionToLoop()
{
	pSystem->playSound(pBasePtr, pChannelGroupPtr, false, &pChannelPtr);
	pChannelPtr->setCallback((FMOD_CHANNELCONTROL_CALLBACK)SoundHandleCallback);
}

void SoundData_t::TransitionToEnd()
{
	if( m_bIsLooping )
		return;

	pBasePtr->release();
	pChannelPtr->stop();
		
	if( !pOutroPtr )
	{
		EndSound();
		return;
	}

	pSystem->playSound(pOutroPtr, pChannelGroupPtr, false, &pChannelPtr);
	pChannelPtr->setCallback((FMOD_CHANNELCONTROL_CALLBACK)SoundHandleCallback);
}

void SoundData_t::EndSound()
{
	FMODManager()->m_Sounds.Element(m_iSoundscript)->FastRemove(m_iIndex);
	if( FMODManager()->m_Sounds.Element(m_iSoundscript)->Count() <= 0 )
		FMODManager()->m_Sounds.RemoveAt(m_iSoundscript);

	delete this;
}

void SoundData_t::SetLooping( bool bLooping )
{
	m_bIsLooping = bLooping;
	
	pBasePtr->setLoopCount(bLooping ? -1 : 0);
}

void SoundData_t::SetVolume( float flVolume )
{
	m_flVolume = flVolume;
}

void SoundData_t::UpdateVolume()
{
	bool bMuted = false;
	pChannelGroupPtr->getMute(&bMuted);
	pChannelGroupPtr->setVolume(m_flVolume * FMODManager()->GetVolumeModifier());
	pChannelGroupPtr->setMute(bMuted);
}

float CFMODManager::GetVolumeModifier()
{
	static const ConVar *snd_musicvolume = NULL;
	snd_musicvolume = g_pCVar->FindVar("snd_musicvolume");

	return snd_musicvolume->GetFloat();
}

void CFMODManager::UpdateSoundVolumes()
{
	FOR_EACH_SOUND(this,pElement)
		pElement->UpdateVolume();
	END_EACH_SOUND
}

/*
	Used for setting delay
	int  iOutputRate = 0;
	unsigned int iDspBufferSize = 0;
	result = pSystem->getSoftwareFormat(&iOutputRate, 0, 0);
	result = pSystem->getDSPBufferSize(&iDspBufferSize, 0);
	unsigned long long iClockTime = 0;
	float flFrequency = 0;
	unsigned int iSoundLenght = 0;
	
	result = pNewSound->pIntroPtr->getDefaults(&flFrequency, 0);
	
	result = pNewSound->pChannelPtr->getDSPClock(0, &iClockTime);

	// Current clock time
	iClockTime += (iDspBufferSize * 2);

	// 30000.0f is the amount needed to convert seconds to DSP
	float flDelay = 0.0f;
	iSoundLenght = (unsigned int)( flDelay * 30000.0f );
	iSoundLenght = (unsigned int)( (float)iSoundLenght / flFrequency * iOutputRate );
	// Set the clock to the end of the 
	iClockTime += iSoundLenght;
*/

// Put a sound into our system and start playing it
// To make it loop, use StartLoop( "Soundscript Name", iArrayPos )
SoundData_t *CFMODManager::PlaySound(char *szSoundName)
{
	// Assume the string parsed is valid
	KeyValues *pSoundscript = GetSoundscript( szSoundName );
	if( !pSoundscript )
		// Soundscript does not exist :(
		return NULL;

	const char *szIntroWav = NULL;
	const char *szBaseWav = pSoundscript->GetString( "wave", NULL );
	const char *szOutroWav = NULL;
	
	// Don't play anything if we dont have a base wave, even if we have intros and outros
	if( szBaseWav == NULL )
		return NULL;
	
	SoundData_t *pNewSound = new SoundData_t();
	
	// I don't like doing this, however, 
	// for some reason, when there's no intro,
	// FMOD encounters an engine level crash
	// when you play a previously played sound
	// So,
	// HACK HACK: If no intro is set, set it to the Null sound
	KeyValues *pStageScript = GetSoundscript(pSoundscript->GetString("intro", "Null"));
	if( pStageScript )
		szIntroWav = pStageScript->GetString( "wave", NULL );
	// Note: This probably has something to do with memory allocation
	// Since this only happens when all instances of this sound stop playing
	// Playing the same sound before a previous one ended works without problem

	pStageScript = GetSoundscript(pSoundscript->GetString("outro", NULL));
	if( pStageScript )
		szOutroWav = pStageScript->GetString("wave", NULL);

	// String we use to format the song paths
	char szSongName[1024];
	
	Q_strncpy(szSongName, szBaseWav, sizeof(szSongName));
	if (!Q_strncmp(szSongName, "#", 1))
	{
		memmove(&szSongName, &szSongName[1], strlen(szSongName));
	}
	
	// Buffer for 
	int iBuffSize = 0;
	pNewSound->pBaseBuf = GetRawSoundPointer(szSongName, &iBuffSize);

	// SoundEXInfo is required for the sound to know where the buffer ends
	FMOD_CREATESOUNDEXINFO infoLoop;
	memset(&infoLoop, 0, sizeof(infoLoop));
	infoLoop.length = iBuffSize;
	infoLoop.cbsize = sizeof(infoLoop);
	
	pSystem->createStream( (const char *)pNewSound->pBaseBuf,
		FMOD_OPENMEMORY | FMOD_CREATESTREAM | FMOD_LOOP_NORMAL,
		&infoLoop,
		&pNewSound->pBasePtr );
		
	pNewSound->pBasePtr->setLoopPoints(0, FMOD_TIMEUNIT_PCM, GetSoundLengthPCM(pNewSound->pBasePtr), FMOD_TIMEUNIT_PCM);

	// Don't loop by default
	pNewSound->pBasePtr->setLoopCount(0);

	// Setup Intro
	if( szIntroWav )
	{
		Q_strncpy(szSongName, szIntroWav, sizeof(szSongName));
		if (!Q_strncmp(szSongName, "#", 1))
		{
			memmove(&szSongName, &szSongName[1], strlen(szSongName));
		}

		pNewSound->pIntroBuf = GetRawSoundPointer(szSongName, &iBuffSize);
		
		FMOD_CREATESOUNDEXINFO infoIntro;
		memset(&infoIntro, 0, sizeof(infoIntro));
		infoIntro.length = iBuffSize;
		infoIntro.cbsize = sizeof(infoIntro);
		
		pSystem->createStream((const char *)pNewSound->pIntroBuf, FMOD_OPENMEMORY | FMOD_CREATESTREAM, &infoIntro, &pNewSound->pIntroPtr);
	}
	
	// Setup Outro
	if( szOutroWav )
	{
		Q_strncpy(szSongName, szOutroWav, sizeof(szSongName));
		if (!Q_strncmp(szSongName, "#", 1))
		{
			memmove(&szSongName, &szSongName[1], strlen(szSongName));
		}

		pNewSound->pOutroBuf = GetRawSoundPointer(szSongName, &iBuffSize);
		
		FMOD_CREATESOUNDEXINFO infoOutro;
		memset(&infoOutro, 0, sizeof(infoOutro));
		infoOutro.length = iBuffSize;
		infoOutro.cbsize = sizeof(infoOutro);
		
		pSystem->createStream((const char *)pNewSound->pOutroBuf, FMOD_OPENMEMORY | FMOD_CREATESTREAM, &infoOutro, &pNewSound->pOutroPtr);
	}
	
	int iIndex = m_Sounds.Find(szSoundName);

	if (iIndex == m_Sounds.InvalidIndex())
	{
		iIndex = m_Sounds.Insert(szSoundName);
		m_Sounds[iIndex] = new CUtlVector < SoundData_t* >;
	}

	m_Sounds[iIndex]->AddToTail(pNewSound);
	
	int iElementIndex = m_Sounds[iIndex]->Count() - 1;

	// Set the Soundscript pos and Index
	pNewSound->m_iSoundscript = iIndex;
	pNewSound->m_iIndex = iElementIndex;

	// Temp Pointer we just need to initialize a syncpoint
	FMOD_SYNCPOINT *ptr = NULL;

	if( szIntroWav )
	{
		pNewSound->pIntroPtr->addSyncPoint(GetSoundLengthPCM(pNewSound->pIntroPtr),
			FMOD_TIMEUNIT_PCM,
			VarArgs("%d %d 0", iIndex, iElementIndex), // 0 Stands for Intro, we use this to determine what to do in a callback
			&ptr);
		pSystem->playSound(pNewSound->pIntroPtr, pNewSound->pChannelGroupPtr, false, &pNewSound->pChannelPtr);
	}
	else
	{
		pSystem->playSound(pNewSound->pBasePtr, pNewSound->pChannelGroupPtr, false, &pNewSound->pChannelPtr);
	}

	ptr = NULL;
	pNewSound->pBasePtr->addSyncPoint(FMODManager()->GetSoundLengthPCM(pNewSound->pBasePtr),
	FMOD_TIMEUNIT_PCM,
	VarArgs("%d %d 1", iIndex, iElementIndex), // 1 Stands for LOOP, we use this to determine what to do in a callback
	&ptr);
	if( szOutroWav )
	{
		ptr = NULL;
		pNewSound->pOutroPtr->addSyncPoint(FMODManager()->GetSoundLengthPCM(pNewSound->pOutroPtr),
			FMOD_TIMEUNIT_PCM,
			VarArgs("%d %d 2", iIndex, iElementIndex), // 2 Stands for Outro, we use this to determine what to do in a callback
			&ptr);
	}
	pNewSound->pChannelPtr->setCallback((FMOD_CHANNELCONTROL_CALLBACK)SoundHandleCallback);	

	const ConVar *snd_mute_losefocus = NULL;
	snd_mute_losefocus = g_pCVar->FindVar("snd_mute_losefocus");

	pNewSound->SetVolume(pStageScript->GetFloat("volume", 1.0f));
	pNewSound->UpdateVolume();
	pNewSound->pChannelGroupPtr->setMute(!engine->IsActiveApp() && snd_mute_losefocus->GetBool());
	pNewSound->pChannelPtr->setPaused(false);

	return pNewSound;

/*
	result = pLoopingSound->setLoopPoints(0, FMOD_TIMEUNIT_PCM, GetSoundLengthPCM(pLoopingSound), FMOD_TIMEUNIT_PCM);

	int iLengthLoop = 0;
	void *pBufferLoop = GetRawSoundPointer(pLoopingMusic, &iLengthLoop);
	
	FMOD_CREATESOUNDEXINFO infoLoop;
	memset(&infoLoop, 0, sizeof(infoLoop));
	infoLoop.length = iLengthLoop;
	infoLoop.cbsize = sizeof(infoLoop);

	result = pSystem->createStream((const char *)pBufferLoop, FMOD_OPENMEMORY | FMOD_LOOP_NORMAL | FMOD_CREATESTREAM, &infoLoop, &pLoopingSound);
	pSystem->playSound(pLoopingSound, pNewChannelGroup, true, &pTempLoopChannel);

	result = pNewSound->pStartPtr->getLength(&iSoundLenght, FMOD_TIMEUNIT_PCM);
	result = pNewSound->pStartPtr->getDefaults(&flFrequency, 0);

	iSoundLenght = (unsigned int)((float)iSoundLenght / flFrequency * iOutputRate);
	iClockTime += iSoundLenght;

	result = pTempLoopChannel->setDelay(iClockTime, 0, false);
	result = pTempLoopChannel->setPaused(false);
	pTempChannel->setChannelGroup(pNewChannelGroup);
	pTempLoopChannel->setChannelGroup(pNewChannelGroup);
	*/
	// Save for later, whenever a sound reaches its last dsp point, fast remove itself
	// Use .InvalidIndex() to find the first free index and insert the sound inside that
	// We do this so our indexes dont get broken when a sound stops playing
	// m_Sounds.Element(0).FastRemove();
}

void ConPlayFmodSound(const CCommand &args)
{
	char szSound[1024];
	Q_strncpy(szSound, args.Arg(1),sizeof(szSound));
	FMODManager()->PlaySound(szSound);
}

static ConCommand fm_play("fm_play", ConPlayFmodSound, "Play a Sounscript File.", FCVAR_NONE);

void ConPlayLoopingFmodSound(const CCommand &args)
{
	char szSound[1024];
	Q_strncpy(szSound, args.Arg(1),sizeof(szSound));
	SoundData_t *pSound = FMODManager()->PlaySound(szSound);
	
	if( pSound )
		pSound->SetLooping(true);
}

static ConCommand fm_play_looping("fm_play_looping", ConPlayLoopingFmodSound, "Play a looping Sounscript File.", FCVAR_NONE);

void ConSetLoopingFmodSound(const CCommand &args)
{
	char szSound[1024];
	Q_strncpy(szSound, args.Arg(1),sizeof(szSound));
	bool bLoop = atoi(args.Arg(2)) > 0;
	FMODManager()->SetLoopSound(szSound, bLoop);
}

static ConCommand fm_set_looping("fm_set_looping", ConSetLoopingFmodSound, "Sets if a sound loops or not.", FCVAR_NONE);

void CFMODManager::SetLoopSound( char *szSoundName, bool bLoop, int iIndex )
{
	unsigned short nSoundscript = m_Sounds.Find(szSoundName);
	if( nSoundscript == m_Sounds.InvalidIndex() )
		return;

	if( iIndex > -1 )
		m_Sounds.Element(nSoundscript)->Element(iIndex)->SetLooping(bLoop);
	else
	{
		FOR_EACH_SOUND(this, pElement)
			pElement->SetLooping(bLoop);
		END_EACH_SOUND
	}
}

void ConStopFmodSound(const CCommand &args)
{
	char szSound[1024];
	Q_strncpy(szSound, args.Arg(1), sizeof(szSound));
	FMODManager()->StopSound(szSound);
}

static ConCommand fm_stop("fm_stop", ConStopFmodSound, "Stop a Sounscript File.", FCVAR_NONE);

void CFMODManager::StopSound(char *szSoundName, int iIndex)
{
	unsigned short nSoundscript = m_Sounds.Find(szSoundName);
	if( nSoundscript == m_Sounds.InvalidIndex() )
		return;

	if (iIndex > -1)
		m_Sounds.Element(nSoundscript)->Element(iIndex)->EndSound();
	else
	{
		FOR_EACH_SOUND(this, pElement)
			pElement->EndSound();
		END_EACH_SOUND
	}
}

void ConEndFmodSound(const CCommand &args)
{
	char szSound[1024];
	Q_strncpy(szSound, args.Arg(1), sizeof(szSound));
	FMODManager()->ToEndSound(szSound);
}

static ConCommand fm_end("fm_end", ConEndFmodSound, "Send a Sounscript File to the outro.", FCVAR_NONE);

void CFMODManager::ToEndSound(char *szSoundName, int iIndex)
{
	unsigned short nSoundscript = m_Sounds.Find(szSoundName);
	if (nSoundscript == m_Sounds.InvalidIndex())
		return;

	if (iIndex > -1)
		m_Sounds.Element(nSoundscript)->Element(iIndex)->TransitionToEnd();
	else
	{
		FOR_EACH_SOUND(this, pElement)
			pElement->TransitionToEnd();
		END_EACH_SOUND
	}
}

void CFMODManager::SetSoundVolume(float flVolume, char *szSoundName, int iIndex )
{
	unsigned short nSoundscript = m_Sounds.Find(szSoundName);
	if (nSoundscript == m_Sounds.InvalidIndex())
		return;

	if (iIndex > -1)
	{
		m_Sounds.Element(nSoundscript)->Element(iIndex)->SetVolume(flVolume);
		m_Sounds.Element(nSoundscript)->Element(iIndex)->UpdateVolume();
	}
	else
	{
		FOR_EACH_SOUND(this, pElement)
			pElement->SetVolume(flVolume);
			pElement->UpdateVolume();
		END_EACH_SOUND
	}
}

void CFMODManager::PlayMusicEnd( ChannelGroup *pNewChannelGroup, const char* pLoopingMusic, bool bDelay, Channel *pLoopingChannel )
{
	Channel *pTempChannel;
	
	if ( !pNewChannelGroup )
		return;
	
	if (pLoopingMusic)
	{
		Sound *pIntroSound = NULL;
		
		int iLength = 0;
		void *vBuffer = GetRawSoundPointer(pLoopingMusic, &iLength);
		
		FMOD_CREATESOUNDEXINFO info;
		memset(&info, 0, sizeof(info));
		info.length = iLength;
		info.cbsize = sizeof(info);			
		
		result = pSystem->createStream((const char*)vBuffer, FMOD_CREATESTREAM | FMOD_OPENMEMORY, &info, &pIntroSound);
		pSystem->playSound(pIntroSound, pNewChannelGroup, true, &pTempChannel);
		
		result = pTempChannel->setPaused(false);
		pTempChannel->setChannelGroup(pNewChannelGroup);
	}
	
}

void CommSndStp( void )
{
	FMODManager()->StopAllSound();
}
static ConCommand fm_stop_all_sound( "fm_stop_all_sound", CommSndStp, "Stops every channel group", FCVAR_NONE );

// Abruptly stops playing all ambient sounds
void CFMODManager::StopAllSound(void)
{
	//	pChannelGroup->setVolume(0.0f);

	for (int i = m_Sounds.First(); i != m_Sounds.InvalidIndex(); i = m_Sounds.Next(i))
	{
		int iElementCount = m_Sounds[i]->Tail()->m_iIndex;
		for (int y = 0; y <= iElementCount; y++)
		{
			if (m_Sounds[i]->IsValidIndex(y))
			{
				SoundData_t *pElement = m_Sounds[i]->Element(y);

				pElement->EndSound();
			}
			m_Sounds[i]->Purge();
		}
	}
	m_Sounds.PurgeAndDeleteElements();
//	pChannelGroup->setPaused(true);
}