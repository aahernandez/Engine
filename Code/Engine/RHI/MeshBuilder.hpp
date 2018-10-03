#pragma once
#include "Engine/Math/Vertex.hpp"
#include "Engine/Math/AABB2D.hpp"
#include "Engine/Math/Matrix4.hpp"
#include <vector>

class FileBinaryStream;
class Texture2D;

struct DrawInstruction_t
{
	unsigned int startIndexVerex;
	unsigned int startIndexIndex;
	unsigned int vertexCount;
	unsigned int indexCount;
	Matrix4 modelMatrix;
	AABB2D orthoProjection;
	unsigned int type;
	Texture2D *textureToUse;
	bool isUsingIndexBuffer;
};

class MeshBuilder
{
public:
	std::vector<Vertex> m_vertices;
	std::vector<unsigned long> m_indices;
	std::vector<DrawInstruction_t> m_drawInstructions;
	DrawInstruction_t m_currentDrawInstruction;

	MeshBuilder();
	~MeshBuilder();

	void Initialize();
	void Destroy();

	void Begin(bool isUsingIndexBuffers, Texture2D *textureToUse);
	void End();
	size_t AddVertex(const Vector3 &position);
	void AddVertex(const Vertex &vertex);
	void AddIndex(unsigned int newIndex);
	void SetModelMatrix(const Matrix4& newModel);
	void SetTangent(const Vector3 &tangent);
	void SetBitangent(const Vector3 &bitangent);
	void SetNormal(const Vector3 &normal);
	void SetColor(const RGBA &color);
	void SetUV(const Vector2 &uv);
	void SetUV2(const Vector2 &uv);
	void SetBoneIndices(const UIntVector4 &indices);
	void SetBoneWeights(const Vector4 &weights);
	void SetIndex(const unsigned int &index);

	void FlipXofVertexData();
	void FlipYofVertexData();
	void FlipZofVertexData();
	void Clear();

	void SaveMesh(FileBinaryStream& stream, std::string meshName);
	void LoadMesh(FileBinaryStream& stream, std::string meshName);

private:
	Vertex m_vertexStamp;
};