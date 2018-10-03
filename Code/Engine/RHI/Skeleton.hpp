#pragma once
#include "Engine/Math/Vertex.hpp"
#include "Engine/Math/Matrix4.hpp"

#include <vector>
#include <string>

class VertexBuffer;
class IndexBuffer;
class SimpleRenderer;
class Motion;
class BinaryStream;
class FileBinaryStream;

struct Joint
{
	std::string name;
	std::string parentName;
	Matrix4 transform;
};

class Skeleton
{
public:
	std::vector<Joint> m_joints;

	Skeleton();
	~Skeleton();

	void Clear();
	void FlipXofVertexData();
	void AddJoint(char const *name, char const *parent_name, Matrix4 const &transform);
	unsigned int GetJointCount() const;
	int GetJointIndex(char const *name) const;
	int GetJointParentIndex(unsigned int jointIndex) const;
	std::string GetJointName(unsigned int index) const;
	Matrix4 GetJointTransform(unsigned int joint_idx) const;
	Matrix4 GetJointTransform(char const *name);
	void GetVertexBufferVector(std::vector<VertexBuffer*> &vba, std::vector<IndexBuffer*> &iba, SimpleRenderer *simpleRenderer);
	void DrawSkeleton(SimpleRenderer *simpleRenderer);
	void SaveSkeleton(FileBinaryStream& stream, std::string skeletonName);
	void LoadSkeleton(FileBinaryStream& stream, std::string skeletonName);
};