#include "ThirdParty/FBX/FBX.hpp"
#include "Engine/RHI/Motion.hpp"
#include "Engine/RHI/MotionDatabase.hpp"

Motion* MotionDatabase::CreateOrGetMotion(const std::string &motionName, const Skeleton* skel)
{
	if (DoesMotionExist(motionName))
		return GetMotion(motionName);
	return CreateMotion(motionName, skel);
}

Motion* MotionDatabase::CreateMotion(const std::string &motionName, const Skeleton* skel)
{
	m_motions[motionName] = new Motion();

	std::string filePath = "Data/Models/FBX/";
	filePath += motionName;
	filePath += ".fbx";

	FbxLoadMotion(m_motions[motionName], skel, filePath.c_str());

	return m_motions[motionName];
}

Motion* MotionDatabase::GetMotion(const std::string &motionName)
{
	return m_motions[motionName];
}

bool MotionDatabase::DoesMotionExist(const std::string &motionName)
{
	if (m_motions.count(motionName) > 0)
		return true;
	return false;
}

