#pragma once
#include <map>
#include <string>

class Motion;

class MotionDatabase
{
public:
	std::map<std::string, Motion*> m_motions;

	Motion* CreateOrGetMotion(const std::string &motionName, const Skeleton* skel);

private:
	Motion* CreateMotion(const std::string &motionName, const Skeleton* skel);
	Motion* GetMotion(const std::string &motionName);
	bool DoesMotionExist(const std::string &motionName);
};