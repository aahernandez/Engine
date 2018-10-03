//---------------------------------------------------------------------------
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"


//---------------------------------------------------------------------------
// FMOD startup code based on "GETTING STARTED With FMOD Ex Programmer’s API for Windows" document
//	from the FMOD programming API at http://www.fmod.org/download/
//
AudioSystem::AudioSystem()
	: m_fmodSystem(nullptr)
{
	InitializeFMOD();
	m_musicChannel = nullptr;
	m_speechChannel = nullptr;
}


//-----------------------------------------------------------------------------------------------
AudioSystem::~AudioSystem()
{
	ShutDownFMOD();
}


//---------------------------------------------------------------------------
void AudioSystem::InitializeFMOD()
{
	const int MAX_AUDIO_DEVICE_NAME_LEN = 256;
	FMOD_RESULT result;
	unsigned int fmodVersion;
	int numDrivers;
	FMOD_SPEAKERMODE speakerMode;
	FMOD_CAPS deviceCapabilities;
	char audioDeviceName[MAX_AUDIO_DEVICE_NAME_LEN];

	// Create a System object and initialize.
	result = FMOD::System_Create(&m_fmodSystem);
	ValidateResult(result);

	result = m_fmodSystem->getVersion(&fmodVersion);
	ValidateResult(result);

	if (fmodVersion < FMOD_VERSION)
	{
		DebuggerPrintf("Engine/Audio SYSTEM ERROR!  Your FMOD .dll is of an older version (0x%08x == %d) than that the .lib used to compile this code (0x%08x == %d).\n", fmodVersion, fmodVersion, FMOD_VERSION, FMOD_VERSION);
	}

	result = m_fmodSystem->getNumDrivers(&numDrivers);
	ValidateResult(result);

	if (numDrivers == 0)
	{
		result = m_fmodSystem->setOutput(FMOD_OUTPUTTYPE_NOSOUND);
		ValidateResult(result);
	}
	else
	{
		result = m_fmodSystem->getDriverCaps(0, &deviceCapabilities, 0, &speakerMode);
		ValidateResult(result);

		// Set the user selected speaker mode.
		result = m_fmodSystem->setSpeakerMode(speakerMode);
		ValidateResult(result);

		if (deviceCapabilities & FMOD_CAPS_HARDWARE_EMULATED)
		{
			// The user has the 'Acceleration' slider set to off! This is really bad
			// for latency! You might want to warn the user about this.
			result = m_fmodSystem->setDSPBufferSize(1024, 10);
			ValidateResult(result);
		}

		result = m_fmodSystem->getDriverInfo(0, audioDeviceName, MAX_AUDIO_DEVICE_NAME_LEN, 0);
		ValidateResult(result);

		if (strstr(audioDeviceName, "SigmaTel"))
		{
			// Sigmatel sound devices crackle for some reason if the format is PCM 16bit.
			// PCM floating point output seems to solve it.
			result = m_fmodSystem->setSoftwareFormat(48000, FMOD_SOUND_FORMAT_PCMFLOAT, 0, 0, FMOD_DSP_RESAMPLER_LINEAR);
			ValidateResult(result);
		}
	}

	result = m_fmodSystem->init(100, FMOD_INIT_NORMAL, 0);
	if (result == FMOD_ERR_OUTPUT_CREATEBUFFER)
	{
		// Ok, the speaker mode selected isn't supported by this sound card. Switch it
		// back to stereo...
		result = m_fmodSystem->setSpeakerMode(FMOD_SPEAKERMODE_STEREO);
		ValidateResult(result);

		// ... and re-init.
		result = m_fmodSystem->init(100, FMOD_INIT_NORMAL, 0);
		ValidateResult(result);
	}
}


//-----------------------------------------------------------------------------------------------
void AudioSystem::ShutDownFMOD()
{
	//	FMOD_RESULT result = FMOD_OK;
	//	result = FMOD_System_Close( m_fmodSystem );
	//	result = FMOD_System_Release( m_fmodSystem );
	m_fmodSystem = nullptr;
}


//---------------------------------------------------------------------------
SoundID AudioSystem::CreateOrGetSound(const std::string& soundFileName)
{
	std::map< std::string, SoundID >::iterator found = m_registeredSoundIDs.find(soundFileName);
	if (found != m_registeredSoundIDs.end())
	{
		return found->second;
	}
	else
	{
		FMOD::Sound* newSound = nullptr;
		m_fmodSystem->createSound(soundFileName.c_str(), FMOD_SOFTWARE, nullptr, &newSound);
		if (newSound)
		{
			SoundID newSoundID = (SoundID)m_registeredSounds.size();
			m_registeredSoundIDs[soundFileName] = newSoundID;
			m_registeredSounds.push_back(newSound);
			return newSoundID;
		}
	}

	return MISSING_SOUND_ID;
}


AudioChannelHandle AudioSystem::PlaySound(SoundID soundID, float volumeLevel /*= 1.f*/, SoundType soundType /*= SOUND_GAMEPLAY*/)
{
	unsigned int numSounds = m_registeredSounds.size();
	if (soundID < 0 || soundID >= numSounds)
		return nullptr;

	FMOD::Sound* sound = m_registeredSounds[soundID];
	if (!sound)
		return nullptr;

	FMOD::Channel* channelAssignedToSound = nullptr;
	if (soundType == SOUND_MUSIC)
	{
		StopMusic();
		m_fmodSystem->playSound(FMOD_CHANNEL_REUSE, sound, false, &m_musicChannel);
		m_musicChannel->setMode(FMOD_LOOP_NORMAL);
	}
	else
	{
		m_fmodSystem->playSound(FMOD_CHANNEL_FREE, sound, false, &channelAssignedToSound);
	}

	if (channelAssignedToSound)
	{
		channelAssignedToSound->setVolume(volumeLevel);
	}

	if (m_musicChannel)
	{
		m_musicChannel->setVolume(volumeLevel);
	}

	m_channels.push_back((AudioChannelHandle)channelAssignedToSound);

	return (AudioChannelHandle)channelAssignedToSound;
}

//---------------------------------------------------------------------------
void AudioSystem::StopChannel(AudioChannelHandle channel)
{
	if (channel != nullptr)
	{
		FMOD::Channel* fmodChannel = (FMOD::Channel*) channel;
		fmodChannel->stop();
	}
}


void AudioSystem::StopMusic()
{
	m_musicChannel->stop();
	m_musicChannel = nullptr;
}

void AudioSystem::StopSpeech()
{
	m_speechChannel->stop();
	m_speechChannel = nullptr;
}

bool AudioSystem::IsMusicPlaying()
{
	if (m_musicChannel != nullptr)
		return true;
	return false;
}

AudioChannelHandle AudioSystem::PlayGameplaySound(SoundID soundID, float volumeLevel)
{
	unsigned int numSounds = m_registeredSounds.size();
	if (soundID < 0 || soundID >= numSounds)
		return nullptr;

	FMOD::Sound* sound = m_registeredSounds[soundID];
	if (!sound)
		return nullptr;

	FMOD::Channel* channelAssignedToSound = nullptr;
	m_fmodSystem->playSound(FMOD_CHANNEL_FREE, sound, false, &channelAssignedToSound);

	if (channelAssignedToSound)
	{
		channelAssignedToSound->setVolume(volumeLevel);
	}

	if (m_musicChannel)
	{
		m_musicChannel->setVolume(volumeLevel);
	}

	m_channels.push_back((AudioChannelHandle)channelAssignedToSound);

	return (AudioChannelHandle)channelAssignedToSound;
}

void AudioSystem::PlayMusic(SoundID soundID, float volumeLevel /*= 1.f*/)
{
	unsigned int numSounds = m_registeredSounds.size();
	if (soundID < 0 || soundID >= numSounds)
		return;

	FMOD::Sound* sound = m_registeredSounds[soundID];
	if (!sound)
		return;

	StopMusic();
	m_fmodSystem->playSound(FMOD_CHANNEL_REUSE, sound, false, &m_musicChannel);
	m_musicChannel->setMode(FMOD_LOOP_NORMAL);

	m_musicChannel->setVolume(volumeLevel);
}

void AudioSystem::PlaySpeech(SoundID soundID, float volumeLevel /*= 1.f*/)
{
	unsigned int numSounds = m_registeredSounds.size();
	if (soundID < 0 || soundID >= numSounds)
		return;

	FMOD::Sound* sound = m_registeredSounds[soundID];
	if (!sound)
		return;

	StopSpeech();
	m_fmodSystem->playSound(FMOD_CHANNEL_REUSE, sound, false, &m_speechChannel);
	m_speechChannel->setMode(FMOD_LOOP_OFF);

	m_speechChannel->setVolume(volumeLevel);
}

//---------------------------------------------------------------------------
void AudioSystem::Update()
{
	FMOD_RESULT result = m_fmodSystem->update();
	ValidateResult(result);
}

float AudioSystem::GetSpeechAmplitude()
{
	bool isPlaying = false;
	m_speechChannel->isPlaying(&isPlaying);
	if (!isPlaying)
		return 1.f;

	const unsigned int samplesPerSec = 256;
	float data1[samplesPerSec];
	float data2[samplesPerSec];

	m_speechChannel->getWaveData(&data1[0], samplesPerSec, 0);
	m_speechChannel->getWaveData(&data2[0], samplesPerSec, 1);

	float averageAmplitude = 0.f;
	for (int waveDataCount = 0; waveDataCount < samplesPerSec; waveDataCount++)
	{
		averageAmplitude += data1[waveDataCount];
		averageAmplitude += data2[waveDataCount];
	}

	const float divisor = 1.f / (samplesPerSec * 2.f);
	averageAmplitude = averageAmplitude * divisor;
	if (averageAmplitude <= 0.f)
		averageAmplitude = 0.00001f;

	m_previousAmplitudes.push_back(averageAmplitude);

	if (m_previousAmplitudes.size() > 20)
		m_previousAmplitudes.pop_front();

	averageAmplitude = GetAverageSpeechAmplitude();

	return averageAmplitude;
}

float AudioSystem::GetAverageSpeechAmplitude()
{
	float averageAmplitude = 0;
	unsigned int prevAmpsSize = (unsigned int) m_previousAmplitudes.size();
	for (unsigned int ampIndex = 0; ampIndex < prevAmpsSize; ampIndex++)
	{
		averageAmplitude += m_previousAmplitudes[ampIndex];
	}

	averageAmplitude = averageAmplitude / (float)prevAmpsSize;
	return averageAmplitude;
}

int AudioSystem::GetSpeechIndex()
{
	int currentIndex;

	m_speechChannel->getIndex(&currentIndex);

	return currentIndex;
}

//---------------------------------------------------------------------------
void AudioSystem::ValidateResult(FMOD_RESULT result)
{
	GUARANTEE_OR_DIE(result == FMOD_OK, Stringf("Engine/Audio SYSTEM ERROR: Got error result code %d.\n", result));
}


