#ifndef FMOD_MANAGER_H
#define FMOD_MANAGER_H
#ifdef _WIN32
#pragma once
#endif

#include "fmod.hpp"

using namespace FMOD;

class SoundData_t
{
public:
	SoundData_t();
	~SoundData_t();

	virtual void TransitionToLoop();
	virtual void TransitionToEnd();
	virtual void EndSound();
	virtual void SetLooping( bool bLooping );
	virtual void SetVolume( float flVolume );
	virtual void UpdateVolume();

public:
	// Channel we use to stream the sounds
	ChannelGroup *pChannelGroupPtr;
	Channel		 *pChannelPtr;

	Sound		 *pIntroPtr;
	Sound		 *pBasePtr;
	Sound		 *pOutroPtr;
	
	void		*pIntroBuf;
	void		*pBaseBuf;
	void		*pOutroBuf;
	
	int			m_iSoundscript;
	int			m_iIndex;
	// Unused for now, setup in the transition functions if needed
	int			m_iStage;
	float		m_flVolume;

	bool		m_bIsLooping;
};

class CFMODManager
{
public:
	CFMODManager();
	~CFMODManager();

	void InitFMOD();
	void ExitFMOD();

	void Think();

	void PlayLoopingMusic( ChannelGroup *pNewChannelGroup, const char* pLoopingMusic,const char* pIntroMusic = NULL, float flDelay = 0 , bool fadeIn = false);
	SoundData_t *PlaySound(char *szSoundName);
	void StopSound( char *szSoundName, int iIndex = -1 );
	void ToEndSound( char *szSoundName, int iIndex = -1 );
	void SetSoundVolume( float flVolume, char *szSoundName, int iIndex = -1 );
	void SetLoopSound( char *szSoundName, bool bLoop, int iIndex = -1 );
	void PlayMusicEnd( ChannelGroup *pNewChannelGroup, const char* pLoopingMusic, bool bDelay = false, Channel *pLoopingChannel = NULL );
	void StopAllSound( void );
	virtual float GetVolumeModifier(void);
	void UpdateSoundVolumes();
	unsigned int GetSoundLengthPCM( Sound *sound );

	CUtlDict< CUtlVector<SoundData_t*>*, unsigned short > m_Sounds;
private:
	void *GetRawSoundPointer( const char* pathToFileFromModFolder, int *pLength );
private:
	bool m_bUnfocused;
	float m_flVolume;

	const char* currentSound;
	const char* introSound;
	const char* loopingSound;
	const char* newSoundFileToTransitionTo;
	bool m_bShouldTransition;
	bool m_bFadeIn;
	bool m_bFadeOut;
	bool m_bIntro;
	float m_fFadeDelay;
	float m_flSongStart;
};

extern CFMODManager* FMODManager();
 
#endif //FMOD_MANAGER_H