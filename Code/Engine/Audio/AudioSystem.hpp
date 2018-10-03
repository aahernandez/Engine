#pragma once

//---------------------------------------------------------------------------
#pragma comment( lib, "ThirdParty/fmod/fmodex_vc" ) // Link in the fmodex_vc.lib static library


//---------------------------------------------------------------------------
#include "ThirdParty/fmod/fmod.hpp"
#include <string>
#include <vector>
#include <deque>
#include <map>


//---------------------------------------------------------------------------
typedef unsigned int SoundID;
typedef void* AudioChannelHandle;
const unsigned int MISSING_SOUND_ID = 0xffffffff;

enum SoundType
{
	SOUND_MUSIC,
	SOUND_GAMEPLAY,
	SOUND_SPEECH,
	NUM_SOUND_TYPES,
};

/////////////////////////////////////////////////////////////////////////////
class AudioSystem
{
public:
	AudioSystem();
	virtual ~AudioSystem();

	SoundID CreateOrGetSound(const std::string& soundFileName);
	AudioChannelHandle PlaySound(SoundID soundID, float volumeLevel = 1.f, SoundType soundType = SOUND_GAMEPLAY);
	AudioChannelHandle PlayGameplaySound(SoundID soundID, float volumeLevel = 1.f);
	void PlayMusic(SoundID soundID, float volumeLevel = 1.f);
	void PlaySpeech(SoundID soundID, float volumeLevel = 1.f);
	void StopChannel(AudioChannelHandle channel);
	void StopMusic();
	void StopSpeech();
	bool IsMusicPlaying();
	void Update();
	float GetSpeechAmplitude();
	float GetAverageSpeechAmplitude();
	int GetSpeechIndex();

protected:
	void InitializeFMOD();
	void ShutDownFMOD();
	void ValidateResult(FMOD_RESULT result);

protected:
	FMOD::System*						m_fmodSystem;
	FMOD::Channel*						m_musicChannel;
	FMOD::Channel*						m_speechChannel;
	std::map< std::string, SoundID >	m_registeredSoundIDs;
	std::vector< FMOD::Sound* >			m_registeredSounds;
	std::vector< AudioChannelHandle>	m_channels;
	std::deque<float>					m_previousAmplitudes;
};

