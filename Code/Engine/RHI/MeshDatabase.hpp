#pragma once

#include <map>
#include <string>

class MeshBuilder;
class SkeletonInstance;
class SkeletonDatabase;
class MotionDatabase;
class Skeleton;
class Motion;

class MeshDatabase
{
public:
	static std::map<std::string, MeshBuilder*> m_meshes;

	MeshBuilder* CreateOrGetMesh(const std::string &meshName, Skeleton *skel, Motion *motion, SkeletonInstance *skelInst, SkeletonDatabase *skelDB, MotionDatabase *motionDB);

	static void AddMeshBuilder(std::string meshName, MeshBuilder* meshBuilder);

	static void Destroy();

private:
	MeshBuilder* CreateMesh(const std::string &meshName, Skeleton *skel, Motion *motion, SkeletonInstance *skelInst, SkeletonDatabase *skelDB, MotionDatabase *motionDB);
	MeshBuilder* GetMesh(const std::string &meshName, Skeleton &skel, Motion &motion, SkeletonInstance &skelInst, SkeletonDatabase *skelDB, MotionDatabase *motionDB);
	bool DoesMeshExist(const std::string &meshName);
};