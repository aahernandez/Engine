#pragma once
#include "ThirdParty/PocketSphinx/pocketsphinx.h"

#include <string>
#include <vector>

struct TimedWord_t
{
	std::string word;
	float startTime;
	float endTime;
};

class SpeechToTextSystem
{
public:
	ps_decoder_t *ps;
	cmd_ln_t *config;

	SpeechToTextSystem();
	~SpeechToTextSystem();

	std::string GetSpeechFromFile(const char *filename);
	std::string GetPhonemesFromFile(const char *filename);
	void GetPhonemeTimesFromFile(const char *filename, std::vector<TimedWord_t*> &timedWords);
private:
	std::string GetTextFromFile(const char *filename);
	void GetTimedTextFromFile(const char *filename, std::vector<TimedWord_t*> &timedWords);
	void LoadTimedWords(const char *filename, std::vector<TimedWord_t*> &timedWords);
	void SaveTimedWords(const char *filename, std::vector<TimedWord_t*> &timedWords);
};