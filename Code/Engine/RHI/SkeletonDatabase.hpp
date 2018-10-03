#pragma once

#include <map>
#include <string>

class Skeleton;

class SkeletonDatabase
{
public:
	std::map<std::string, Skeleton*> m_skeletons;

	Skeleton* CreateOrGetSkeleton(const std::string &skeletonName);

private:
	Skeleton* CreateSkeleton(const std::string &skeletonName);
	Skeleton* GetSkeleton(const std::string &skeletonName);
	bool DoesSkeletonExist(const std::string &skeletonName);
};