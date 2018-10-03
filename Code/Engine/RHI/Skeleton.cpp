#include "Engine/RHI/Motion.hpp"
#include "Engine/RHI/Skeleton.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/RHI/SimpleRenderer.hpp"
#include "Engine/Streams/BinaryStream.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Streams/FileBinaryStream.hpp"

Skeleton::Skeleton()
{

}

Skeleton::~Skeleton()
{

}

void Skeleton::Clear()
{
	m_joints.clear();
}

void Skeleton::FlipXofVertexData()
{
	std::vector<Joint>::iterator jointIter;
	for (jointIter = m_joints.begin(); jointIter != m_joints.end(); ++jointIter)
	{
		jointIter->transform.m_iBasis.x = -jointIter->transform.m_iBasis.x;
		jointIter->transform.m_jBasis.x = -jointIter->transform.m_jBasis.x;
		jointIter->transform.m_kBasis.x = -jointIter->transform.m_kBasis.x;
		jointIter->transform.m_translation.x = -jointIter->transform.m_translation.x;
	}
}

void Skeleton::AddJoint(char const *name, char const *parent_name, Matrix4 const &transform)
{
	std::string newName = name;
	std::string newParentName = "";
	if (parent_name != NULL && parent_name != '\0')	
		newParentName += parent_name;
	m_joints.push_back(Joint{newName, newParentName, transform});
}

unsigned int Skeleton::GetJointCount() const
{
	return (unsigned int)m_joints.size();
}

int Skeleton::GetJointIndex(char const *name) const
{
	for (unsigned int jointIndex = 0; jointIndex < (unsigned int)m_joints.size(); jointIndex++)
	{
		if (strcmp(m_joints[jointIndex].name.c_str(), name) == 0)
		{
			return jointIndex;
		}
	}
	return -1;
}

int Skeleton::GetJointParentIndex(unsigned int jointIndex) const
{
	std::string parentName = m_joints[jointIndex].parentName;
	int index = GetJointIndex(parentName.c_str());
	return index;
}

std::string Skeleton::GetJointName(unsigned int index) const
{
	return m_joints[index].name;
}

Matrix4 Skeleton::GetJointTransform(char const *name)
{
	std::vector<Joint>::iterator jointIter;
	for (jointIter = m_joints.begin(); jointIter != m_joints.end(); ++jointIter)
	{
		if ( strcmp ( jointIter->name.c_str(), name) == 0 )
		{
			return jointIter->transform;
		}
	}
// 	ASSERT_OR_DIE(false, Stringf("No transform exists with the name %s.", name));
	return Matrix4::IDENTITY;
}

Matrix4 Skeleton::GetJointTransform(unsigned int joint_idx) const
{
	return m_joints[joint_idx].transform;
}

void Skeleton::GetVertexBufferVector(std::vector<VertexBuffer*> &vba, std::vector<IndexBuffer*> &iba, SimpleRenderer *simpleRenderer)
{
	std::vector<Vertex> m_vertices;
	std::vector<unsigned long> m_indices;
	std::vector<Joint>::iterator jointIter;
	for (jointIter = m_joints.begin(); jointIter != m_joints.end(); ++jointIter)
	{
		m_vertices.clear();
		m_indices.clear();
		if (jointIter->parentName.c_str() != nullptr && jointIter->parentName.c_str() != '\0')
		{

			m_vertices.push_back(Vertex( jointIter->transform.m_translation.xyz(), Vector2(0.f, 0.f) ));
			m_indices.push_back(m_indices.size());

			m_vertices.push_back(Vertex( GetJointTransform(jointIter->parentName.c_str()).m_translation.xyz(), Vector2(1.f, 1.f) ));
			m_indices.push_back(m_indices.size());

			VertexBuffer *vertexBuffer = simpleRenderer->m_rhiDevice->CreateVertexBuffer(m_vertices);
			IndexBuffer *indexBuffer = simpleRenderer->m_rhiDevice->CreateIndexBuffer(m_indices);
			
			vba.push_back(vertexBuffer);
			iba.push_back(indexBuffer);
		}
	}
}

void Skeleton::DrawSkeleton(SimpleRenderer *simpleRenderer)
{
	std::vector<Joint>::iterator jointIter;
	for (jointIter = m_joints.begin(); jointIter != m_joints.end(); ++jointIter)
	{
		if (jointIter->parentName != "")
		{
			Vector3 jointPos = jointIter->transform.m_translation.xyz();
			Vector3 parentPos = GetJointTransform(jointIter->parentName.c_str()).m_translation.xyz();
			simpleRenderer->DrawLine3D(jointPos, parentPos);
		}
	}
}

void Skeleton::SaveSkeleton(FileBinaryStream& stream, std::string skeletonName)
{
	std::string filePath = "Data\\FbxSaves\\";
	filePath += skeletonName;
	stream.OpenForWrite(filePath.c_str());

	int jointSize = (int)m_joints.size();
	stream.WriteBytes(&jointSize, sizeof(int));

	for (int jointIndex = 0; jointIndex < (int)m_joints.size(); jointIndex++)
	{
		Joint &joint = m_joints[jointIndex];

		int nameSize = (int)joint.name.size();
		stream.WriteBytes(&nameSize, sizeof(int));
		if (nameSize > 0)
		{
			stream.WriteBytes(joint.name.c_str(), joint.name.size());
		}

		int parentNameSize = (int)joint.parentName.size();
		stream.WriteBytes(&parentNameSize, sizeof(int));
		if (parentNameSize > 0)
			stream.WriteBytes(joint.parentName.c_str(), joint.parentName.size());

		stream.WriteBytes(&joint.transform.m_iBasis.x, sizeof(float));
		stream.WriteBytes(&joint.transform.m_iBasis.y, sizeof(float));
		stream.WriteBytes(&joint.transform.m_iBasis.z, sizeof(float));
		stream.WriteBytes(&joint.transform.m_iBasis.w, sizeof(float));

		stream.WriteBytes(&joint.transform.m_jBasis.x, sizeof(float));
		stream.WriteBytes(&joint.transform.m_jBasis.y, sizeof(float));
		stream.WriteBytes(&joint.transform.m_jBasis.z, sizeof(float));
		stream.WriteBytes(&joint.transform.m_jBasis.w, sizeof(float));

		stream.WriteBytes(&joint.transform.m_kBasis.x, sizeof(float));
		stream.WriteBytes(&joint.transform.m_kBasis.y, sizeof(float));
		stream.WriteBytes(&joint.transform.m_kBasis.z, sizeof(float));
		stream.WriteBytes(&joint.transform.m_kBasis.w, sizeof(float));

		stream.WriteBytes(&joint.transform.m_translation.x, sizeof(float));
		stream.WriteBytes(&joint.transform.m_translation.y, sizeof(float));
		stream.WriteBytes(&joint.transform.m_translation.z, sizeof(float));
		stream.WriteBytes(&joint.transform.m_translation.w, sizeof(float));
	}

	stream.Close();
}

void Skeleton::LoadSkeleton(FileBinaryStream& stream, std::string skeletonName)
{
	Clear();

	std::string filePath = "Data\\FbxSaves\\";
	filePath += skeletonName;
	stream.OpenForRead(filePath.c_str());

	int jointSize = 0;
	stream.ReadBytes(&jointSize, sizeof(int));
	m_joints.resize(jointSize);

	for (int jointIndex = 0; jointIndex < jointSize; jointIndex++)
	{
		Joint &joint = m_joints[jointIndex];

		int nameSize = 0;
		stream.ReadBytes(&nameSize, sizeof(int));
		if (nameSize > 0)
		{
			joint.name.resize(nameSize);
			stream.ReadBytes(&joint.name[0], nameSize);
			joint.name = joint.name.c_str();
		}

		int parentNameSize = 0;
		stream.ReadBytes(&parentNameSize, sizeof(int));
		if (parentNameSize > 0)
		{
			joint.parentName.resize(parentNameSize);
			stream.ReadBytes(&joint.parentName[0], parentNameSize);
			joint.parentName = joint.parentName.c_str();
		}

		stream.ReadBytes(&joint.transform.m_iBasis.x, sizeof(float));
		stream.ReadBytes(&joint.transform.m_iBasis.y, sizeof(float));
		stream.ReadBytes(&joint.transform.m_iBasis.z, sizeof(float));
		stream.ReadBytes(&joint.transform.m_iBasis.w, sizeof(float));

		stream.ReadBytes(&joint.transform.m_jBasis.x, sizeof(float));
		stream.ReadBytes(&joint.transform.m_jBasis.y, sizeof(float));
		stream.ReadBytes(&joint.transform.m_jBasis.z, sizeof(float));
		stream.ReadBytes(&joint.transform.m_jBasis.w, sizeof(float));

		stream.ReadBytes(&joint.transform.m_kBasis.x, sizeof(float));
		stream.ReadBytes(&joint.transform.m_kBasis.y, sizeof(float));
		stream.ReadBytes(&joint.transform.m_kBasis.z, sizeof(float));
		stream.ReadBytes(&joint.transform.m_kBasis.w, sizeof(float));

		stream.ReadBytes(&joint.transform.m_translation.x, sizeof(float));
		stream.ReadBytes(&joint.transform.m_translation.y, sizeof(float));
		stream.ReadBytes(&joint.transform.m_translation.z, sizeof(float));
		stream.ReadBytes(&joint.transform.m_translation.w, sizeof(float));
	}

	stream.Close();
}
