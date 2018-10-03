#if defined(TOOLS_BUILD) 

#include "Engine/RHI/MeshBuilder.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/DeveloperConsole.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/RHI/Skeleton.hpp"
#include "Engine/RHI/Motion.hpp"
#include "Engine/RHI/SkeletonInstance.hpp"
#include "Engine/RHI/SkeletonDatabase.hpp"
#include "Engine/RHI/MotionDatabase.hpp"
#include "Engine/RHI/MeshDatabase.hpp"
#include "ThirdParty/FBX/FBX.hpp"

#define INVALID_JOINT_INDEX -1

void FbxListFile(char const *filename)
{
	FbxScene *scene = FbxLoadScene(filename, true);
	ASSERT_OR_DIE(scene != nullptr, Stringf("Failed to load scene: %s", filename));
	g_theDevConsole->ConsolePrintf("Listing File: %s", filename);
	FbxListScene(scene);
	FbxListTextures(scene);
	FbxUnloadScene(scene);
}

FbxScene* FbxLoadScene(char const *filename, bool import_embedded)
{
	FbxManager *fbx_manager = FbxManager::Create();
	ASSERT_OR_DIE(fbx_manager != nullptr, "Could not create FBX Manager.");

	FbxIOSettings *io_settings = FbxIOSettings::Create(fbx_manager, IOSROOT);
	io_settings->SetBoolProp(IMP_FBX_EXTRACT_EMBEDDED_DATA, import_embedded);
	io_settings->SetBoolProp(IMP_FBX_MATERIAL, import_embedded);

	fbx_manager->SetIOSettings(io_settings);


	FbxImporter *importer = FbxImporter::Create(fbx_manager, "");

	bool result = importer->Initialize(filename, -1, io_settings);

	if (result) {
		FbxScene *scene = FbxScene::Create(fbx_manager, "");

		result = importer->Import(scene);
		importer->Destroy();
		importer = nullptr;

		if (result) {
			return scene;
		}
	}

	FBX_SAFE_DESTROY(io_settings);
	FBX_SAFE_DESTROY(importer);
	FBX_SAFE_DESTROY(fbx_manager);

	return nullptr;
}

void FbxUnloadScene(FbxScene *scene)
{
	ASSERT_OR_DIE(scene != nullptr, "Unable to unload scene.");

	FbxManager *manager = scene->GetFbxManager();
	FbxIOSettings *io_settings = manager->GetIOSettings();

	FBX_SAFE_DESTROY(scene);
	FBX_SAFE_DESTROY(io_settings);
	FBX_SAFE_DESTROY(manager);
}

void FbxPrintNode(FbxNode *node, int depth)
{
	g_theDevConsole->ConsolePrintf("%*sNode [%s]", depth, " ", node->GetName());
	for (int i = 0; i < node->GetNodeAttributeCount(); i++) {
		FbxPrintAttribute(node->GetNodeAttributeByIndex(i), depth);
	}

	for (long int i = 0; i < node->GetChildCount(); ++i) {
		FbxPrintNode(node->GetChild(i), depth + 1);
	}
}

void FbxListScene(FbxScene *scene)
{
	FbxNode *root = scene->GetRootNode();
	FbxPrintNode(root, 0);
}

void FbxPrintAttribute(FbxNodeAttribute* pAttribute, int depth)
{
	ASSERT_OR_DIE(pAttribute != nullptr, "Unable to print attribute.");

	FbxNodeAttribute::EType type = pAttribute->GetAttributeType();

	char const* typeName = GetAttributeTypeName(type);
	char const* attrName = pAttribute->GetName();

	g_theDevConsole->ConsolePrintf("%*s- type='%s' name='%s'", depth, " ", typeName, attrName);
}

char const* GetAttributeTypeName(FbxNodeAttribute::EType type)
{
	switch (type) {
	case FbxNodeAttribute::eUnknown: return "unidentified";
	case FbxNodeAttribute::eNull: return "null";
	case FbxNodeAttribute::eMarker: return "marker";
	case FbxNodeAttribute::eSkeleton: return "skeleton";
	case FbxNodeAttribute::eMesh: return "mesh";
	case FbxNodeAttribute::eNurbs: return "nurbs";
	case FbxNodeAttribute::ePatch: return "patch";
	case FbxNodeAttribute::eCamera: return "camera";
	case FbxNodeAttribute::eCameraStereo: return "stereo";
	case FbxNodeAttribute::eCameraSwitcher: return "camera switcher";
	case FbxNodeAttribute::eLight: return "light";
	case FbxNodeAttribute::eOpticalReference: return "optical reference";
	case FbxNodeAttribute::eOpticalMarker: return "marker";
	case FbxNodeAttribute::eNurbsCurve: return "nurbs curve";
	case FbxNodeAttribute::eTrimNurbsSurface: return "trim nurbs surface";
	case FbxNodeAttribute::eBoundary: return "boundary";
	case FbxNodeAttribute::eNurbsSurface: return "nurbs surface";
	case FbxNodeAttribute::eShape: return "shape";
	case FbxNodeAttribute::eLODGroup: return "lodgroup";
	case FbxNodeAttribute::eSubDiv: return "subdiv";
	default: return "unknown";
	}
}


bool FbxLoadMesh(MeshBuilder *mb, Skeleton &skeleton, Motion &motion, SkeletonInstance &skeletonInstance, char const *filepath, char const *filename, SkeletonDatabase *skelDB, MotionDatabase *motionDB)
{
	skeleton = *skelDB->CreateOrGetSkeleton(filename);//  FbxLoadSkeleton(skeleton, filename);
	motion = *motionDB->CreateOrGetMotion(filename, &skeleton);//  FbxLoadMotion(motion, skeleton, filename);
	skeletonInstance = SkeletonInstance(&skeleton);
	motion.m_currentTime = 0.f;

	FbxScene *scene = FbxLoadScene(filepath, false);
	if (scene == nullptr)
	{
		return false;
	}

	ConvertSceneToEngineBasis(scene);
	TriangulateScene(scene);

	mb->Clear();

	MatrixStack mat_stack;

	FbxNode *root = scene->GetRootNode();

	ImportMeshes(mb, root, mat_stack, &skeleton);

	FbxUnloadScene(scene);

	FlipX(mb);
	return true;
}


bool FbxLoadMeshFromFile(MeshBuilder *mb, Skeleton &skeleton, Motion &motion, SkeletonInstance &skeletonInstance, char const *filepath)
{
	FbxLoadSkeleton(&skeleton, filepath);
	FbxLoadMotion(&motion, &skeleton, filepath);
	skeletonInstance = SkeletonInstance(&skeleton);
	motion.m_currentTime = 0.f;

	FbxScene *scene = FbxLoadScene(filepath, false);
	if (scene == nullptr)
	{
		return false;
	}

	ConvertSceneToEngineBasis(scene);
	TriangulateScene(scene);

	mb->Clear();

	MatrixStack mat_stack;

	FbxNode *root = scene->GetRootNode();

	ImportMeshes(mb, root, mat_stack, &skeleton);

	FbxUnloadScene(scene);

	FlipX(mb);
	return true;
}


void ConvertSceneToEngineBasis(FbxScene *scene)
{
	fbxsdk::FbxAxisSystem local_system(
		(fbxsdk::FbxAxisSystem::EUpVector) fbxsdk::FbxAxisSystem::EUpVector::eYAxis,
		(fbxsdk::FbxAxisSystem::EFrontVector) fbxsdk::FbxAxisSystem::EFrontVector::eParityOdd, 
		(fbxsdk::FbxAxisSystem::ECoordSystem) fbxsdk::FbxAxisSystem::ECoordSystem::eRightHanded);

	FbxAxisSystem scene_system = scene->GetGlobalSettings().GetAxisSystem();
	if (scene_system != local_system) {
		local_system.ConvertScene(scene);
	}
}

void TriangulateScene(FbxScene *scene)
{
	FbxGeometryConverter converter(scene->GetFbxManager());
	converter.Triangulate(scene, true);
}

void FlipX(MeshBuilder *mb)
{
	mb;
// 	mb->FlipXofVertexData();
// 	mb->FlipYofVertexData();
// 	mb->FlipZofVertexData();
}

void FlipX(Skeleton *skeleton)
{
	skeleton->FlipXofVertexData();
}

void ImportMeshes(MeshBuilder* out, FbxNode *node, MatrixStack &mat_stack, const Skeleton* skeleton)
{
	Matrix4 transform = GetNodeWorldTransform(node);
	mat_stack.PushDirect(transform);

	int attrib_count = node->GetNodeAttributeCount();

// 	DebuggerPrintf("Num Attributes: %i\n", attrib_count);
// 	DebuggerPrintf("Num Attributes: %s\n", node->GetName());

	for (int ai = 0; ai < attrib_count; ++ai)
	{
		FbxNodeAttribute *attrib = node->GetNodeAttributeByIndex(ai);

		if (attrib->GetAttributeType() == FbxNodeAttribute::eMesh)
		{
			std::string nodeName = node->GetName();
			ImportMesh(nodeName, out, mat_stack, (FbxMesh*)attrib, skeleton);
		}
	}

	int child_count = node->GetChildCount();

	for (int ci = 0; ci < child_count; ++ci)
	{
		FbxNode *child = node->GetChild(ci);
		ImportMeshes(out, child, mat_stack, skeleton);
	}

	mat_stack.Pop();
}

Matrix4 GetNodeWorldTransform(FbxNode *node)
{
	FbxAMatrix fbx_mat = node->EvaluateGlobalTransform();
	return ToEngineMatrix(fbx_mat);
}

Matrix4 ToEngineMatrix( FbxMatrix const &fbx_mat )
{
	Matrix4 flip_x = Matrix4::CreateFromBasis(-Vector3::RIGHT, Vector3::UP, Vector3::FORWARD);

	Matrix4 ret = Matrix4::CreateFromRows( ToVec4(fbx_mat.mData[0]), ToVec4(fbx_mat.mData[1]), ToVec4(fbx_mat.mData[2]), ToVec4(fbx_mat.mData[3]) );

	return flip_x * ret * flip_x;
}

static bool ImportMesh(std::string nodeName, MeshBuilder* out, MatrixStack &mat_stack, FbxMesh *mesh, const Skeleton *skeleton)
{
	MeshBuilder *meshBuiler = new MeshBuilder();
	ASSERT_OR_DIE(mesh->IsTriangleMesh(), "Mesh is not a triangle mesh.");

	int32_t poly_count = mesh->GetPolygonCount();
	if (poly_count <= 0) {
		return false;
	}

	std::vector<FbxSkinWeight_t> skin_weights;
	if (skeleton != nullptr) {
		CalculateSkinWeights(&skin_weights, mesh, skeleton);
	}

	Matrix4 geo_trans = GetGeometricTransform(mesh->GetNode());
	mat_stack.Push(geo_trans);

	Matrix4 transform = mat_stack.GetTop();

	unsigned int indexCount = 0;

	for (int32_t poly_idx = 0; poly_idx < poly_count; ++poly_idx) 
	{
		int32_t vert_count = mesh->GetPolygonSize(poly_idx);
		ASSERT_OR_DIE(vert_count == 3, "Triangle Mesh does not have 3 vertices.");

		for (int32_t vert_idx = 0; vert_idx < vert_count; ++vert_idx) 
		{
			FbxImportVertex(meshBuiler, transform, mesh, poly_idx, vert_idx, &skin_weights, indexCount);
			indexCount++;
		}
	}

	out->End();
	mat_stack.Pop();
	MeshDatabase::AddMeshBuilder(nodeName, meshBuiler);
	return true;
}

Matrix4 GetGeometricTransform(FbxNode *node)
{
	Matrix4 ret;
	ret.MakeIdentity();

	if ((node != nullptr) && (node->GetNodeAttribute() != nullptr)) 
	{
		FbxEuler::EOrder order;

		node->GetRotationOrder(FbxNode::eSourcePivot, order);

		FbxVector4 const geo_trans = node->GetGeometricTranslation(FbxNode::eSourcePivot);
		FbxVector4 const geo_rot = node->GetGeometricRotation(FbxNode::eSourcePivot);
		FbxVector4 const geo_scale = node->GetGeometricScaling(FbxNode::eSourcePivot);

		FbxAMatrix geo_mat;
		FbxAMatrix mat_rot;
		mat_rot.SetR(geo_rot, order);
		geo_mat.SetTRS(geo_trans, mat_rot, geo_scale);

		ret = ToEngineMatrix(geo_mat);
	}

	return ret;
}

bool GetNormal(Vector3 *out, FbxMesh *mesh, int poly_idx, int vert_idx)
{
	FbxGeometryElementNormal *element = mesh->GetElementNormal(0);
	if (element == nullptr) 
	{
		return false;
	}

	int elem_idx = 0;
	switch (element->GetMappingMode()) 
	{
	case FbxGeometryElement::eByControlPoint: 
	{
		elem_idx = mesh->GetPolygonVertex(poly_idx, vert_idx);
	} break;

	case FbxGeometryElement::eByPolygonVertex: 
	{
		elem_idx = mesh->GetPolygonVertexIndex(poly_idx) + vert_idx;
	} break;

	default:
		ASSERT_OR_DIE(false, "Unknown mapping mode");
	}

	switch (element->GetReferenceMode()) 
	{
	case FbxGeometryElement::eDirect: 
	{
		if (elem_idx < element->GetDirectArray().GetCount()) 
		{
			*out = ToVec3(element->GetDirectArray().GetAt(elem_idx));
			return true;
		}
	} break;

	case FbxGeometryElement::eIndexToDirect: {
		if (elem_idx < element->GetIndexArray().GetCount()) 
		{
			int index = element->GetIndexArray().GetAt(elem_idx);
			*out = ToVec3(element->GetDirectArray().GetAt(index));
			return true;
		}
	} break;

	default:
		ASSERT_OR_DIE(false, "Unknown reference type");
	}
	return false;
}

void FbxImportVertex(MeshBuilder *out, Matrix4 const &transform, FbxMesh *mesh, int32_t poly_idx, int32_t vert_idx, std::vector<FbxSkinWeight_t> *skinWeights, unsigned int currentIndex)
{
	Vector2 uv;
	if (FbxGetUVs(&uv, mesh, poly_idx, vert_idx))
	{
		uv = transform.TransformVector( Vector4(uv.x, uv.y, 0.f, 0.f) ).xy();
		uv.y = 1 - uv.y;
		out->SetUV(uv);
	}

	Vector3 normal;
	if (FbxGetNormal(&normal, mesh, poly_idx, vert_idx))
	{
		normal = transform.TransformVector(Vector4(normal.x, normal.y, normal.z, 0.f)).xyz();
		out->SetNormal(normal);
	}

	int controlIndex = INT_MAX;
	if (FbxGetControlIndex(controlIndex, mesh, poly_idx, vert_idx))
	{
		UIntVector4 indices = (*skinWeights)[controlIndex].indices;
		out->SetBoneIndices(indices);
		Vector4 weights = (*skinWeights)[controlIndex].weights;
		out->SetBoneWeights(weights);
	}

	out->SetIndex(currentIndex);

	Vector3 position;
	if (FbxGetPosition(&position, transform, mesh, poly_idx, vert_idx))
	{
		out->AddVertex(position);
	}
}

bool FbxGetPosition(Vector3 *out_pos, Matrix4 const &transform, FbxMesh *mesh, int poly_idx, int vert_idx)
{
	int ctrl_idx = mesh->GetPolygonVertex(poly_idx, vert_idx);
	if (ctrl_idx < 0) {
		return false;
	}

	FbxVector4 fbx_pos = mesh->GetControlPointAt(ctrl_idx);

	Vector3 pos = ToVec4(fbx_pos).xyz();
	*out_pos = transform.TransformPosition(pos);

	return true;
}

Vector4 ToVec4(const FbxVector4 &fbxVec4)
{
	return Vector4((float)fbxVec4.mData[0], (float)fbxVec4.mData[1], (float)fbxVec4.mData[2], (float)fbxVec4.mData[3]);
}

Vector4 ToVec4(const FbxDouble4 &fbxDouble4)
{
	return Vector4((float)fbxDouble4.mData[0], (float)fbxDouble4.mData[1], (float)fbxDouble4.mData[2], (float)fbxDouble4.mData[3]);
}

Vector3 ToVec3(const FbxVector4 &fbxVec4)
{
	return Vector3((float)fbxVec4.mData[0], (float)fbxVec4.mData[1], (float)fbxVec4.mData[2]);
}

bool FbxLoadSkeleton( Skeleton *skel, char const *filename )
{
	FbxScene *fbx_scene = FbxLoadScene( filename, false );
	if (nullptr == fbx_scene) 
	{
		return false;
	}

	ConvertSceneToEngineBasis(fbx_scene);

	skel->Clear();

	FbxNode *root = fbx_scene->GetRootNode();

	FbxPose *pose = GetBindPose(fbx_scene);

	ImportSkeleton( skel, root, nullptr, nullptr, pose );

	FlipX(skel);

	FbxUnloadScene( fbx_scene );

	return (skel->GetJointCount() > 0U);
}

FbxPose* GetBindPose( FbxScene *scene )
{
	int pose_count = scene->GetPoseCount();
	for (int i = 0; i < pose_count; ++i) 
	{
		FbxPose *pose = scene->GetPose(i);
		if (pose->IsBindPose()) 
		{
			int count = pose->GetCount();
			for (int j = 0; j < count; ++j) 
			{
				FbxNode *node = pose->GetNode(j);
				g_theDevConsole->ConsolePrintf("Pose Node: %s", node->GetName());
			}

			return pose;
		}
	}

	return nullptr;
}

void ImportSkeleton(Skeleton *out, FbxNode *node, FbxSkeleton *root_bone, FbxSkeleton *parent_bone, FbxPose *pose)
{
	for (int i = 0; i < node->GetNodeAttributeCount(); ++i) 
	{

		FbxNodeAttribute *na = node->GetNodeAttributeByIndex(i);
		if (na->GetAttributeType() == FbxNodeAttribute::eSkeleton) 
		{

			FbxSkeleton *skel = (FbxSkeleton*) na;
			FbxSkeleton::EType type = skel->GetSkeletonType();
			if ((type == FbxSkeleton::eRoot) || (type == FbxSkeleton::eEffector)) 
			{
				ASSERT_OR_DIE(root_bone != nullptr, "The root bone is null.");
				root_bone = skel;
			}

			int pose_node_idx = -1;
			if (pose != nullptr) 
			{
				pose_node_idx = pose->Find( node );
			}

			Matrix4 bone_transform = Matrix4::IDENTITY;
			if (pose_node_idx != -1) 
			{
				bool is_local = pose->IsLocalMatrix( pose_node_idx );
				ASSERT_OR_DIE(is_local == false, "Pose transform is not local");

				FbxMatrix fbx_matrix = pose->GetMatrix( pose_node_idx );
				bone_transform = ToEngineMatrix( fbx_matrix );
			} 
			else 
			{
				if (parent_bone == nullptr) 
				{
					bone_transform = ToEngineMatrix( node->EvaluateGlobalTransform() );
				} 
				else 
				{
					Matrix4 local = ToEngineMatrix( node->EvaluateLocalTransform() );
					Matrix4 ptrans = out->GetJointTransform(GetBoneName(parent_bone));
					bone_transform = local * ptrans;
				}
			}

			out->AddJoint(GetBoneName(skel), (parent_bone != nullptr) ? GetBoneName(parent_bone) : nullptr, bone_transform);

			parent_bone = skel;
			break;
		}
	}

	for (int i = 0; i < node->GetChildCount(); ++i) 
	{
		ImportSkeleton( out, node->GetChild(i), root_bone, parent_bone, pose );
	}
}

static char const* GetNodeName(FbxNode const *node)
{
	if (nullptr == node) 
	{
		return "";
	} 
	else 
	{
		return node->GetName();
	}
}

static char const* GetBoneName(FbxSkeleton const *skel) 
{
	if (nullptr == skel) 
	{
		return nullptr; 
	} 

	char const *node_name = GetNodeName(skel->GetNode());
	if (node_name != nullptr && node_name[0] != '\0' )
	{
		return skel->GetNode()->GetName();
	} 
	else 
	{
		return skel->GetName();
	}
}

Matrix4 GetNodeWorldTransformAtTime(FbxNode *node, FbxTime time)
{
	if (node == nullptr) {
		return Matrix4::IDENTITY;
	}

	FbxMatrix fbx_mat = node->EvaluateGlobalTransform(time);
	return ToEngineMatrix(fbx_mat);
}

static bool FbxImportMotion(Motion *motion,	Skeleton const *skeleton, FbxScene *scene, FbxAnimStack *anim, float framerate)
{
	scene->SetCurrentAnimationStack(anim);

	FbxTime advance;
	advance.SetSecondDouble((double)(1.0f / framerate));

	FbxTime local_start = anim->LocalStart;
	FbxTime local_end = anim->LocalStop;
	float local_start_fl = (float)local_start.GetSecondDouble();
	float local_end_fl = (float)local_end.GetSecondDouble();

	local_start_fl = GetMax(0.0f, local_start_fl);
	float duration_fl = local_end_fl - local_start_fl;

	if (duration_fl <= 0.0f) 
	{
		return false;
	}

	char const *motion_name = anim->GetName();

	motion->SetName(motion_name);
	motion->SetFramerate(framerate);
	motion->SetDuration(duration_fl);

	unsigned int frame_count = (unsigned int)Ceiling(duration_fl * framerate) + 1;

	unsigned int joint_count = skeleton->GetJointCount();
	for (unsigned int joint_idx = 0; joint_idx < joint_count; ++joint_idx)
	{
		std::string jointName = skeleton->GetJointName(joint_idx);
		char const *name = jointName.c_str();

		FbxNode *node = scene->FindNodeByName(name);
		FbxNode *parent = nullptr;
		int parent_idx = skeleton->GetJointParentIndex(joint_idx);
		ASSERT_OR_DIE(node != nullptr, "Node is nullptr.");

		if (parent_idx != INVALID_JOINT_INDEX)
		{
			jointName = skeleton->GetJointName(parent_idx);
			char const *parent_name = jointName.c_str();
			parent = scene->FindNodeByName(parent_name);
			ASSERT_OR_DIE(parent != nullptr, "Parent is nullptr.");
		}

		FbxTime eval_time = FbxTime(0);
		for (unsigned int frame_idx = 0; frame_idx < frame_count; ++frame_idx)
		{
			Pose *pose = motion->GetPose(frame_idx);
			Matrix4 joint_world = GetNodeWorldTransformAtTime(node, eval_time);
			Matrix4 joint_local = joint_world;
			if (nullptr != parent)
			{
				Matrix4 parent_world = GetNodeWorldTransformAtTime(parent, eval_time);
				Matrix4 parentWorldInverse = parent_world.GetInverse();
				joint_local = joint_world * parentWorldInverse;
			}

			pose->localTransforms.push_back(joint_local);
			eval_time += advance;
		}
	}

	return true;
}

bool FbxLoadMotion(Motion *motion, Skeleton const *skeleton, char const *filename, unsigned int framerate /*= 10*/)
{

	FbxScene *scene = FbxLoadScene(filename, false);
	if (scene == nullptr) 
	{
		return false;
	}
	ConvertSceneToEngineBasis(scene);

	float fr = (float)framerate;


	int anim_count = scene->GetSrcObjectCount<FbxAnimStack>();
	if (anim_count > 0U) 
	{
		FbxAnimStack *anim = scene->GetSrcObject<FbxAnimStack>(0);
		FbxImportMotion(motion, skeleton, scene, anim, fr);
	}

	FbxUnloadScene(scene);

	return (motion->GetDuration() > 0.0f);
}

void CalculateSkinWeights(std::vector<FbxSkinWeight_t> *skinWeights, FbxMesh const *mesh, Skeleton const *skeleton)
{
	unsigned int ctrl_count = (unsigned int)mesh->GetControlPointsCount();
	skinWeights->resize(ctrl_count);

	for (unsigned int i = 0; i < ctrl_count; ++i) 
	{
		(*skinWeights)[i].Reset();
	}

	if (skeleton == nullptr) 
	{
		skinWeights->clear();
		return;
	}

	int deformer_count = mesh->GetDeformerCount(FbxDeformer::eSkin);
	for (int didx = 0; didx < deformer_count; ++didx) 
	{
		FbxSkin *skin = (FbxSkin*)mesh->GetDeformer(didx, FbxDeformer::eSkin);
		if (nullptr == skin) 
		{
			continue;
		}

		int cluster_count = skin->GetClusterCount();
		for (int cidx = 0; cidx < cluster_count; ++cidx) 
		{
			FbxCluster *cluster = skin->GetCluster(cidx);
			FbxNode const *link_node = cluster->GetLink();

			if (nullptr == link_node) 
			{
				continue;
			}

			uint32_t joint_idx = skeleton->GetJointIndex(link_node->GetName());
			if (joint_idx == INVALID_JOINT_INDEX) 
			{
				continue;
			}

			int *indices = cluster->GetControlPointIndices();
			int index_count = cluster->GetControlPointIndicesCount();
			double *weights = cluster->GetControlPointWeights();

			for (int i = 0; i < index_count; ++i) 
			{
				int control_idx = indices[i];
				double weight = weights[i];

				FbxSkinWeight_t *skin_weight = &(*skinWeights)[control_idx];
				AddHighestWeight(skin_weight, (uint32_t)joint_idx, (float)weight);
			}
		}
	}
}

void AddHighestWeight(FbxSkinWeight_t *skin_weight, uint32_t bone_idx, float weight)
{
	float smallestWeight = FLT_MAX;
	int smallestWeightIndex = INT_MAX;

	float *allWeights = skin_weight->weights.GetAsFloatArray();

	for (int weightCount = 0; weightCount < 4; weightCount++)
	{
		float currentWeight = allWeights[weightCount];

		if (currentWeight < smallestWeight)
		{
			smallestWeight = currentWeight;
			smallestWeightIndex = weightCount;
		}
	}

	if (weight > smallestWeight)
	{
		switch (smallestWeightIndex)
		{
		case 0:
			skin_weight->weights.x = weight;
			skin_weight->indices.x = bone_idx;
			break;
		case 1:
			skin_weight->weights.y = weight;
			skin_weight->indices.y = bone_idx;
			break;
		case 2:
			skin_weight->weights.z = weight;
			skin_weight->indices.z = bone_idx;
			break;
		case 3:
			skin_weight->weights.w = weight;
			skin_weight->indices.w = bone_idx;
			break;
		}
	}
}

bool FbxGetControlIndex(int &controlIndex, FbxMesh *mesh, int poly_idx, int vert_idx)
{
	controlIndex = mesh->GetPolygonVertex(poly_idx, vert_idx);
	if (controlIndex < 0) {
		return false;
	}
	return true;
}

bool FbxGetNormal(Vector3 *outNorm, FbxMesh *mesh, int poly_idx, int vert_idx)
{
	FbxGeometryElementNormal *element = mesh->GetElementNormal(0);
	if (element == nullptr) 
	{
		return false;
	}

	int elem_idx = 0;
	switch (element->GetMappingMode())
	{
	case FbxGeometryElement::eByControlPoint: 
	{
		elem_idx = mesh->GetPolygonVertex(poly_idx, vert_idx);
	} 
	break;

	case FbxGeometryElement::eByPolygonVertex: 
	{
		elem_idx = mesh->GetPolygonVertexIndex(poly_idx) + vert_idx;
	} 
	break;

	default:
		return false;
	}

	switch (element->GetReferenceMode()) 
	{
	case FbxGeometryElement::eDirect: 
	{
		// this is just an array - use elem index as an index into this array
		if (elem_idx < element->GetDirectArray().GetCount()) 
		{
			*outNorm = ToVec3(element->GetDirectArray().GetAt(elem_idx));
			return true;
		}
	} 
	break;

	case FbxGeometryElement::eIndexToDirect: 
	{
		if (elem_idx < element->GetIndexArray().GetCount()) 
		{
			int index = element->GetIndexArray().GetAt(elem_idx);
			*outNorm = ToVec3(element->GetDirectArray().GetAt(index));
			return true;
		}
	} 
	break;

	default:
		return false;
	}
	return false;
}

bool FbxGetUVs(Vector2 *outUV, FbxMesh *mesh, int poly_idx, int vert_idx)
{
	FbxGeometryElementUV *element = mesh->GetElementUV(0);
	if (element == nullptr)
	{
		return false;
	}

	int elem_idx = 0;
	switch (element->GetMappingMode())
	{
	case FbxGeometryElement::eByControlPoint:
	{
		elem_idx = mesh->GetPolygonVertex(poly_idx, vert_idx);
	}
	break;

	case FbxGeometryElement::eByPolygonVertex:
	{
		elem_idx = mesh->GetPolygonVertexIndex(poly_idx) + vert_idx;
	}
	break;

	default:
		return false;
	}

	switch (element->GetReferenceMode())
	{
	case FbxGeometryElement::eDirect:
	{
		if (elem_idx < element->GetDirectArray().GetCount())
		{
			*outUV = ToVec2(element->GetDirectArray().GetAt(elem_idx));
			return true;
		}
	}
	break;

	case FbxGeometryElement::eIndexToDirect:
	{
		if (elem_idx < element->GetIndexArray().GetCount())
		{
			int index = element->GetIndexArray().GetAt(elem_idx);
			*outUV = ToVec2(element->GetDirectArray().GetAt(index));
			return true;
		}
	}
	break;

	default:
		return false;
	}
	return false;
}

Vector2 ToVec2(const FbxVector2 &fbxVec2)
{
	return Vector2((float)fbxVec2.mData[0], (float)fbxVec2.mData[1]);
}

void FbxListTextures(FbxScene *scene)
{
	int32_t tex_count = scene->GetTextureCount();
	g_theDevConsole->ConsolePrintf("fbx", " Scene contains %i textures.", tex_count);
	for (int32_t tex_idx = 0; tex_idx < tex_count; ++tex_idx) {

		FbxTexture *tex = scene->GetTexture(tex_idx);
		char const *name = tex->GetName();
		if (tex->Is<FbxFileTexture>()) {
			FbxFileTexture *file_tex = (FbxFileTexture*)tex;
			name = file_tex->GetFileName();
		}

		g_theDevConsole->ConsolePrintf("fbx", " - %s[%s]", tex->GetName(), name);
	}
}


#else

class MeshBuilder;
class Skeleton;
class Motion;
class SkeletonInstance;
class SkeletonDatabase;
class MotionDatabase;

void FbxListFile(char const *filename)
{
	filename;
}
bool FbxLoadMesh(MeshBuilder *mb, Skeleton &skeleton, Motion &motion, SkeletonInstance &skeletonInstance, char const *filepath, char const *filename, SkeletonDatabase *skelDB, MotionDatabase *motionDB)
{
	mb; skeleton; motion; skeletonInstance; filepath; filename; skelDB; motionDB; return false;
}
bool FbxLoadMeshFromFile(MeshBuilder *mb, Skeleton &skeleton, Motion &motion, SkeletonInstance &skeletonInstance, char const *filepath)
{
	mb; skeleton; motion; skeletonInstance; filepath; return false;
}
bool FbxLoadSkeleton(Skeleton *skel, char const *filename)
{
	skel; filename; return false;
}
bool FbxLoadMotion(Motion *motion, Skeleton const *skeleton, char const *filename, unsigned int framerate = 10)
{
	motion; skeleton; filename; framerate; return false;
}

#endif
