#include "Engine/Audio/SpeechToTextSystem.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Memory/LogSystem.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/FileUtils.hpp"
#include "Engine/Streams/FileBinaryStream.hpp"

SpeechToTextSystem::SpeechToTextSystem()
{
}

SpeechToTextSystem::~SpeechToTextSystem()
{
}

std::string SpeechToTextSystem::GetSpeechFromFile(const char *filename)
{
	config = cmd_ln_init(NULL, ps_args(), TRUE,
		"-hmm", "Data/LanguageModels/en-us/en-us",
		"-lm", "Data/LanguageModels/en-us/en-us.lm.bin",
		"-dict", "Data/LanguageModels/en-us/cmudict-en-us.dict",
		NULL);

	ASSERT_OR_DIE(config != NULL, "Failed to create config object in speech to text sytem.");

	ps = ps_init(config);
	ASSERT_OR_DIE(ps != NULL, "Failed to create recognizer in speech to text system.");

	return GetTextFromFile(filename);
}

std::string SpeechToTextSystem::GetPhonemesFromFile(const char *filename)
{
	config = cmd_ln_init(NULL, ps_args(), TRUE,
		"-hmm", "Data/LanguageModels/en-us/en-us",
		"-allphone", "Data/LanguageModels/en-us/en-us-phone.lm.bin",
		"-backtrace", "yes",
		"-beam", "1e-20",
		"-pbeam", "1e-20",
		"-lw", "2.0",
		NULL);

	ASSERT_OR_DIE(config != NULL, "Failed to create config object in speech to text sytem.");

	ps = ps_init(config);
	ASSERT_OR_DIE(ps != NULL, "Failed to create recognizer in speech to text system.");

	return GetTextFromFile(filename);
}

void SpeechToTextSystem::GetPhonemeTimesFromFile(const char *filename, std::vector<TimedWord_t*> &timedWords)
{
	std::string filenameString = "Data/Audio/";
	filenameString += GetFileNameWithoutPathAndFileType(filename);
	filenameString += ".fas";

	if (DoesFileExist(filenameString.c_str()))
	{
		LoadTimedWords(filenameString.c_str(), timedWords);
	}
	else
	{
		config = cmd_ln_init(NULL, ps_args(), TRUE,
			"-hmm", "Data/LanguageModels/en-us/en-us",
			"-allphone", "Data/LanguageModels/en-us/en-us-phone.lm.bin",
			"-backtrace", "yes",
			"-beam", "1e-20",
			"-pbeam", "1e-20",
			"-lw", "2.0",
			NULL);

		ASSERT_OR_DIE(config != NULL, "Failed to create config object in speech to text sytem.");

		ps = ps_init(config);
		ASSERT_OR_DIE(ps != NULL, "Failed to create recognizer in speech to text system.");

		GetTimedTextFromFile(filename, timedWords);
		SaveTimedWords(filenameString.c_str(), timedWords);
	}

}

std::string SpeechToTextSystem::GetTextFromFile(const char *filename)
{
	FILE *fh;
	char const *hyp;
	int16 buf[512];
	int rv;
	int32 score;

	fh = fopen(filename, "rb");
	if (fh == NULL)
	{
		DebuggerPrintf("Unable to open input file %s", filename);
		return "";
	}

	rv = ps_start_utt(ps);

	while (!feof(fh))
	{
		size_t nsamp;
		nsamp = fread(buf, 2, 512, fh);
		rv = ps_process_raw(ps, buf, nsamp, FALSE, FALSE);
	}

	rv = ps_end_utt(ps);

	hyp = ps_get_hyp(ps, &score);
	if (g_theLogSystem != nullptr)
		g_theLogSystem->LogTaggedPrintf("{SpeechToText", "Recognized: \"%s\"", hyp);

	fclose(fh);

	ps_free(ps);
	cmd_ln_free_r(config);

	return std::string(hyp);
}

void SpeechToTextSystem::GetTimedTextFromFile(const char *filename, std::vector<TimedWord_t*> &timedWords)
{
	FILE *fh;
	int16 buf[512];
	int rv;

	fh = fopen(filename, "rb");
	if (fh == NULL)
	{
		DebuggerPrintf("Unable to open input file %s", filename);
		return;
	}

	rv = ps_start_utt(ps);

	while (!feof(fh))
	{
		size_t nsamp;
		nsamp = fread(buf, 2, 512, fh);
		rv = ps_process_raw(ps, buf, nsamp, FALSE, FALSE);
	}

	rv = ps_end_utt(ps);

	int frame_rate = cmd_ln_int32_r(config, "-frate");
	ps_seg_t *iter = ps_seg_iter(ps);
	while (iter != NULL) {
		int32 sf, ef, pprob;

		ps_seg_frames(iter, &sf, &ef);
		pprob = ps_seg_prob(iter, NULL, NULL, NULL);

		std::string timedString = ps_seg_word(iter);
		float startTime = ((float)sf / frame_rate);
		float endTime = ((float)ef / frame_rate);
		TimedWord_t *timedWord = new TimedWord_t{ timedString, startTime, endTime };
		timedWords.push_back(timedWord);
		iter = ps_seg_next(iter);
	}

	fclose(fh);

	ps_free(ps);
	cmd_ln_free_r(config);
}

void SpeechToTextSystem::LoadTimedWords(const char *filename, std::vector<TimedWord_t*> &timedWords)
{
	FileBinaryStream stream;
	stream.OpenForRead(filename);

	int numTimedWords = 0;
	stream.ReadBytes(&numTimedWords, sizeof(int));
	timedWords.resize(numTimedWords);

	for (int timedWordCount = 0; timedWordCount < numTimedWords; timedWordCount++)
	{
		TimedWord_t *newTimedWord = new TimedWord_t{ "", 0.f, 0.f };
		timedWords[timedWordCount] = newTimedWord;

		int wordSize = 0;
		stream.ReadBytes(&wordSize, sizeof(int));
		if (wordSize > 0)
		{
			newTimedWord->word.resize(wordSize);
			stream.ReadBytes(&newTimedWord->word[0], wordSize);
			newTimedWord->word = newTimedWord->word.c_str();
		}

		stream.ReadBytes(&newTimedWord->startTime, sizeof(float));
		stream.ReadBytes(&newTimedWord->endTime, sizeof(float));

	}

	stream.Close();
}

void SpeechToTextSystem::SaveTimedWords(const char *filename, std::vector<TimedWord_t*> &timedWords)
{
	FileBinaryStream stream;
	stream.OpenForWrite(filename);

	int numTimedWords = (int)timedWords.size();
	stream.WriteBytes(&numTimedWords, sizeof(int));

	std::vector<TimedWord_t*>::iterator timedWordsIter;
	for (timedWordsIter = timedWords.begin(); timedWordsIter != timedWords.end(); ++timedWordsIter)
	{
		TimedWord_t *currentTimedWord = (*timedWordsIter);
		int wordSize = (int)currentTimedWord->word.size();
		stream.WriteBytes(&wordSize, sizeof(int));
		if (wordSize > 0)
			stream.WriteBytes(currentTimedWord->word.c_str(), wordSize);

		stream.WriteBytes(&currentTimedWord->startTime, sizeof(float));
		stream.WriteBytes(&currentTimedWord->endTime, sizeof(float));
	}

	stream.Close();
}
