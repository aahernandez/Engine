#include "Engine/RHI/MeshBuilder.hpp"
#include "Engine/RHI/MeshDatabase.hpp"
#include "Engine/RHI/Motion.hpp"
#include "Engine/RHI/Skeleton.hpp"
#include "Engine/RHI/SkeletonDatabase.hpp"
#include "Engine/RHI/MotionDatabase.hpp"
#include "Engine/RHI/SkeletonInstance.hpp"
#include "ThirdParty/FBX/FBX.hpp"

std::map<std::string, MeshBuilder*> MeshDatabase::m_meshes;

MeshBuilder* MeshDatabase::CreateOrGetMesh(const std::string &meshName, Skeleton *skel, Motion *motion, SkeletonInstance *skelInst, SkeletonDatabase *skelDB, MotionDatabase *motionDB)
{
	if (DoesMeshExist(meshName))
		return GetMesh(meshName, *skel, *motion, *skelInst, skelDB, motionDB);
	return CreateMesh(meshName, skel, motion, skelInst, skelDB, motionDB);
}

void MeshDatabase::AddMeshBuilder(std::string meshName, MeshBuilder* meshBuilder)
{
	m_meshes[meshName] = meshBuilder;
}

void MeshDatabase::Destroy()
{
	std::map<std::string, MeshBuilder*>::iterator meshIter = m_meshes.begin();
	while (meshIter != m_meshes.end())
	{
		delete meshIter->second;
		meshIter->second = nullptr;
		meshIter = m_meshes.erase(meshIter);
	}
}

MeshBuilder* MeshDatabase::CreateMesh(const std::string &meshName, Skeleton *skel, Motion *motion, SkeletonInstance *skelInst, SkeletonDatabase *skelDB, MotionDatabase *motionDB)
{
	std::string meshNameWhole = meshName;
	meshNameWhole += "Whole";
	m_meshes[meshNameWhole] = new MeshBuilder();

	std::string filePath = "Data/Models/FBX/";
	filePath += meshName;
	filePath += ".fbx";
	
	FbxLoadMesh(m_meshes[meshNameWhole], *skel, *motion, *skelInst, filePath.c_str(), meshName.c_str(), skelDB, motionDB);

	return m_meshes[meshNameWhole];
}

MeshBuilder* MeshDatabase::GetMesh(const std::string &meshName, Skeleton &skel, Motion &motion, SkeletonInstance &skelInst, SkeletonDatabase *skelDB, MotionDatabase *motionDB)
{
	skel = *skelDB->CreateOrGetSkeleton(meshName);
	motion = *motionDB->CreateOrGetMotion(meshName, &skel);
	skelInst = SkeletonInstance(&skel);
	motion.m_currentTime = 0.f;

	std::string meshNameWhole = meshName;
	meshNameWhole += "Whole";
	return m_meshes[meshNameWhole];
}

bool MeshDatabase::DoesMeshExist(const std::string &meshName)
{
	std::string meshNameWhole = meshName;
	meshNameWhole += "Whole";
	if (m_meshes.count(meshNameWhole) > 0)
		return true;
	return false;
}

