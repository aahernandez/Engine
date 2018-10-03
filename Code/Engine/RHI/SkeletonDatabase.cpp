#include "ThirdParty/FBX/FBX.hpp"
#include "Engine/RHI/Skeleton.hpp"
#include "Engine/RHI/SkeletonDatabase.hpp"

Skeleton* SkeletonDatabase::CreateOrGetSkeleton(const std::string &skeletonName)
{
	if (DoesSkeletonExist(skeletonName))
		return GetSkeleton(skeletonName);
	return CreateSkeleton(skeletonName);
}

Skeleton* SkeletonDatabase::CreateSkeleton(const std::string &skeletonName)
{
	m_skeletons[skeletonName] = new Skeleton();

	std::string filePath = "Data/Models/FBX/";
	filePath += skeletonName;
	filePath += ".fbx";
	FbxLoadSkeleton(m_skeletons[skeletonName], filePath.c_str());

	return m_skeletons[skeletonName];
}

Skeleton* SkeletonDatabase::GetSkeleton(const std::string &skeletonName)
{
	return m_skeletons[skeletonName];
}

bool SkeletonDatabase::DoesSkeletonExist(const std::string &skeletonName)
{
	if (m_skeletons.count(skeletonName) > 0)
		return true;
	return false;
}
