#include "Engine/RHI/MeshBuilder.hpp"
#include "Engine/Streams/FileBinaryStream.hpp"
#include "Engine/RHI/SimpleRenderer.hpp"

MeshBuilder::MeshBuilder()
{
	Initialize();
}

MeshBuilder::~MeshBuilder()
{
	Destroy();
}

void MeshBuilder::Initialize()
{
	m_vertexStamp.SetMeshBuilderDefault();
}

void MeshBuilder::Destroy()
{
	Clear();
}

void MeshBuilder::Begin(bool isUsingIndexBuffers, Texture2D *textureToUse)
{
	m_currentDrawInstruction.startIndexVerex = m_vertices.size();
	m_currentDrawInstruction.startIndexIndex = m_indices.size();
	m_currentDrawInstruction.vertexCount = 0;
	m_currentDrawInstruction.indexCount = 0;
	m_currentDrawInstruction.isUsingIndexBuffer = isUsingIndexBuffers;
	m_currentDrawInstruction.textureToUse = textureToUse;
	m_currentDrawInstruction.orthoProjection = AABB2D(g_theSimpleRenderer->m_orthoProjection);
	m_currentDrawInstruction.modelMatrix.MakeIdentity();
}

void MeshBuilder::End()
{
	m_drawInstructions.push_back(m_currentDrawInstruction);
}

size_t MeshBuilder::AddVertex(const Vector3 &position)
{
	m_vertexStamp.position = position;

	m_vertices.push_back(m_vertexStamp);
	m_indices.push_back((int)m_indices.size());
	m_currentDrawInstruction.vertexCount += 1;
	return m_vertices.size() - 1;
}

void MeshBuilder::AddVertex(const Vertex &vertex)
{
	m_vertices.push_back(vertex);
	m_currentDrawInstruction.vertexCount += 1;
}

void MeshBuilder::AddIndex(unsigned int newIndex)
{
	m_indices.push_back(newIndex);
	m_currentDrawInstruction.indexCount += 1;
}

void MeshBuilder::SetModelMatrix(const Matrix4& newModel)
{
	m_currentDrawInstruction.modelMatrix = newModel;
}

void MeshBuilder::SetTangent(const Vector3 &tangent)
{
	m_vertexStamp.tangent = tangent;
}

void MeshBuilder::SetBitangent(const Vector3 &bitangent)
{
	m_vertexStamp.bitangent = bitangent;
}

void MeshBuilder::SetNormal(const Vector3 &normal)
{
	m_vertexStamp.normal = normal;
}

void MeshBuilder::SetColor(const RGBA &color)
{
	m_vertexStamp.color = color;
}

void MeshBuilder::SetUV(const Vector2 &uv)
{
	m_vertexStamp.uv = uv;
}

void MeshBuilder::SetUV2(const Vector2 &uv)
{
	m_vertexStamp.uv2 = uv;
}

void MeshBuilder::SetBoneIndices(const UIntVector4 &indices)
{
	m_vertexStamp.boneIndices = indices;
}

void MeshBuilder::SetBoneWeights(const Vector4 &weights)
{
	m_vertexStamp.boneWeights = weights;
}

void MeshBuilder::SetIndex(const unsigned int &index)
{
	m_vertexStamp.index = index;
}

void MeshBuilder::FlipXofVertexData()
{
	std::vector<Vertex>::iterator vertIter;
	for (vertIter = m_vertices.begin(); vertIter != m_vertices.end(); ++vertIter)
	{
		vertIter->position.x = -vertIter->position.x;
		vertIter->tangent.x = -vertIter->tangent.x;
		vertIter->bitangent.x = -vertIter->bitangent.x;
		vertIter->normal.x = -vertIter->normal.x;
	}
}

void MeshBuilder::FlipYofVertexData()
{
	std::vector<Vertex>::iterator vertIter;
	for (vertIter = m_vertices.begin(); vertIter != m_vertices.end(); ++vertIter)
	{
		vertIter->position.y = -vertIter->position.y;
		vertIter->tangent.y = -vertIter->tangent.y;
		vertIter->bitangent.y = -vertIter->bitangent.y;
		vertIter->normal.y = -vertIter->normal.y;
	}
}

void MeshBuilder::FlipZofVertexData()
{
	std::vector<Vertex>::iterator vertIter;
	for (vertIter = m_vertices.begin(); vertIter != m_vertices.end(); ++vertIter)
	{
		vertIter->position.z = -vertIter->position.z;
		vertIter->tangent.z = -vertIter->tangent.z;
		vertIter->bitangent.z = -vertIter->bitangent.z;
		vertIter->normal.z = -vertIter->normal.z;
	}
}

void MeshBuilder::Clear()
{
	m_vertices.clear();
	m_indices.clear();
	m_drawInstructions.clear();
}

void MeshBuilder::SaveMesh(FileBinaryStream& stream, std::string meshName)
{
	std::string filePath = "Data\\FbxSaves\\";
	filePath += meshName;
	stream.OpenForWrite(filePath.c_str());

	int vertSize = (int)m_vertices.size();
	stream.WriteBytes(&vertSize, sizeof(int));

	for (int vertIndex = 0; vertIndex < (int)m_vertices.size(); vertIndex++)
	{
		Vertex &vertex = m_vertices[vertIndex];

		stream.WriteBytes(&vertex.position.x, sizeof(float));
		stream.WriteBytes(&vertex.position.y, sizeof(float));
		stream.WriteBytes(&vertex.position.z, sizeof(float));

		stream.WriteBytes(&vertex.normal.x, sizeof(float));
		stream.WriteBytes(&vertex.normal.y, sizeof(float));
		stream.WriteBytes(&vertex.normal.z, sizeof(float));

		stream.WriteBytes(&vertex.uv.x, sizeof(float));
		stream.WriteBytes(&vertex.uv.y, sizeof(float));

		stream.WriteBytes(&vertex.uv2.x, sizeof(float));
		stream.WriteBytes(&vertex.uv2.y, sizeof(float));

		stream.WriteBytes(&vertex.color.x, sizeof(float));
		stream.WriteBytes(&vertex.color.y, sizeof(float));
		stream.WriteBytes(&vertex.color.z, sizeof(float));
		stream.WriteBytes(&vertex.color.w, sizeof(float));

		stream.WriteBytes(&vertex.tangent.x, sizeof(float));
		stream.WriteBytes(&vertex.tangent.y, sizeof(float));
		stream.WriteBytes(&vertex.tangent.z, sizeof(float));

		stream.WriteBytes(&vertex.bitangent.x, sizeof(float));
		stream.WriteBytes(&vertex.bitangent.y, sizeof(float));
		stream.WriteBytes(&vertex.bitangent.z, sizeof(float));

		stream.WriteBytes(&vertex.boneIndices.x, sizeof(unsigned int));
		stream.WriteBytes(&vertex.boneIndices.y, sizeof(unsigned int));
		stream.WriteBytes(&vertex.boneIndices.z, sizeof(unsigned int));
		stream.WriteBytes(&vertex.boneIndices.w, sizeof(unsigned int));

		stream.WriteBytes(&vertex.boneWeights.x, sizeof(float));
		stream.WriteBytes(&vertex.boneWeights.y, sizeof(float));
		stream.WriteBytes(&vertex.boneWeights.z, sizeof(float));
		stream.WriteBytes(&vertex.boneWeights.w, sizeof(float));
	}

	stream.Close();
}

void MeshBuilder::LoadMesh(FileBinaryStream& stream, std::string meshName)
{
	std::string filePath = "Data\\FbxSaves\\";
	filePath += meshName;
	stream.OpenForRead(filePath.c_str());

	int vertSize = 0;
	stream.ReadBytes(&vertSize, sizeof(int));
	m_vertices.resize(vertSize);
	m_indices.resize(vertSize);

	for (int vertIndex = 0; vertIndex < vertSize; vertIndex++)
	{
		Vertex &vertex = m_vertices[vertIndex];

		stream.ReadBytes(&vertex.position.x, sizeof(float));
		stream.ReadBytes(&vertex.position.y, sizeof(float));
		stream.ReadBytes(&vertex.position.z, sizeof(float));

		stream.ReadBytes(&vertex.normal.x, sizeof(float));
		stream.ReadBytes(&vertex.normal.y, sizeof(float));
		stream.ReadBytes(&vertex.normal.z, sizeof(float));

		stream.ReadBytes(&vertex.uv.x, sizeof(float));
		stream.ReadBytes(&vertex.uv.y, sizeof(float));

		stream.ReadBytes(&vertex.uv2.x, sizeof(float));
		stream.ReadBytes(&vertex.uv2.y, sizeof(float));

		stream.ReadBytes(&vertex.color.x, sizeof(float));
		stream.ReadBytes(&vertex.color.y, sizeof(float));
		stream.ReadBytes(&vertex.color.z, sizeof(float));
		stream.ReadBytes(&vertex.color.w, sizeof(float));

		stream.ReadBytes(&vertex.tangent.x, sizeof(float));
		stream.ReadBytes(&vertex.tangent.y, sizeof(float));
		stream.ReadBytes(&vertex.tangent.z, sizeof(float));

		stream.ReadBytes(&vertex.bitangent.x, sizeof(float));
		stream.ReadBytes(&vertex.bitangent.y, sizeof(float));
		stream.ReadBytes(&vertex.bitangent.z, sizeof(float));

		stream.ReadBytes(&vertex.boneIndices.x, sizeof(unsigned int));
		stream.ReadBytes(&vertex.boneIndices.y, sizeof(unsigned int));
		stream.ReadBytes(&vertex.boneIndices.z, sizeof(unsigned int));
		stream.ReadBytes(&vertex.boneIndices.w, sizeof(unsigned int));

		stream.ReadBytes(&vertex.boneWeights.x, sizeof(float));
		stream.ReadBytes(&vertex.boneWeights.y, sizeof(float));
		stream.ReadBytes(&vertex.boneWeights.z, sizeof(float));
		stream.ReadBytes(&vertex.boneWeights.w, sizeof(float));

		m_indices[vertIndex] = vertIndex;
	}

	stream.Close();
}
