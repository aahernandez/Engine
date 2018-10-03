#pragma once

class MeshBuilder;
class Skeleton;
class Motion;
class SkeletonInstance;
class SkeletonDatabase;
class MotionDatabase;

#if !defined(TOOLS_BUILD) 

void FbxListFile(char const *filename);
bool FbxLoadMesh(MeshBuilder *mb, Skeleton &skeleton, Motion &motion, SkeletonInstance &skeletonInstance, char const *filepath, char const *filename, SkeletonDatabase *skelDB, MotionDatabase *motionDB);
bool FbxLoadMeshFromFile(MeshBuilder *mb, Skeleton &skeleton, Motion &motion, SkeletonInstance &skeletonInstance, char const *filepath);
bool FbxLoadSkeleton(Skeleton *skel, char const *filename);
bool FbxLoadMotion(Motion *motion, Skeleton const *skeleton, char const *filename, unsigned int framerate = 10);

#else

// #define __PLACEMENT_NEW_INLINE
#include <fbxsdk.h>
// #pragma comment(lib, "libfbxsdk-md.lib")

#include "Engine/Math/MatrixStack.hpp"
#include "Engine/Math/UIntVector4.hpp"

#if !defined( __TOOLS_FBXLOAD__ )
#define __TOOLS_FBXLOAD__


struct FbxSkinWeight_t
{
	FbxSkinWeight_t(){}

	void Reset()
	{
		indices = UIntVector4();
		weights = Vector4();
	}

	UIntVector4 indices;
	Vector4 weights;
};

void FbxListFile(char const *filename);
bool FbxLoadMesh(MeshBuilder *mb, Skeleton &skeleton, Motion &motion, SkeletonInstance &skeletonInstance, char const *filepath, char const *filename, SkeletonDatabase *skelDB, MotionDatabase *motionDB);
bool FbxLoadMeshFromFile(MeshBuilder *mb, Skeleton &skeleton, Motion &motion, SkeletonInstance &skeletonInstance, char const *filepath);
bool FbxLoadSkeleton(Skeleton *skel, char const *filename);
bool FbxLoadMotion( Motion *motion, Skeleton const *skeleton, char const *filename, unsigned int framerate = 10 );

FbxScene* FbxLoadScene(char const *filename, bool import_embedded);
void FbxUnloadScene(FbxScene *scene);
void FbxPrintAttribute(FbxNodeAttribute* pAttribute, int depth);
char const* GetAttributeTypeName(FbxNodeAttribute::EType type);
void FbxListScene(FbxScene *scene);
// bool FbxLoadMesh(MeshBuilder *mb, Skeleton *skeleton, char const *filename);
void ConvertSceneToEngineBasis(FbxScene *scene);
void TriangulateScene(FbxScene *scene);
void FlipX(MeshBuilder *mb);
void ImportMeshes(MeshBuilder *out, FbxNode *node, MatrixStack &mat_stack, const Skeleton *skeleton);
Matrix4 GetNodeWorldTransform(FbxNode *node);
Matrix4 ToEngineMatrix(FbxMatrix const &fbx_mat);
static bool ImportMesh(std::string meshName, MeshBuilder *out, MatrixStack &mat_stack, FbxMesh *mesh, const Skeleton *skeleton);
Matrix4 GetGeometricTransform(FbxNode *node);
void FbxImportVertex(MeshBuilder *out, Matrix4 const &transform, FbxMesh *mesh, int32_t poly_idx, int32_t vert_idx, std::vector<FbxSkinWeight_t> *skinWeights, unsigned int currentIndex);
bool FbxGetPosition(Vector3 *out_pos, Matrix4 const &transform, FbxMesh *mesh, int poly_idx, int vert_idx);
bool FbxGetNormal(Vector3 *outNorm, FbxMesh *mesh, int poly_idx, int vert_idx);
bool FbxGetUVs(Vector2 *outUV, FbxMesh *mesh, int poly_idx, int vert_idx);
bool FbxGetControlIndex(int &controlIndex, FbxMesh *mesh, int poly_idx, int vert_idx);
Vector4 ToVec4(const FbxVector4 &fbxVec4);
Vector4 ToVec4(const FbxDouble4 &fbxDouble4);
Vector3 ToVec3(const FbxVector4 &fbxVec4);
Vector2 ToVec2(const FbxVector2 &fbxVec4);
FbxPose* GetBindPose(FbxScene *scene);
static char const* GetBoneName(FbxSkeleton const *skel);
void ImportSkeleton(Skeleton *out, FbxNode *node, FbxSkeleton *root_bone, FbxSkeleton *parent_bone, FbxPose *pose);
static bool FbxImportMotion(Motion *motion,	Skeleton const *skeleton, FbxScene *scene, FbxAnimStack *anim, float framerate);
void CalculateSkinWeights(std::vector<FbxSkinWeight_t> *skinWeights, FbxMesh const *mesh, Skeleton const *skeleton);
void AddHighestWeight(FbxSkinWeight_t *skin_weight, uint32_t bone_idx, float weight);
void FbxListTextures(FbxScene *scene);

#endif

#endif
