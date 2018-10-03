#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/DeveloperConsole.hpp"
#include "Engine/Core/StringUtils.hpp"

std::string GetTimeAsString(float timeInSeconds)
{
	float newTime = timeInSeconds * ms;
	if (newTime < 1.f)
		return Stringf("%.0fus", timeInSeconds * us);
	else if (timeInSeconds < 1.f)
		return Stringf("%.2fms", newTime);
	else
		return Stringf("%.2fs", timeInSeconds);
}

std::string GetByteSizeAsString(unsigned int sizeInBytes)
{
	float newSize = sizeInBytes / KiB;
	if (newSize < 2.f)
		return Stringf("%i B", sizeInBytes);
	newSize = sizeInBytes / MiB;
	if (newSize < 2.f)
		return Stringf("%.3f KiB", sizeInBytes / KiB);
	newSize = sizeInBytes / GiB;
	if (newSize < 2.f)
		return Stringf("%.3f MiB", sizeInBytes / MiB);
	else
		return Stringf("%.3f GiB", newSize);
}

DeveloperConsole *g_theDevConsole = nullptr;