#include "Engine/RHI/SimpleRenderer.hpp"
#include "Engine/RHI/RHIInstance.hpp"
#include "Engine/RHI/ShaderProgram.hpp"
#include "Engine/RHI/VertexBuffer.hpp"
#include "Engine/RHI/IndexBuffer.hpp"
#include "Engine/RHI/Sampler.hpp"
#include "Engine/RHI/RasterState.hpp"
#include "Engine/RHI/ConstantBuffer.hpp"
#include "Engine/RHI/StructuredBuffer.hpp"
#include "Engine/RHI/BlendState.hpp"
#include "Engine/RHI/Font.hpp"
#include "Engine/RHI/MeshBuilder.hpp"
#include "Engine/Core/CommonUtilities.hpp"
#include "Engine/Core/FileUtils.hpp"
#include "Engine/Profiling/Profiler.hpp"
#include "Engine/Memory/JobSystem.hpp"
#include "Engine/Memory/Thread.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/UI/UIButton.hpp"
#include <sstream>
#include <regex>

SimpleRenderer::SimpleRenderer()
	: m_rhiDevice(nullptr)
	, m_rhiContext(nullptr)
	, m_rhiOutput(nullptr)
	, m_currentTarget(nullptr)
	, m_matrixCB(nullptr)
	, m_timeCB(nullptr)
	, m_defaultRasterState(nullptr)
	, m_currentBlendState(nullptr)
	, m_depthStencilState(nullptr)
	, m_defaultDepthStencil(nullptr)
	, m_font(nullptr)
	, m_meshBuilder(nullptr)
	, m_windowToScreenRatio(1.f, 1.f)
	, m_currentSetTexture(nullptr)
{
}

SimpleRenderer::~SimpleRenderer()
{
	Destroy();
}

void SimpleRenderer::Setup(unsigned int width, unsigned int height, e_RHIOutputMode outputMode, char const *fontName)
{
	m_windowToScreenRatio = Vector2::ONE;
	RHIInstance *rhiInstance = RHIInstance::GetInstance();
	rhiInstance->CreateOutput(&m_rhiDevice, &m_rhiContext, &m_rhiOutput, width, height, outputMode);
	m_defaultRasterState = new RasterState(m_rhiDevice);
	m_rhiContext->SetRasterState(m_defaultRasterState);

	MemZero(&m_timeData);

	m_matrixCB = new ConstantBuffer(m_rhiDevice, &m_matrixData, sizeof(m_matrixData));
	m_timeCB = new ConstantBuffer(m_rhiDevice, &m_timeData, sizeof(m_timeData));
	m_lightCB = new ConstantBuffer(m_rhiDevice, &m_lightData, sizeof(m_lightData));

	SetConstantBuffer(MATRIX_BUFFER_INDEX, m_matrixCB);
	SetConstantBuffer(TIME_BUFFER_INDEX, m_timeCB);
	SetConstantBuffer(LIGHT_BUFFER_INDEX, m_lightCB);

	m_colorShader = m_rhiDevice->CreateShaderFromHLSLFile("Data/HLSL/color.hlsl");
	m_lightShader = m_rhiDevice->CreateShaderFromHLSLFile("Data/HLSL/light.hlsl");
	m_unlitShader = m_rhiDevice->CreateShaderFromHLSLFile("Data/HLSL/unlit.hlsl");
	m_skinWeightsShader = m_rhiDevice->CreateShaderFromHLSLFile("Data/HLSL/skinned_boneweight.hlsl");
	m_morphTargetShader = m_rhiDevice->CreateShaderFromHLSLFile("Data/HLSL/blendShapes.hlsl");

	Image image;
	image.CreateClear(1, 1, RGBA::WHITE);
	m_whiteTexture = new Texture2D(m_rhiDevice, image);

	image.CreateClear(1, 1, RGBA(0.5f, 0.5f, 1.f, 1.f));
	m_flatNormalTexture = new Texture2D(m_rhiDevice, image);

	m_linearSampler = new Sampler(g_theSimpleRenderer->m_rhiDevice, FILTER_LINEAR, FILTER_LINEAR);

	m_font = new Font(fontName);
	m_font->CreateTexture(m_rhiDevice);

	m_defaultDepthStencil = new Texture2D(m_rhiDevice, width, height, IMAGEFORMAT_D24S8);
	m_currentDepthStencil = nullptr;

	m_depthStencilDesc.depthTestEnabled = false;
	m_depthStencilDesc.depthWritingEnabled = true;
	m_depthStencilState = new DepthStencilState(m_rhiDevice, m_depthStencilDesc);
	m_rhiContext->SetDepthStencilState(m_depthStencilState);

	SetRenderTarget(nullptr, nullptr);
	SetSampler(nullptr);
	SetTexture(nullptr);

	m_meshBuilder = new MeshBuilder();
}

void SimpleRenderer::Destroy()
{
	delete m_meshBuilder;
	m_meshBuilder = nullptr;

	if (m_depthStencilState != nullptr)
		delete m_depthStencilState;
	m_depthStencilState = nullptr;

	delete m_lightCB;
	m_lightCB = nullptr;

	delete m_matrixCB;
	m_matrixCB = nullptr;

	delete m_timeCB;
	m_timeCB = nullptr;

	if (m_font != nullptr)
		delete m_font;
	m_font = nullptr;

	delete m_linearSampler;
	m_linearSampler = nullptr;

	delete m_whiteTexture;
	m_whiteTexture = nullptr;
	
	delete m_flatNormalTexture;
	m_flatNormalTexture = nullptr;

	if (m_defaultDepthStencil != nullptr)
		delete m_defaultDepthStencil;
	m_defaultDepthStencil = nullptr;

	delete m_currentBlendState;
	m_currentBlendState = nullptr;

	delete m_defaultRasterState;
	m_defaultRasterState = nullptr;

	delete m_currentTarget;
	m_currentTarget = nullptr;

	delete m_morphTargetShader;
	m_morphTargetShader = nullptr;

	delete m_skinWeightsShader;
	m_skinWeightsShader = nullptr;

	delete m_unlitShader;
	m_unlitShader = nullptr;

	delete m_lightShader;
	m_lightShader = nullptr;

	delete m_colorShader;
	m_colorShader = nullptr;

	delete m_rhiOutput;
	m_rhiOutput = nullptr;

	delete m_rhiContext;
	m_rhiContext = nullptr;

	delete m_rhiDevice;
	m_rhiDevice = nullptr;

	RHIInstance::Destroy();
}

void SimpleRenderer::LoadObjectFile(char const *modelFileName, std::vector<Vertex> &testObjectVertices, std::vector<DWORD> &testObjectIndices, std::string &fileName, float scale)
{
	std::string filePath = "Data/Models/";
	filePath += modelFileName;
	filePath += "/";
	filePath += modelFileName;
	filePath += ".obj";

	size_t objFileSize = 0U;
	std::stringstream stream;
	void *objFileData = FileReadToBuffer(filePath.c_str(), &objFileSize);
	stream.write((const char*)objFileData, objFileSize);

	std::string value;
	std::string objFileLine;
	std::smatch match;
	std::vector<Vector3> tempVertices;
	std::vector<Vector2> texCoords;
	std::vector<Vector3> normals;
	std::vector<unsigned int> vertIndices;
	std::vector<unsigned int> texIndices;
	std::vector<unsigned int> normalIndices;
	unsigned int numIndices = 0;

	while(getline(stream, objFileLine))
	{
		char const *characterCode = objFileLine.c_str();

		if (*characterCode == '#')
		{
			continue;
		}

		if (*characterCode == 'm')
		{
			++characterCode;
			if (*characterCode == 't')
			{
				++characterCode;
				if (*characterCode == 'l')
				{
					++characterCode;
					if (*characterCode == 'l')
					{
						++characterCode;
						if (*characterCode == 'i')
						{
							++characterCode;
							if (*characterCode == 'b')
							{
								++characterCode;
								unsigned int lastSpacePosition = 6;
								while (*characterCode == ' ')
								{
									++lastSpacePosition;
									++characterCode;
								}

								unsigned int lastCharPosition = 0;
								while (*characterCode != '.')
								{
									++lastCharPosition;
									++characterCode;
								}

								fileName = objFileLine.substr(lastSpacePosition, lastCharPosition);
								fileName;
							}
						}
					}
				}
			}
		}

		if (*characterCode == 'v')
		{
			float vertex1, vertex2, vertex3;
			++characterCode;

			if (*characterCode == 'n') //normals
			{
				++characterCode;
				unsigned int lastSpacePosition = 2;
				while (*characterCode == ' ')
				{
					++lastSpacePosition;
					++characterCode;
				}

				unsigned int lastCharPosition = 0;
				while (*characterCode != ' ')
				{
					++lastCharPosition;
					++characterCode;
				}
				value = objFileLine.substr(lastSpacePosition, lastCharPosition);
				vertex1 = std::stof(value);

				lastSpacePosition += lastCharPosition;
				while (*characterCode == ' ')
				{
					++lastSpacePosition;
					++characterCode;
				}

				lastCharPosition = 0;
				while (*characterCode != ' ')
				{
					++lastCharPosition;
					++characterCode;
				}
				value = objFileLine.substr(lastSpacePosition, lastCharPosition);
				vertex2 = std::stof(value);

				lastSpacePosition += lastCharPosition;
				while (*characterCode == ' ')
				{
					++lastSpacePosition;
					++characterCode;
				}

				lastCharPosition = 0;
				while (*characterCode != ' ')
				{
					++lastCharPosition;
					++characterCode;
				}
				value = objFileLine.substr(lastSpacePosition, lastCharPosition);
				vertex3 = std::stof(value);

				normals.push_back(Vector3(vertex1, vertex2, vertex3));
				continue;
			}
			else if (*characterCode == 't') //texture coordinates
			{
				++characterCode;
				unsigned int lastSpacePosition = 1;
				while (*characterCode == ' ')
				{
					++lastSpacePosition;
					++characterCode;
				}

				unsigned int lastCharPosition = 1;
				while (*characterCode != ' ')
				{
					++lastCharPosition;
					++characterCode;
				}
				value = objFileLine.substr(lastSpacePosition, lastCharPosition);
				vertex1 = std::stof(value);

				lastSpacePosition += lastCharPosition;
				while (*characterCode == ' ')
				{
					++lastSpacePosition;
					++characterCode;
				}

				lastCharPosition = 0;
				while (*characterCode != ' ')
				{
					++lastCharPosition;
					++characterCode;
				}
				value = objFileLine.substr(lastSpacePosition, lastCharPosition);
				vertex2 = std::stof(value);

				texCoords.push_back(Vector2(vertex1, 1 - vertex2));
			}
			else //vertexes
			{
				unsigned int lastSpacePosition = 1;
				while (*characterCode == ' ')
				{
					++lastSpacePosition;
					++characterCode;
				}

				unsigned int lastCharPosition = 0;
				while (*characterCode != ' ')
				{
					++lastCharPosition;
					++characterCode;
				}
				value = objFileLine.substr(lastSpacePosition, lastCharPosition);
				vertex1 = std::stof(value);

				lastSpacePosition += lastCharPosition;
				while (*characterCode == ' ')
				{
					++lastSpacePosition;
					++characterCode;
				}

				lastCharPosition = 0;
				while (*characterCode != ' ')
				{
					++lastCharPosition;
					++characterCode;
				}
				value = objFileLine.substr(lastSpacePosition, lastCharPosition);
				vertex2 = std::stof(value);

				lastSpacePosition += lastCharPosition;
				while (*characterCode == ' ')
				{
					++lastSpacePosition;
					++characterCode;
				}

				lastCharPosition = 0;
				while (*characterCode != ' ' && *characterCode != '\r')
				{
					++lastCharPosition;
					++characterCode;
				}
				value = objFileLine.substr(lastSpacePosition, lastCharPosition);
				vertex3 = std::stof(value);

				tempVertices.push_back(Vector3(vertex1 * scale, vertex3 * scale, vertex2 * scale));
			}
		}


		if (*characterCode == 'f') //faces
		{
			unsigned int lastSpacePosition = 0;
			unsigned int lastCharPosition = 0;
			
			while (*characterCode != NULL)
			{
				++numIndices;
				++characterCode;
				++lastSpacePosition;

				if (*characterCode == '\r')
				{
					break;
				}

				while (*characterCode == ' ')
				{
					++lastSpacePosition;
					++characterCode;
				}

				lastCharPosition = 0;
				while (*characterCode != '/')
				{
					++lastCharPosition;
					++characterCode;
				}
				value = objFileLine.substr(lastSpacePosition, lastCharPosition);
				vertIndices.push_back(atoi(value.c_str()));
				++characterCode;

				lastSpacePosition += lastCharPosition + 1;
				lastCharPosition = 0;
				while (*characterCode != '/')
				{
					++lastCharPosition;
					++characterCode;
				}
				value = objFileLine.substr(lastSpacePosition, lastCharPosition);
				if (!value.empty())
				{
					texIndices.push_back(atoi(value.c_str()));
				}

				++characterCode; //forward slash seperator

				lastSpacePosition += lastCharPosition + 1;
				lastCharPosition = 0;
				while (*characterCode != ' ' && *characterCode != '\0')
				{
					++lastCharPosition;
					++characterCode;
				}
				value = objFileLine.substr(lastSpacePosition, lastCharPosition);
				normalIndices.push_back(atoi(value.c_str()));
				lastSpacePosition += lastCharPosition;
			}
		}
	}

	for (unsigned int indexCounter = 0; indexCounter < vertIndices.size(); indexCounter++)
	{
		Vector3 tempVec = tempVertices[vertIndices[indexCounter]-1];
// 		Vector2 tempVec2(0.5f, 0.6f);
		Vector3 tempVec3 = normals[normalIndices[indexCounter]-1];
		Vertex vertex;
		if (!texCoords.empty())
		{
			Vector2 tempVec2 = texCoords[texIndices[indexCounter]-1];
			vertex = Vertex(tempVec, tempVec3, tempVec2, RGBA::WHITE);
		}
		else
		{
			vertex = Vertex(tempVec, tempVec3);
		}
		testObjectVertices.push_back(vertex);
		testObjectIndices.push_back(indexCounter);
	}
}

void SimpleRenderer::SetRenderTarget(Texture2D *colorTarget, Texture2D *depthTarget)
{
	if (colorTarget != nullptr) 
	{
		if (colorTarget->IsRenderTarget())
		{
			m_currentTarget = colorTarget;
		}
	}
	else {
		m_currentTarget = m_rhiOutput->m_renderTarget;
	}

	if (depthTarget == nullptr)
	{
		depthTarget = m_defaultDepthStencil;
	}

	m_currentDepthStencil = depthTarget;
	m_rhiContext->SetColorTarget(m_currentTarget, depthTarget);
}

void SimpleRenderer::SetViewport(unsigned int x, unsigned int y, unsigned int width, unsigned int height)
{
	m_rhiContext->SetViewport(x, y, width, height);
}

void SimpleRenderer::ClearColor(RGBA const &color)
{
	m_rhiContext->ClearColorTarget(m_currentTarget, color);
}

void SimpleRenderer::ClearDepth(float depth /*= 1.f*/, unsigned char stencil /*= 0*/)
{
	m_rhiContext->ClearDepthTarget(m_currentDepthStencil, depth, stencil);
}

void SimpleRenderer::ClearTargetColor(Texture2D *target, RGBA const &color)
{
	m_rhiContext->ClearColorTarget(target, color);
}

void SimpleRenderer::SetCameraMatrix(Matrix4 const &camMatrix)
{
	Vector3 position = camMatrix.GetTranslation();
	m_matrixData.eyePosition = Vector4(position, 1.f);

	Matrix4 viewMatrix = camMatrix.GetInverseOrthonormal();

	SetViewMatrix(viewMatrix);
}

void SimpleRenderer::SetModelMatrix(Matrix4 &model)
{
	model.Transpose();
	m_matrixData.model = model;
	m_matrixCB->Update(m_rhiContext, &m_matrixData);
}

void SimpleRenderer::SetProjectionMatrix(Matrix4 &proj)
{
	proj.Transpose();
	m_matrixData.projection = proj;
	m_matrixCB->Update(m_rhiContext, &m_matrixData);
}

void SimpleRenderer::SetViewMatrix(Matrix4 &viewMat)
{
	viewMat.Transpose();
	m_matrixData.view = viewMat;
	m_matrixCB->Update(m_rhiContext, &m_matrixData);
}

void SimpleRenderer::SetViewMatrixTranslation(Vector3 translation)
{
	m_matrixData.view.m_iBasis.w = translation.x;
	m_matrixData.view.m_jBasis.w = translation.y;
	m_matrixData.view.m_kBasis.w = translation.z;
	m_matrixCB->Update(m_rhiContext, &m_matrixData);
}

void SimpleRenderer::SetViewMatrixRotation(float rollAboutX, float pitchAboutY, float yawAboutZ)
{
	Matrix4 xMatrix;
	xMatrix.m_iBasis.x = CosDegrees(rollAboutX);
	xMatrix.m_iBasis.y = SinDegrees(rollAboutX);
	xMatrix.m_jBasis.x = -SinDegrees(rollAboutX);
	xMatrix.m_jBasis.y = CosDegrees(rollAboutX);

	Matrix4 yMatrix;
	yMatrix.m_jBasis.y = CosDegrees(pitchAboutY);
	yMatrix.m_jBasis.z = SinDegrees(pitchAboutY);
	yMatrix.m_kBasis.y = -SinDegrees(pitchAboutY);
	yMatrix.m_kBasis.z = CosDegrees(pitchAboutY);

	Matrix4 zMatrix;
	zMatrix.m_iBasis.x = CosDegrees(yawAboutZ);
	zMatrix.m_iBasis.z = -SinDegrees(yawAboutZ);
	zMatrix.m_kBasis.x = SinDegrees(yawAboutZ);
	zMatrix.m_kBasis.z = CosDegrees(yawAboutZ);

	Matrix4 rotationMatrix;

	rotationMatrix.m_translation.x = m_matrixData.view.m_iBasis.w;
	rotationMatrix.m_translation.y = m_matrixData.view.m_jBasis.w;
	rotationMatrix.m_translation.z = m_matrixData.view.m_kBasis.w;

	rotationMatrix = rotationMatrix * zMatrix;
	rotationMatrix = rotationMatrix * yMatrix;
	rotationMatrix = rotationMatrix * xMatrix;

	rotationMatrix = rotationMatrix.GetInverse(rotationMatrix);	
	rotationMatrix.Transpose();

	m_matrixData.view = rotationMatrix;
	m_matrixCB->Update(m_rhiContext, &m_matrixData);
}

void SimpleRenderer::SetViewMatrixTranslationAndRotation(Vector3 translation, float rollAboutX, float pitchAboutY, float yawAboutZ)
{
	m_matrixData.eyePosition = Vector4(translation, 1.f);

	Matrix4 xMatrix;
	xMatrix.m_iBasis.x = CosDegrees(rollAboutX);
	xMatrix.m_iBasis.y = SinDegrees(rollAboutX);
	xMatrix.m_jBasis.x = -SinDegrees(rollAboutX);
	xMatrix.m_jBasis.y = CosDegrees(rollAboutX);

	Matrix4 yMatrix;
	yMatrix.m_jBasis.y = CosDegrees(pitchAboutY);
	yMatrix.m_jBasis.z = SinDegrees(pitchAboutY);
	yMatrix.m_kBasis.y = -SinDegrees(pitchAboutY);
	yMatrix.m_kBasis.z = CosDegrees(pitchAboutY);

	Matrix4 zMatrix;
	zMatrix.m_iBasis.x = CosDegrees(yawAboutZ);
	zMatrix.m_iBasis.z = -SinDegrees(yawAboutZ);
	zMatrix.m_kBasis.x = SinDegrees(yawAboutZ);
	zMatrix.m_kBasis.z = CosDegrees(yawAboutZ);

	Matrix4 rotationMatrix;

	rotationMatrix.m_translation.x = translation.x;
	rotationMatrix.m_translation.y = translation.y;
	rotationMatrix.m_translation.z = translation.z;

	rotationMatrix = rotationMatrix * zMatrix;
	rotationMatrix = rotationMatrix * yMatrix;
	rotationMatrix = rotationMatrix * xMatrix;

	rotationMatrix = rotationMatrix.GetInverse(rotationMatrix);
	rotationMatrix.Transpose();

	m_matrixData.view = rotationMatrix;
	m_matrixCB->Update(m_rhiContext, &m_matrixData);
}

void SimpleRenderer::SetOrthoProjection(Vector2 const &bottomLeft, Vector2 const &topRight)
{
	Matrix4 orthoProjection = Matrix4::MakeOrthoProjection(bottomLeft.x,
		topRight.x,
		bottomLeft.y,
		topRight.y,
		0.f, 1.f);
	m_orthoProjection = AABB2D(bottomLeft, topRight);
	SetProjectionMatrix(orthoProjection);
	m_windowToScreenRatio.x = topRight.x / m_rhiOutput->GetWidth();
	m_windowToScreenRatio.y = topRight.y / m_rhiOutput->GetHeight();
}

void SimpleRenderer::SetPerspectiveProjection(float const fovRadians, float aspectRatio, float const nz, float const fz)
{
	Matrix4 proj = MatrixMakePerspectiveProjection(fovRadians, aspectRatio, nz, fz);
	SetProjectionMatrix(proj);
}

void SimpleRenderer::EnableBlend(e_BlendFactor src, e_BlendFactor dest)
{
	if (m_currentBlendState != nullptr)
	{
		delete m_currentBlendState;
		m_currentBlendState = nullptr;
	}
	BlendState *bs = new BlendState(m_rhiDevice, true, src, dest);
	m_rhiContext->SetBlendState(bs);
	
	m_blendState.enabled = true;
	m_blendState.srcFactor = src;
	m_blendState.destFactor = dest;

	m_currentBlendState = bs;
	bs = nullptr;
}

void SimpleRenderer::DisableBlend()
{
	if (m_blendState.enabled) 
	{
		return;
	}

	if (m_currentBlendState != nullptr)
		delete m_currentBlendState;
	m_currentBlendState = nullptr;

	BlendState *bs = new BlendState(m_rhiDevice, false);
	m_rhiContext->SetBlendState(bs);
	m_blendState.enabled = false;
	m_currentBlendState = bs;
	bs = nullptr;
}

void SimpleRenderer::EnableDepthTest(bool isEnabled, bool isWrite)
{
	if (m_depthStencilDesc.depthTestEnabled != isEnabled 
		|| m_depthStencilDesc.depthWritingEnabled != isWrite )
	{
		m_depthStencilDesc.depthWritingEnabled = isWrite;
		m_depthStencilDesc.depthTestEnabled = isEnabled;
		delete m_depthStencilState;
		m_depthStencilState = nullptr;
		m_depthStencilState = new DepthStencilState(m_rhiDevice, m_depthStencilDesc);
		m_rhiContext->SetDepthStencilState(m_depthStencilState);
	}
}

void SimpleRenderer::EnableDepthWrite(bool isEnabled)
{
	if (m_depthStencilDesc.depthWritingEnabled != isEnabled) {
		m_depthStencilDesc.depthWritingEnabled = isEnabled;
		delete m_depthStencilState;
		m_depthStencilState = nullptr;
		m_depthStencilState = new DepthStencilState(m_rhiDevice, m_depthStencilDesc);
		m_rhiContext->SetDepthStencilState(m_depthStencilState);
	}
}

void SimpleRenderer::Present()
{
	m_rhiOutput->Present();
}

void SimpleRenderer::SetWindowTitle(const char *title)
{
	m_rhiOutput->SetWindowTitle(title);
}

void SimpleRenderer::DispatchComputeJob(ComputeJob *job)
{
	m_rhiContext->DispatchComputeJob(job);
}

void SimpleRenderer::SetShader(ShaderProgram *shader)
{
	m_rhiContext->SetShader(shader);
}

void SimpleRenderer::SetComputeShader(ComputeShaderProgram *shader)
{
	m_rhiContext->SetComputeShader(shader);
}

void SimpleRenderer::SetTexture(unsigned int textureIndex, Texture2D *texture)
{
	if (texture == nullptr)
		texture = m_whiteTexture;

	m_currentSetTexture = texture;

	m_rhiContext->SetTexture(textureIndex, texture);
}

void SimpleRenderer::BindUAV(unsigned int textureIndex, Texture2D *texture)
{
	ASSERT_OR_DIE(texture != nullptr, "UAV texture is nullptr.");

	m_rhiContext->BindUAV(textureIndex, texture);
}

void SimpleRenderer::UnbindUAV()
{
	m_rhiContext->UnbindUAV();
}

void SimpleRenderer::SetSampler(unsigned int samplerIndex, Sampler *sampler)
{
	if (sampler == nullptr) {
		sampler = m_linearSampler;
	}

	m_rhiContext->SetSampler(samplerIndex, sampler);
}

void SimpleRenderer::SetAmbientLight(float intensity, RGBA const &color /*= RGBA::WHITE*/)
{
	float colorRGB[4];
	color.GetAsFloats(colorRGB[0], colorRGB[1], colorRGB[2], colorRGB[3]);
	m_lightData.ambient = Vector4(colorRGB[0], colorRGB[1], colorRGB[2], intensity);
	m_lightCB->Update(m_rhiContext, &m_lightData);
}

void SimpleRenderer::SetSpecularConstants(float const spec_power, float const spec_factor /*= 1.0f*/)
{
	m_lightData.specFactor = spec_factor;
	m_lightData.specPower = spec_power;
	m_lightCB->Update(m_rhiContext, &m_lightData);
}

void SimpleRenderer::EnablePointLight(int index, Vector3 const &pos, RGBA const &color, float intensity /*= 1.0f*/, Vector3 const &attenuation /*= Vector3(0, 0, 1)*/, Vector3 const &spec_attenuation /*= Vector3(0, 0, 1)*/)
{
	float r, g, b, a;
	color.GetAsFloats(r, g, b, a);
	m_lightData.pointLights[index].lightPosition = Vector4( pos, 1.f);
	m_lightData.pointLights[index].lightColor = Vector4(r, g, b, intensity);
	m_lightData.pointLights[index].attenuation = Vector4(attenuation, 1.f);
	m_lightData.pointLights[index].specAttenuation = Vector4(spec_attenuation, 1.f);
	m_lightCB->Update(m_rhiContext, &m_lightData);
}

void SimpleRenderer::SetConstantBuffer(unsigned int index, ConstantBuffer *constantBuffer)
{
	m_rhiContext->SetConstantBuffer(index, constantBuffer);
}

void SimpleRenderer::SetComputeConstantBuffer(unsigned int index, ConstantBuffer *cb)
{
	m_rhiContext->SetComputeConstantBuffer(index, cb);
}

void SimpleRenderer::SetStructuredBuffer(unsigned int index, StructuredBuffer *sb)
{
	m_rhiContext->SetStructuredBuffer(index, sb);
}

void SimpleRenderer::Draw(e_PrimitiveType topology, VertexBuffer *vbo, IndexBuffer *ibo)
{
	m_rhiContext->Draw(topology, vbo, ibo);
}

void SimpleRenderer::DrawVertexBufferArray(e_PrimitiveType topology, std::vector<VertexBuffer *> m_vertexBufferArray, std::vector<IndexBuffer *> m_indexBufferArray)
{
	for (int bufferIndex = 0; bufferIndex < (int)m_vertexBufferArray.size(); bufferIndex++)
	{
		Draw(topology, m_vertexBufferArray[bufferIndex], m_indexBufferArray[bufferIndex]);
	}

}

void SimpleRenderer::DrawMeshBuilder()
{
	std::vector<DrawInstruction_t>::iterator instructionIter = m_meshBuilder->m_drawInstructions.begin();
	std::vector<VertexBuffer*> vba;
	std::vector<IndexBuffer*> iba;
	Texture2D* currentTexture = nullptr;
	AABB2D currentOrtho = AABB2D();
	Matrix4 currentModelMatrix = Matrix4::ZERO;

	for (instructionIter; instructionIter != m_meshBuilder->m_drawInstructions.end(); ++instructionIter)
	{
		DrawInstruction_t &instruction = (*instructionIter);
		
		if (instruction.vertexCount == 0 || instruction.indexCount == 0)
			continue;


		if (currentTexture != instruction.textureToUse || currentOrtho != instruction.orthoProjection || currentModelMatrix != instruction.modelMatrix)
		{
			if (!vba.empty())
			{
				DrawVertexBufferArray(PRIMITIVE_TRIANGLES, vba, iba);

				std::vector<VertexBuffer*>::iterator vbaIter;
				std::vector<IndexBuffer*>::iterator ibaIter;;

				for (vbaIter = vba.begin(); vbaIter != vba.end(); ++vbaIter)
				{
					delete *vbaIter;
					*vbaIter = nullptr;
				}
				vba.clear();

				for (ibaIter = iba.begin(); ibaIter != iba.end(); ++ibaIter)
				{
					delete *ibaIter;
					*ibaIter = nullptr;
				}
				iba.clear();
			}

			currentTexture = instruction.textureToUse;
			currentOrtho = instruction.orthoProjection;
			SetTexture(currentTexture);
			SetModelMatrix(instruction.modelMatrix);
			SetOrthoProjection(instruction.orthoProjection.mins, instruction.orthoProjection.maxs);
		}
		VertexBuffer *glyphVB = m_rhiDevice->CreateVertexBuffer(m_meshBuilder->m_vertices, instruction.startIndexVerex, instruction.vertexCount);
		IndexBuffer *glyphIB = m_rhiDevice->CreateIndexBuffer(m_meshBuilder->m_indices, instruction.startIndexIndex, instruction.indexCount);
		vba.push_back(glyphVB);
		iba.push_back(glyphIB);
	}
	DrawVertexBufferArray(PRIMITIVE_TRIANGLES, vba, iba);

	std::vector<VertexBuffer*>::iterator vbaIter;
	std::vector<IndexBuffer*>::iterator ibaIter;;

	for (vbaIter = vba.begin(); vbaIter != vba.end(); ++vbaIter)
	{
		delete *vbaIter;
		*vbaIter = nullptr;
	}
	vba.clear();

	for (ibaIter = iba.begin(); ibaIter != iba.end(); ++ibaIter)
	{
		delete *ibaIter;
		*ibaIter = nullptr;
	}
	iba.clear();
}

void SimpleRenderer::DrawText2D(char const *string, Vector2 position, RGBA color /*= RGBA::WHITE*/, float scale /*= 1.f*/, Font *font /*= m_font*/)
{
	Vector2 cursor = position;

	char const *characterCode = string;
	int prevCharacterCode = -1;
	unsigned int charCount = 0;
	RGBA newColor = color;

	if (font == nullptr)
	{
		font = m_font;
	}

	m_meshBuilder->Begin(true, font->m_texture);

	while (*characterCode != NULL)
	{
		if (*characterCode == '{')
		{
			++characterCode;
			if (*characterCode == '-')
			{
				newColor = color;
				++characterCode;
				++characterCode;
			}
			else if (*characterCode >= 48 && *characterCode <= 57) //if character code is a number
			{
				std::string rStr, gStr, bStr;
				float r, g, b;
				while (*characterCode != ',')
				{
					rStr += *characterCode;
					++characterCode;
				}
				r = (float)atof(rStr.c_str());
				++characterCode;

				while (*characterCode != ',')
				{
					gStr += *characterCode;
					++characterCode;
				}
				g = (float)atof(gStr.c_str());
				++characterCode;

				while (*characterCode != '}')
				{
					bStr += *characterCode;
					++characterCode;
				}
				b = (float)atof(bStr.c_str());

				newColor.SetAsFloats(r, g, b, 1.f);
				++characterCode;
			}
			else
			{
				--characterCode;
			}
		}
		else if (*characterCode == '\t')
		{
			++characterCode;

			int numTabs = 1;
			std::string numTabsStr = "";
			while (*characterCode >= 48 && *characterCode <= 57) //while character code is a number
			{
				numTabsStr += *characterCode;
				++characterCode;
			}
			if (!numTabsStr.empty())
			{
				numTabs = atoi(numTabsStr.c_str());
			}

			int cursorTabs = (int)(cursor.x / TAB_SPACES);
			cursor.x = TAB_SPACES * (float)(cursorTabs + numTabs);
		}
		else
		{
			t_FontData const *glyph = font->GetGlyph(*characterCode);

			if (glyph == nullptr)
				glyph = font->GetInvalidGlyph();

			float scaleW = (float)font->m_scale.x;
			float scaleH = (float)font->m_scale.y;

			Vector2 topLeft = cursor + (Vector2((float)glyph->xOffset * scale, (float)-glyph->yOffset * scale));
			Vector2 bottomLeft = topLeft + Vector2(0.f, (float)-glyph->height * scale);
			Vector2 topRight = topLeft + Vector2((float)glyph->width * scale, 0.f);

			Vector2 uvTopLeft = Vector2((float)glyph->x / scaleW, (float)glyph->y / scaleH);
			Vector2 uvTopRight = uvTopLeft + Vector2(0.f, (float)glyph->height / scaleH);
			Vector2 uvBottomLeft = uvTopLeft + Vector2((float)glyph->width / scaleW, 0.f);
			float tempX = uvBottomLeft.x;
			uvBottomLeft.x = uvTopRight.x;
			uvTopRight.x = tempX;

			AABB2D boundingBox(bottomLeft, topRight);
			Vector2 radii = boundingBox.CalcSize();
			radii *= scale;
			AABB2D boundingBoxScaled(boundingBox);
			boundingBoxScaled.maxs = boundingBoxScaled.mins + radii;

			m_meshBuilder->AddVertex(Vertex(Vector3(bottomLeft.x, bottomLeft.y, 1.f), Vector2(uvBottomLeft.x, uvTopRight.y), newColor));
			m_meshBuilder->AddVertex(Vertex(Vector3(topRight.x, topRight.y, 1.f), Vector2(uvTopRight.x, uvBottomLeft.y), newColor));
			m_meshBuilder->AddVertex(Vertex(Vector3(bottomLeft.x, topRight.y, 1.f), Vector2(uvBottomLeft.x, uvBottomLeft.y), newColor));
			m_meshBuilder->AddVertex(Vertex(Vector3(topRight.x, bottomLeft.y, 1.f), Vector2(uvTopRight.x, uvTopRight.y), newColor));

			m_meshBuilder->AddIndex(charCount + 0);
			m_meshBuilder->AddIndex(charCount + 1);
			m_meshBuilder->AddIndex(charCount + 2);
			m_meshBuilder->AddIndex(charCount + 0);
			m_meshBuilder->AddIndex(charCount + 3);
			m_meshBuilder->AddIndex(charCount + 1);

			cursor.x += glyph->xAdvance * scale;
			prevCharacterCode = *characterCode;
			++characterCode;

			int kearning = font->GetKearning(prevCharacterCode, *characterCode);
			cursor.x += kearning * scale;

			charCount += 4;
		}
	}

	m_meshBuilder->End();
}

void SimpleRenderer::DrawText2DCenteredOnPosition(char const *string, Vector2 position, RGBA color /*= RGBA::WHITE*/, float scale /*= 1.f*/, Font *font /*= nullptr*/)
{
	if (font == nullptr)
	{
		font = m_font;
	}

	float stringWidth = font->CalculateTextWidth(string, scale * m_windowToScreenRatio.x);
	float stringHeight = font->CalculateTextHeight(string, scale * m_windowToScreenRatio.y);
	Vector2 cursor = position;
	cursor.x = cursor.x - (stringWidth * 0.5f);
	cursor.y = cursor.y + (stringHeight * 0.5f);

	char const *characterCode = string;
	int prevCharacterCode = -1;
	unsigned int charCount = 0;
	RGBA newColor = color;


	m_meshBuilder->Begin(true, font->m_texture);

	while (*characterCode != NULL)
	{
		if (*characterCode == '{')
		{
			++characterCode;
			if (*characterCode == '-')
			{
				newColor = color;
				++characterCode;
				++characterCode;
			}
			else if (*characterCode >= 48 && *characterCode <= 57) //if character code is a number
			{
				std::string rStr, gStr, bStr;
				float r, g, b;
				while (*characterCode != ',')
				{
					rStr += *characterCode;
					++characterCode;
				}
				r = (float)atof(rStr.c_str());
				++characterCode;

				while (*characterCode != ',')
				{
					gStr += *characterCode;
					++characterCode;
				}
				g = (float)atof(gStr.c_str());
				++characterCode;

				while (*characterCode != '}')
				{
					bStr += *characterCode;
					++characterCode;
				}
				b = (float)atof(bStr.c_str());

				newColor.SetAsFloats(r, g, b, 1.f);
				++characterCode;
			}
			else
			{
				--characterCode;
			}
		}

		t_FontData const *glyph = font->GetGlyph(*characterCode);

		if (glyph == nullptr)
			glyph = font->GetInvalidGlyph();

		float scaleW = (float)font->m_scale.x;
		float scaleH = (float)font->m_scale.y;

		Vector2 topLeft = cursor + (Vector2((float)glyph->xOffset * scale * m_windowToScreenRatio.x, (float)-glyph->yOffset * scale * m_windowToScreenRatio.y));
		Vector2 bottomLeft = topLeft + Vector2(0.f, (float)-glyph->height * scale * m_windowToScreenRatio.y);
		Vector2 topRight = topLeft + Vector2((float)glyph->width * scale * m_windowToScreenRatio.x, 0.f);

		Vector2 uvTopLeft = Vector2((float)glyph->x / scaleW, (float)glyph->y / scaleH);
		Vector2 uvTopRight = uvTopLeft + Vector2(0.f, (float)glyph->height / scaleH);
		Vector2 uvBottomLeft = uvTopLeft + Vector2((float)glyph->width / scaleW, 0.f);
		SwapValues(uvBottomLeft.x, uvTopRight.x);

		AddQuadToMeshBuilder(bottomLeft, topRight, newColor, uvBottomLeft, uvTopRight, charCount);

		cursor.x += glyph->xAdvance * scale * m_windowToScreenRatio.x;
		prevCharacterCode = *characterCode;
		++characterCode;

		int kearning = font->GetKearning(prevCharacterCode, *characterCode);
		cursor.x += kearning * scale * m_windowToScreenRatio.x;

		charCount += 4;
	}

	m_meshBuilder->End();
}

void SimpleRenderer::DrawTextList2DCenteredOnPosition(std::vector<std::string>& stringList, Vector2 position, Vector2 positionDelta, RGBA color /*= RGBA::WHITE*/, float scale /*= 1.f*/, Font *font /*= nullptr*/)
{
	std::vector<std::string>::iterator stringIter;
	for (stringIter = stringList.begin(); stringIter != stringList.end(); ++stringIter)
	{
		std::string currentString = *stringIter;
		DrawText2DCenteredOnPosition(currentString.c_str(), position, color, scale, font);
		position += positionDelta;
	}
}

void SimpleRenderer::DrawText2DCenteredAndScaledToFitWithinBounds(char const *string, AABB2D bounds, RGBA color /*= RGBA::WHITE*/, Font *font /*= nullptr*/)
{
	if (font == nullptr)
	{
		font = m_font;
	}

	Vector2 boundsSize = bounds.CalcSize();
	float scale = GetMin(boundsSize.x * m_windowToScreenRatio.x, boundsSize.y * m_windowToScreenRatio.y);
	float stringWidth = 0.f;
	float stringHeight = 0.f;

	do 
	{
		stringWidth = font->CalculateTextWidth(string, scale);
		stringHeight = font->CalculateTextHeight(string, scale);

		if (stringWidth > boundsSize.x)
		{
			scale *= (boundsSize.x / stringWidth);
		}
		else if (stringHeight > boundsSize.y)
		{
			scale *= (boundsSize.y / stringHeight);
		}
	} while (stringWidth > boundsSize.x || stringHeight > boundsSize.y);

	Vector2 cursor = bounds.CalcCenter();
	cursor.x = cursor.x - (stringWidth * 0.5f);
	cursor.y = cursor.y + (stringHeight * 0.5f);

	char const *characterCode = string;
	int prevCharacterCode = -1;
	unsigned int charCount = 0;
	RGBA newColor = color;


	m_meshBuilder->Begin(true, font->m_texture);

	while (*characterCode != NULL)
	{
		if (*characterCode == '{')
		{
			++characterCode;
			if (*characterCode == '-')
			{
				newColor = color;
				++characterCode;
				++characterCode;
			}
			else if (*characterCode >= 48 && *characterCode <= 57) //if character code is a number
			{
				std::string rStr, gStr, bStr;
				float r, g, b;
				while (*characterCode != ',')
				{
					rStr += *characterCode;
					++characterCode;
				}
				r = (float)atof(rStr.c_str());
				++characterCode;

				while (*characterCode != ',')
				{
					gStr += *characterCode;
					++characterCode;
				}
				g = (float)atof(gStr.c_str());
				++characterCode;

				while (*characterCode != '}')
				{
					bStr += *characterCode;
					++characterCode;
				}
				b = (float)atof(bStr.c_str());

				newColor.SetAsFloats(r, g, b, 1.f);
				++characterCode;
			}
			else
			{
				--characterCode;
			}
		}

		t_FontData const *glyph = font->GetGlyph(*characterCode);

		if (glyph == nullptr)
			glyph = font->GetInvalidGlyph();

		float scaleW = (float)font->m_scale.x;
		float scaleH = (float)font->m_scale.y;

		Vector2 topLeft = cursor + (Vector2((float)glyph->xOffset * scale, (float)-glyph->yOffset * scale));
		Vector2 bottomLeft = topLeft + Vector2(0.f, (float)-glyph->height * scale);
		Vector2 topRight = topLeft + Vector2((float)glyph->width * scale, 0.f);

		Vector2 uvTopLeft = Vector2((float)glyph->x / scaleW, (float)glyph->y / scaleH);
		Vector2 uvTopRight = uvTopLeft + Vector2(0.f, (float)glyph->height / scaleH);
		Vector2 uvBottomLeft = uvTopLeft + Vector2((float)glyph->width / scaleW, 0.f);
		SwapValues(uvBottomLeft.x, uvTopRight.x);

		AddQuadToMeshBuilder(bottomLeft, topRight, newColor, uvBottomLeft, uvTopRight, charCount);

		cursor.x += glyph->xAdvance * scale;
		prevCharacterCode = *characterCode;
		++characterCode;

		int kearning = font->GetKearning(prevCharacterCode, *characterCode);
		cursor.x += kearning * scale;

		charCount += 4;
	}

	m_meshBuilder->End();
}

void SimpleRenderer::GetTextQuad2DCenteredAndScaledToFitWithinBounds(AABB2D bounds, RGBA color, Font *font, char glyphCharacter, VertexBuffer **vertexBuffer, IndexBuffer **indexBuffer)
{
	float scale = 1.f;
	std::string glyphString = "";
	glyphString += glyphCharacter;
	float textWidth = font->CalculateTextWidth(glyphString.c_str(), scale);
	float textHeight = font->GetMaxTextHeight(glyphString.c_str(), scale);
	float boundsWidth = (bounds.maxs.x - bounds.mins.x);
	float boundsHeight = (bounds.maxs.y - bounds.mins.y);
	if (textWidth > boundsWidth && textWidth > textHeight)
	{
		scale = (boundsWidth / textWidth);
	}
	else if (textHeight > boundsHeight && textHeight > textWidth)
	{
		scale = (boundsHeight / textHeight);
	}
	textWidth = font->CalculateTextWidth(glyphString.c_str(), scale);
	textHeight = font->GetMaxTextHeight(glyphString.c_str(), scale);

	Vector2 boundsCenter = Vector2((bounds.maxs.x - (boundsWidth / 2.f)), (bounds.maxs.y - (boundsHeight / 2.f)));
	Vector2 cursor = Vector2((boundsCenter.x - (textWidth / 2.f)), boundsCenter.y + (textHeight * (7.f / 10.f)));
	cursor = cursor / scale;
	cursor.x = FastFloor(cursor.x);
	cursor.y = FastFloor(cursor.y);

	SetTexture(0, font->m_texture);
	RGBA newColor = color;

	t_FontData const *glyph = font->GetGlyph(glyphCharacter);

	if (glyph == nullptr)
	{
		glyph = font->GetInvalidGlyph();
	}

	float scaleW = (float)font->m_scale.x;
	float scaleH = (float)font->m_scale.y;

	Vector2 topLeft = cursor + Vector2((float)glyph->xOffset, (float)-glyph->yOffset);
	Vector2 topRight = topLeft + Vector2(0.f, (float)-glyph->height);
	Vector2 bottomLeft = topLeft + Vector2((float)glyph->width, 0.f);

	Vector2 uvTopLeft = Vector2((float)glyph->x / scaleW, (float)glyph->y / scaleH);
	Vector2 uvBottomLeft = uvTopLeft + Vector2(0.f, (float)glyph->height / scaleH);
	Vector2 uvTopRight = uvTopLeft + Vector2((float)glyph->width / scaleW, 0.f);
	float tempX = uvBottomLeft.x;
	uvBottomLeft.x = uvTopRight.x;
	uvTopRight.x = tempX;

	CreateTexturedQuad2D(bottomLeft * scale, topRight * scale, uvBottomLeft, uvTopRight, newColor, *&vertexBuffer, *&indexBuffer);
}

void SimpleRenderer::DrawText2DScaledWithAspectRatio(Vector2 position, float scaleX, float scaleY, RGBA color, Font *font, char const *string)
{
	Vector2 cursor = Vector2(position.x / scaleX, position.y / scaleY);

	char const *characterCode = string;
	int prevCharacterCode = -1;

	std::vector<VertexBuffer*> glyphVBA;
	std::vector<IndexBuffer*> glyphIBA;
	SetTexture(0, font->m_texture);
	RGBA newColor = color;

	while (*characterCode != NULL)
	{
		if (*characterCode == '[')
		{
			++characterCode;
			if (*characterCode == '-')
			{
				newColor = color;
				++characterCode;
				++characterCode;
			}
			else
			{
				std::string rStr, gStr, bStr;
				float r, g, b;
				while (*characterCode != ',')
				{
					rStr += *characterCode;
					++characterCode;
				}
				r = (float)atof(rStr.c_str());
				++characterCode;

				while (*characterCode != ',')
				{
					gStr += *characterCode;
					++characterCode;
				}
				g = (float)atof(gStr.c_str());
				++characterCode;

				while (*characterCode != ']')
				{
					bStr += *characterCode;
					++characterCode;
				}
				b = (float)atof(bStr.c_str());

				newColor.SetAsFloats(r, g, b, 1.f);
				++characterCode;

			}
		}

		t_FontData const *glyph = font->GetGlyph(*characterCode);

		if (glyph == nullptr)
		{
			glyph = font->GetInvalidGlyph();
		}

		float scaleW = (float)font->m_scale.x;
		float scaleH = (float)font->m_scale.y;

		Vector2 topLeft = cursor + Vector2((float)glyph->xOffset, (float)-glyph->yOffset);
		Vector2 topRight = topLeft + Vector2(0.f, (float)-glyph->height);
		Vector2 bottomLeft = topLeft + Vector2((float)glyph->width, 0.f);

		Vector2 uvTopLeft = Vector2((float)glyph->x / scaleW, (float)glyph->y / scaleH);
		Vector2 uvBottomLeft = uvTopLeft + Vector2(0.f, (float)glyph->height / scaleH);
		Vector2 uvTopRight = uvTopLeft + Vector2((float)glyph->width / scaleW, 0.f);
		float tempX = uvBottomLeft.x;
		uvBottomLeft.x = uvTopRight.x;
		uvTopRight.x = tempX;

		VertexBuffer *glyphVB = nullptr;
		IndexBuffer *glyphIB = nullptr;
		Vector2 bottomLeftScaled = Vector2(bottomLeft.x * scaleX, bottomLeft.y * scaleY);
		Vector2 topRightScaled = Vector2(topRight.x * scaleX, topRight.y * scaleY);
		CreateTexturedQuad2D(bottomLeftScaled, topRightScaled, uvBottomLeft, uvTopRight, newColor, &glyphVB, &glyphIB);
		glyphVBA.push_back(glyphVB);
		glyphIBA.push_back(glyphIB);

		cursor.x += glyph->xAdvance;
		prevCharacterCode = *characterCode;
		++characterCode;

		int kearning = font->GetKearning(prevCharacterCode, *characterCode);
		cursor.x += kearning;
	}
	DrawVertexBufferArray(PRIMITIVE_TRIANGLES, glyphVBA, glyphIBA);

	std::vector<VertexBuffer*>::iterator glyphVBAIter;
	std::vector<IndexBuffer*>::iterator glyphIBAIter;;

	for (glyphVBAIter = glyphVBA.begin(); glyphVBAIter != glyphVBA.end(); ++glyphVBAIter)
	{
		delete *glyphVBAIter;
		*glyphVBAIter = nullptr;
	}
	glyphVBA.clear();

	for (glyphIBAIter = glyphIBA.begin(); glyphIBAIter != glyphIBA.end(); ++glyphIBAIter)
	{
		delete *glyphIBAIter;
		*glyphIBAIter = nullptr;
	}
	glyphIBA.clear();
}

//#FIXME
// void SimpleRenderer::DrawText3D(Vector3 position, float scale, RGBA color, Font *font, char const *string)
// {
// 	Vector3 cursor = position;
// 
// 	char const *characterCode = string;
// 	int prevCharacterCode = -1;
// 
// 	std::vector<VertexBuffer*> glyphVBA;
// 	SetTexture(0, font->m_texture);
// 	RGBA newColor = color;
// 
// 	while (*characterCode != NULL)
// 	{
// 		if (*characterCode == '[')
// 		{
// 			++characterCode;
// 			if (*characterCode == '-')
// 			{
// 				newColor = color;
// 				++characterCode;
// 				++characterCode;
// 			}
// 			else
// 			{
// 				std::string rStr, gStr, bStr;
// 				float r, g, b;
// 				while (*characterCode != ',')
// 				{
// 					rStr += *characterCode;
// 					++characterCode;
// 				}
// 				r = (float)atof(rStr.c_str());
// 				++characterCode;
// 
// 				while (*characterCode != ',')
// 				{
// 					gStr += *characterCode;
// 					++characterCode;
// 				}
// 				g = (float)atof(gStr.c_str());
// 				++characterCode;
// 
// 				while (*characterCode != ']')
// 				{
// 					bStr += *characterCode;
// 					++characterCode;
// 				}
// 				b = (float)atof(bStr.c_str());
// 
// 				newColor.SetAsFloats(r, g, b, 1.f);
// 				++characterCode;
// 
// 			}
// 		}
// 
// 		t_FontData const *glyph = font->GetGlyph(*characterCode);
// 
// 		if (glyph == nullptr)
// 		{
// 			glyph = font->GetInvalidGlyph();
// 		}
// 
// 		float scaleW = 512.f;
// 		float scaleH = 512.f;
// 
// 		Vector3 topLeft = cursor + Vector3((float)glyph->xOffset, (float)-glyph->yOffset, 0.f);
// 		Vector3 topRight = topLeft + Vector3(0.f, (float)-glyph->height, 0.f);
// 		Vector3 bottomLeft = topLeft + Vector3((float)glyph->width, 0.f, 0.f);
// 
// 		Vector3 uvTopLeft = Vector3((float)glyph->x / scaleW, (float)glyph->y / scaleH, 0.f);
// 		Vector3 uvBottomLeft = uvTopLeft + Vector3(0.f, (float)glyph->height / scaleH, 0.f);
// 		Vector3 uvTopRight = uvTopLeft + Vector3((float)glyph->width / scaleW, 0.f, 0.f);
// 		float tempX = uvBottomLeft.x;
// 		uvBottomLeft.x = uvTopRight.x;
// 		uvTopRight.x = tempX;
// 
// 		VertexBuffer *glyphQuad = CreateTexturedQuad3DVertexBuffer(bottomLeft * scale, topRight * scale, uvBottomLeft, uvTopRight, newColor);
// // 		VertexBuffer *glyphQuad = CreateTexturedQuad2D(bottomLeft * scale, topRight* scale, uvBottomLeft, uvTopRight, newColor);
// 		glyphVBA.push_back(glyphQuad);
// 
// 		cursor.x += glyph->xAdvance;
// 		prevCharacterCode = *characterCode;
// 		++characterCode;
// 
// 		int kearning = font->GetKearning(prevCharacterCode, *characterCode);
// 		cursor.x += kearning;
// 	}
// 	DrawVertexBufferArray(PRIMITIVE_TRIANGLES, glyphVBA);
// 
// 	std::vector<VertexBuffer*>::iterator glyphIter;
// 
// 	for (glyphIter = glyphVBA.begin(); glyphIter != glyphVBA.end(); ++glyphIter)
// 	{
// 		delete *glyphIter;
// 		*glyphIter = nullptr;
// 	}
// 	glyphVBA.clear();
// }

VertexBuffer* SimpleRenderer::CreateQuad2DVertexBuffer(float x, float y, float z, RGBA color)
{
	Vertex vertices[6] =
	{
		Vertex(Vector3(-x, -y, z),	Vector2(0.f, 0.f), color),
		Vertex(Vector3(x, y, z),	Vector2(0.f, 0.f), color),
		Vertex(Vector3(-x, y, z),	Vector2(0.f, 0.f), color),
		Vertex(Vector3(-x, -y, z),	Vector2(0.f, 0.f), color),
		Vertex(Vector3(x, -y, z),	Vector2(0.f, 0.f), color),
		Vertex(Vector3(x, y, z),	Vector2(0.f, 0.f), color),
	};

	VertexBuffer* quadVBO = m_rhiDevice->CreateVertexBuffer(vertices, 6);

	return quadVBO;
}

void SimpleRenderer::DrawQuad2D(Vector2 bottomLeft, Vector2 topRight, RGBA color)
{
	m_meshBuilder->Begin(true, m_whiteTexture);
	AddQuadToMeshBuilder(bottomLeft, topRight, color);
	m_meshBuilder->End();
}

void SimpleRenderer::DrawQuad2D(Vector2 bottomLeft, Vector2 topRight, Vector2 uvBottomLeft, Vector2 uvTopRight, RGBA color)
{
	m_meshBuilder->Begin(true, m_currentSetTexture);
	AddQuadToMeshBuilder(bottomLeft, topRight, color, uvBottomLeft, uvTopRight);
	m_meshBuilder->End();
}

void SimpleRenderer::DrawQuad2DTextured(const AABB2D& bounds, const AABB2D uvBounds, Texture2D *texture, float orientation /*= 0.f*/, const RGBA& color /*= RGBA::WHITE*/)
{
	//#FIXME: Use mesh builder
	SetTexture(texture);

	Vector2 size = bounds.CalcSize();
	AABB2D centerBounds;
	centerBounds.SetCenterAndRadii(Vector2::ZERO, size.x * 0.5f, size.y * 0.5f);

	std::vector<Vertex> vertices;
	vertices.push_back(Vertex(Vector3(centerBounds.mins.x, centerBounds.mins.y, 0.f), Vector2(uvBounds.mins.x, uvBounds.maxs.y), color));
	vertices.push_back(Vertex(Vector3(centerBounds.maxs.x, centerBounds.maxs.y, 0.f), Vector2(uvBounds.maxs.x, uvBounds.mins.y), color));
	vertices.push_back(Vertex(Vector3(centerBounds.mins.x, centerBounds.maxs.y, 0.f), Vector2(uvBounds.mins.x, uvBounds.mins.y), color));
	vertices.push_back(Vertex(Vector3(centerBounds.maxs.x, centerBounds.mins.y, 0.f), Vector2(uvBounds.maxs.x, uvBounds.maxs.y), color));

	std::vector<DWORD> indices;
	indices.push_back(0);
	indices.push_back(1);
	indices.push_back(2);
	indices.push_back(0);
	indices.push_back(3);
	indices.push_back(1);

	Matrix4 modelMatrix;
	modelMatrix.RotateDegreesAboutZ(orientation);
	Vector2 centerOfBounds = bounds.CalcCenter();
	modelMatrix.SetTranslation(Vector3(centerOfBounds.x, centerOfBounds.y, 0.f));
	SetModelMatrix(modelMatrix);

	std::vector<VertexBuffer*> vba;
	std::vector<IndexBuffer*> iba;
	VertexBuffer *vertexBuffer = m_rhiDevice->CreateVertexBuffer(vertices);
	IndexBuffer *indexBuffer = m_rhiDevice->CreateIndexBuffer(indices);
	vba.push_back(vertexBuffer);
	iba.push_back(indexBuffer);
	DrawVertexBufferArray(PRIMITIVE_TRIANGLES, vba, iba);

	modelMatrix.MakeIdentity();
	modelMatrix.SetTranslation(Vector3(0.f, 0.f, 0.f));
	SetModelMatrix(modelMatrix);

	delete vba[0];
	vba[0] = nullptr;
	vba.clear();

	delete iba[0];
	iba[0] = nullptr;
	iba.clear();
}

void SimpleRenderer::DrawQuad2DTextured(Vector2 bottomLeft, Vector2 topRight, Texture2D *texture, RGBA color /*= RGBA::WHITE*/)
{
	m_meshBuilder->Begin(true, texture);
	AddQuadToMeshBuilder(bottomLeft, topRight, color);
	m_meshBuilder->End();
}

void SimpleRenderer::DrawQuad2DTextured(const AABB2D& bounds, Texture2D* texture, RGBA color /*= RGBA::WHITE*/)
{
	m_meshBuilder->Begin(true, texture);
	AddQuadToMeshBuilder(bounds.mins, bounds.maxs, color);
	m_meshBuilder->End();
}

void SimpleRenderer::CreateQuad2DTextured(const AABB2D& bounds, const AABB2D uvBounds, Texture2D *texture, const RGBA& color, VertexBuffer **vertexBuffer, IndexBuffer **indexBuffer)
{
	SetTexture(texture);

	std::vector<Vertex> vertices;
	vertices.push_back(Vertex(Vector3(bounds.mins.x, bounds.mins.y, 1.f), Vector2(uvBounds.mins.x, uvBounds.maxs.y), color));
	vertices.push_back(Vertex(Vector3(bounds.maxs.x, bounds.maxs.y, 1.f), Vector2(uvBounds.maxs.x, uvBounds.mins.y), color));
	vertices.push_back(Vertex(Vector3(bounds.mins.x, bounds.maxs.y, 1.f), Vector2(uvBounds.mins.x, uvBounds.mins.y), color));
	vertices.push_back(Vertex(Vector3(bounds.maxs.x, bounds.mins.y, 1.f), Vector2(uvBounds.maxs.x, uvBounds.maxs.y), color));

	std::vector<DWORD> indices;
	indices.push_back(0);
	indices.push_back(1);
	indices.push_back(2);
	indices.push_back(0);
	indices.push_back(3);
	indices.push_back(1);

	*vertexBuffer = m_rhiDevice->CreateVertexBuffer(vertices);
	*indexBuffer = m_rhiDevice->CreateIndexBuffer(indices);
}

void SimpleRenderer::DrawLineRotated2D(const Vector2& startPoint, const Vector2& endPoint, float lineThickness, const RGBA& startColor, const RGBA& endColor, const float& rotation, const Vector2& newMidPoint)
{
	float distanceBetweenVectorsHalved = CalcDistance(startPoint, endPoint) / 2.f;
	Vector2 midPoint = (startPoint + endPoint) / 2.f;
	Vector2 extraBoundsDistance;
	if (newMidPoint != Vector2::ZERO)
	{
		extraBoundsDistance = newMidPoint - midPoint;
	}
	Vector2 minsBounds = Vector2(-lineThickness, -distanceBetweenVectorsHalved);
	minsBounds += extraBoundsDistance;
	Vector2 maxsBounds = Vector2(lineThickness, distanceBetweenVectorsHalved);
	maxsBounds += extraBoundsDistance;
	AABB2D bounds = AABB2D(minsBounds, maxsBounds);
	AABB2D uvBounds = AABB2D(Vector2(0.f, 0.f), Vector2(1.f, 1.f));

	m_meshBuilder->Begin(true, m_whiteTexture);

	m_meshBuilder->AddVertex(Vertex(Vector3(bounds.mins.x, bounds.mins.y, 1.f), Vector2(uvBounds.mins.x, uvBounds.maxs.y), startColor));
	m_meshBuilder->AddVertex(Vertex(Vector3(bounds.maxs.x, bounds.maxs.y, 1.f), Vector2(uvBounds.maxs.x, uvBounds.mins.y), endColor));
	m_meshBuilder->AddVertex(Vertex(Vector3(bounds.mins.x, bounds.maxs.y, 1.f), Vector2(uvBounds.mins.x, uvBounds.mins.y), endColor));
	m_meshBuilder->AddVertex(Vertex(Vector3(bounds.maxs.x, bounds.mins.y, 1.f), Vector2(uvBounds.maxs.x, uvBounds.maxs.y), startColor));

	m_meshBuilder->AddIndex(0);
	m_meshBuilder->AddIndex(1);
	m_meshBuilder->AddIndex(2);
	m_meshBuilder->AddIndex(0);
	m_meshBuilder->AddIndex(3);
	m_meshBuilder->AddIndex(1);

	Matrix4 modelMatrix;
	if (newMidPoint != Vector2::ZERO)
	{
		float degreesBetweenVectors = CalcAngleBetweenVectors(endPoint, startPoint);
		modelMatrix.RotateDegreesAboutZ(degreesBetweenVectors);
		modelMatrix.RotateDegreesAboutZ(rotation);
		modelMatrix.SetTranslation(Vector3(newMidPoint.x, newMidPoint.y, 0.f));
	}
	else
	{
		float degreesBetweenVectors = CalcAngleBetweenVectors(endPoint, startPoint);
		modelMatrix.RotateDegreesAboutZ(degreesBetweenVectors);
		modelMatrix.SetTranslation(Vector3(midPoint.x, midPoint.y, 0.f));
	}

	m_meshBuilder->SetModelMatrix(modelMatrix);

	m_meshBuilder->End();
}

void SimpleRenderer::DrawLine2D(const Vector2& startPoint, const Vector2& endPoint, float lineThickness, const RGBA& startColor /*= RGBA::WHITE*/, const RGBA& endColor /*= RGBA::WHITE*/)
{
	m_meshBuilder->Begin(true, m_whiteTexture);
	Vector2 bottomLeft = Vector2(endPoint.x - (lineThickness * 0.5f), startPoint.y - (lineThickness * 0.5f));
	Vector2 topRight = Vector2(startPoint.x + (lineThickness * 0.5f), endPoint.y + (lineThickness * 0.5f));
	AABB2D uvBounds = AABB2D(Vector2(0.f, 0.f), Vector2(1.f, 1.f));

	m_meshBuilder->AddVertex(Vertex(Vector3(bottomLeft.x, bottomLeft.y, 1.f),	Vector2(uvBounds.mins.x, uvBounds.maxs.y), startColor));
	m_meshBuilder->AddVertex(Vertex(Vector3(topRight.x, topRight.y, 1.f),		Vector2(uvBounds.maxs.x, uvBounds.mins.y), endColor));
	m_meshBuilder->AddVertex(Vertex(Vector3(bottomLeft.x, topRight.y, 1.f),		Vector2(uvBounds.mins.x, uvBounds.mins.y), endColor));
	m_meshBuilder->AddVertex(Vertex(Vector3(topRight.x, bottomLeft.y, 1.f),		Vector2(uvBounds.maxs.x, uvBounds.maxs.y), startColor));

	m_meshBuilder->AddIndex(0);
	m_meshBuilder->AddIndex(1);
	m_meshBuilder->AddIndex(2);
	m_meshBuilder->AddIndex(0);
	m_meshBuilder->AddIndex(3);
	m_meshBuilder->AddIndex(1);

	m_meshBuilder->End();
}

void SimpleRenderer::DrawGrid2D(const Grid& grid, const AABB2D& gridBounds, float lineThickness /*= 1.f*/, const RGBA& color /*= RGBA::WHITE*/)
{
	int numColumns = grid.GetNumColumns();
	int numRows = grid.GetNumRows();
	float columnWidth = gridBounds.CalcSize().x / (float)numColumns;
	float rowHeight = gridBounds.CalcSize().y / (float)numRows;

	for (int columnCount = 0; columnCount <= numColumns; columnCount++)
	{
		float xSpacing = gridBounds.mins.x + (columnCount * columnWidth);
		Vector2 startPoint = Vector2(xSpacing, gridBounds.mins.y);
		Vector2 endPoint = Vector2(xSpacing, gridBounds.maxs.y);
		g_theSimpleRenderer->DrawLineRotated2D(startPoint, endPoint, lineThickness, color, color);
	}

	for (int rowCount = 0; rowCount <= numRows; rowCount++)
	{
		float ySpacing = gridBounds.mins.y + (rowCount * rowHeight);
		Vector2 startPoint = Vector2(gridBounds.mins.x, ySpacing);
		Vector2 endPoint = Vector2(gridBounds.maxs.x, ySpacing);
		g_theSimpleRenderer->DrawLineRotated2D(startPoint, endPoint, lineThickness, color, color);
	}
}

void SimpleRenderer::DrawDisc2D(const Vector2& center, const float radius, const RGBA& color, int numVertices)
{
	AABB2D uvBounds = AABB2D(Vector2(0.f, 0.f), Vector2(1.f, 1.f));
	std::vector<Vertex> vertices;
// 	vertices.push_back(Vertex(Vector3(position.x, position.y, 1.f), Vector2(uvBounds.mins.x, uvBounds.maxs.y), edgeColor));
	std::vector<DWORD> indices;

	float degreesPerVertex = (float)(360.f / numVertices);
	for (int vertexIndex = 0; vertexIndex < numVertices; ++vertexIndex)
	{
		float degreesThisVertex = degreesPerVertex * (float)vertexIndex;
		float x = center.x + (radius * CosDegrees(degreesThisVertex));
		float y = center.y + (radius * SinDegrees(degreesThisVertex));
		vertices.push_back(Vertex(Vector3(x, y, 1.f), Vector2(uvBounds.mins.x, uvBounds.maxs.y), color));

		if (vertexIndex != 0)
		{
			indices.push_back(0);
			indices.push_back(vertexIndex);
			indices.push_back(vertexIndex + 1);
		}
		if (vertexIndex == numVertices - 1)
		{
			indices.push_back(0);
			indices.push_back(vertexIndex + 1);
			indices.push_back(1);
		}
	}

	std::vector<VertexBuffer*> vba;
	std::vector<IndexBuffer*> iba;
	VertexBuffer *vertexBuffer = m_rhiDevice->CreateVertexBuffer(vertices);
	IndexBuffer *indexBuffer = m_rhiDevice->CreateIndexBuffer(indices);
	vba.push_back(vertexBuffer);
	iba.push_back(indexBuffer);
	DrawVertexBufferArray(PRIMITIVE_TRIANGLES, vba, iba);

	delete vba[0];
	vba[0] = nullptr;
	vba.clear();

	delete iba[0];
	iba[0] = nullptr;
	iba.clear();
}

void SimpleRenderer::DrawDebugQuad2D(const AABB2D& bounds, float edgeThickness, const RGBA& interiorColor, const RGBA& edgeColor)
{
	m_meshBuilder->Begin(true, m_whiteTexture);

	AddQuadToMeshBuilder(bounds, interiorColor);

	AABB2D leftEdge = AABB2D(Vector2(bounds.mins.x - edgeThickness, bounds.mins.y - edgeThickness), Vector2(bounds.mins.x + edgeThickness, bounds.maxs.y + edgeThickness));
	AABB2D rightEdge = AABB2D(Vector2(bounds.maxs.x - edgeThickness, bounds.mins.y - edgeThickness), Vector2(bounds.maxs.x + edgeThickness, bounds.maxs.y + edgeThickness));
	AABB2D topEdge = AABB2D(Vector2(bounds.mins.x - edgeThickness, bounds.maxs.y - edgeThickness), Vector2(bounds.maxs.x + edgeThickness, bounds.maxs.y + edgeThickness));
	AABB2D bottomEdge = AABB2D(Vector2(bounds.mins.x - edgeThickness, bounds.mins.y - edgeThickness), Vector2(bounds.maxs.x + edgeThickness, bounds.mins.y + edgeThickness));
	AddQuadToMeshBuilder(leftEdge, edgeColor, Vector2::ZERO, Vector2::ONE, 4);
	AddQuadToMeshBuilder(rightEdge, edgeColor, Vector2::ZERO, Vector2::ONE, 8);
	AddQuadToMeshBuilder(topEdge, edgeColor, Vector2::ZERO, Vector2::ONE, 12);
	AddQuadToMeshBuilder(bottomEdge, edgeColor, Vector2::ZERO, Vector2::ONE, 16);

	m_meshBuilder->End();
}

void SimpleRenderer::DrawDebugCircle2D(const Vector2& position, float radius, float edgeThickness, int numSides, const RGBA& interiorColor, const RGBA& edgeColor)
{
	AABB2D uvBounds = AABB2D(Vector2(0.f, 0.f), Vector2(1.f, 1.f));
	std::vector<Vertex> vertices;
	vertices.push_back(Vertex(Vector3(position.x, position.y, 1.f), Vector2(uvBounds.mins.x, uvBounds.maxs.y), edgeColor));
	std::vector<DWORD> indices;
	
	float degreesPerVertex = (float)(360.f / numSides);
	for (int vertexIndex = 0; vertexIndex < numSides; ++vertexIndex)
	{
		float degreesThisVertex = degreesPerVertex * (float)vertexIndex;
		float x = position.x + (radius * CosDegrees(degreesThisVertex));
		float y = position.y + (radius * SinDegrees(degreesThisVertex));
		vertices.push_back(Vertex(Vector3(x, y, 1.f), Vector2(uvBounds.mins.x, uvBounds.maxs.y), edgeColor));

		if (vertexIndex != 0)
		{
			indices.push_back(0);
			indices.push_back(vertexIndex);
			indices.push_back(vertexIndex + 1);
		}
		if (vertexIndex == numSides - 1)
		{
			indices.push_back(0);
			indices.push_back(vertexIndex + 1);
			indices.push_back(1);
		}
	}

	vertices.push_back(Vertex(Vector3(position.x, position.y, 1.f), Vector2(uvBounds.mins.x, uvBounds.maxs.y), interiorColor));
	float innerRadius = radius - edgeThickness;
	for (int vertexIndex = 0; vertexIndex < numSides; ++vertexIndex)
	{
		float degreesThisVertex = degreesPerVertex * (float)vertexIndex;
		float x = position.x + (innerRadius * CosDegrees(degreesThisVertex));
		float y = position.y + (innerRadius * SinDegrees(degreesThisVertex));
		vertices.push_back(Vertex(Vector3(x, y, 1.f), Vector2(uvBounds.mins.x, uvBounds.maxs.y), interiorColor));

		if (vertexIndex != 0)
		{
			indices.push_back(numSides + 1);
			indices.push_back(vertexIndex + numSides + 1);
			indices.push_back(vertexIndex + numSides + 2);
		}
		if (vertexIndex == numSides - 1)
		{
			indices.push_back(numSides + 1);
			indices.push_back(vertexIndex + numSides + 2);
			indices.push_back(numSides + 2);
		}
	}

	std::vector<VertexBuffer*> vba;
	std::vector<IndexBuffer*> iba;
	VertexBuffer *vertexBuffer = m_rhiDevice->CreateVertexBuffer(vertices);
	IndexBuffer *indexBuffer = m_rhiDevice->CreateIndexBuffer(indices);
	vba.push_back(vertexBuffer);
	iba.push_back(indexBuffer);
	DrawVertexBufferArray(PRIMITIVE_TRIANGLES, vba, iba);

	delete vba[0];
	vba[0] = nullptr;
	vba.clear();

	delete iba[0];
	iba[0] = nullptr;
	iba.clear();
}

void SimpleRenderer::DrawCircleHollow2D(const Vector2& position, float radiusToEdge, float edgeThickness, int numSides, const RGBA& edgeColor)
{
	Vector2 lastVertex = Vector2::ZERO;
	Vector2 currentVertex = Vector2::ZERO;
	float degreesPerVertex = (float)(360.f / numSides);
	for (int vertexIndex = 0; vertexIndex < numSides; ++vertexIndex)
	{
		float degreesThisVertex = degreesPerVertex * (float)vertexIndex;
		float x = position.x + (radiusToEdge * CosDegrees(degreesThisVertex));
		float y = position.y + (radiusToEdge * SinDegrees(degreesThisVertex));
		currentVertex.x = x;
		currentVertex.y = y;

		if (lastVertex != Vector2::ZERO)
		{
			DrawLineRotated2D(lastVertex, currentVertex, edgeThickness, edgeColor, edgeColor);
		}
		lastVertex = currentVertex;
	}

	float x = position.x + (radiusToEdge * CosDegrees(0.f));
	float y = position.y + (radiusToEdge * SinDegrees(0.f));
	currentVertex.x = x;
	currentVertex.y = y;

	DrawLineRotated2D(lastVertex, currentVertex, edgeThickness, edgeColor, edgeColor);
}

void SimpleRenderer::AddQuadToMeshBuilder(Vector2 bottomLeft, Vector2 topRight, RGBA color /*= RGBA::WHITE*/, Vector2 uvBottomLeft /*= Vector2::ZERO*/, Vector2 uvTopRight /*= Vector2::ONE*/, int startingIndex /*= 0*/)
{
	m_meshBuilder->AddVertex(Vertex(Vector3(bottomLeft.x, bottomLeft.y, 1.f), Vector2(uvBottomLeft.x, uvTopRight.y), color));
	m_meshBuilder->AddVertex(Vertex(Vector3(topRight.x, topRight.y, 1.f), Vector2(uvTopRight.x, uvBottomLeft.y), color));
	m_meshBuilder->AddVertex(Vertex(Vector3(bottomLeft.x, topRight.y, 1.f), Vector2(uvBottomLeft.x, uvBottomLeft.y), color));
	m_meshBuilder->AddVertex(Vertex(Vector3(topRight.x, bottomLeft.y, 1.f), Vector2(uvTopRight.x, uvTopRight.y), color));

	m_meshBuilder->AddIndex(startingIndex + 0);
	m_meshBuilder->AddIndex(startingIndex + 1);
	m_meshBuilder->AddIndex(startingIndex + 2);
	m_meshBuilder->AddIndex(startingIndex + 0);
	m_meshBuilder->AddIndex(startingIndex + 3);
	m_meshBuilder->AddIndex(startingIndex + 1);
}

void SimpleRenderer::AddQuadToMeshBuilder(AABB2D quadBounds, RGBA color /*= RGBA::WHITE*/, Vector2 uvBottomLeft /*= Vector2::ZERO*/, Vector2 uvTopRight /*= Vector2::ONE*/, int startingIndex /*= 0*/)
{
	Vector2 bottomLeft = quadBounds.mins;
	Vector2 topRight = quadBounds.maxs;

	m_meshBuilder->AddVertex(Vertex(Vector3(bottomLeft.x, bottomLeft.y, 1.f), Vector2(uvBottomLeft.x, uvTopRight.y), color));
	m_meshBuilder->AddVertex(Vertex(Vector3(topRight.x, topRight.y, 1.f), Vector2(uvTopRight.x, uvBottomLeft.y), color));
	m_meshBuilder->AddVertex(Vertex(Vector3(bottomLeft.x, topRight.y, 1.f), Vector2(uvBottomLeft.x, uvBottomLeft.y), color));
	m_meshBuilder->AddVertex(Vertex(Vector3(topRight.x, bottomLeft.y, 1.f), Vector2(uvTopRight.x, uvTopRight.y), color));

	m_meshBuilder->AddIndex(startingIndex + 0);
	m_meshBuilder->AddIndex(startingIndex + 1);
	m_meshBuilder->AddIndex(startingIndex + 2);
	m_meshBuilder->AddIndex(startingIndex + 0);
	m_meshBuilder->AddIndex(startingIndex + 3);
	m_meshBuilder->AddIndex(startingIndex + 1);
}

void SimpleRenderer::DrawUIButton(const UIButton& uiButton)
{
	AABB2D buttonBox = uiButton.GetBoundingBox();
	DrawQuad2D(buttonBox.mins, buttonBox.maxs, uiButton.GetBoundingBoxColor());
	DrawText2DCenteredOnPosition(uiButton.GetText().c_str(), buttonBox.CalcCenter(), uiButton.GetTextColor());
}

VertexBuffer* SimpleRenderer::CreateTexturedQuad2DVertexBuffer(float x, float y, float z, float vec2XMin, float vec2YMin, float vec2XMax, float vec2YMax, RGBA color)
{
	Vertex vertices[6] =
	{
		Vertex(Vector3(-x, -y, z),	Vector2(vec2XMin, vec2YMax), color),
		Vertex(Vector3(x, y, z),	Vector2(vec2XMax, vec2YMin), color),
		Vertex(Vector3(-x, y, z),	Vector2(vec2XMin, vec2YMin), color),
		Vertex(Vector3(-x, -y, z),	Vector2(vec2XMin, vec2YMax), color),
		Vertex(Vector3(x, -y, z),	Vector2(vec2XMax, vec2YMax), color),
		Vertex(Vector3(x, y, z),	Vector2(vec2XMax, vec2YMin), color),
	};

	VertexBuffer* quadVBO = m_rhiDevice->CreateVertexBuffer(vertices, 6);

	return quadVBO;
}

void SimpleRenderer::CreateTexturedQuad2D(Vector2 bottomLeft, Vector2 topRight, Vector2 uvBottomLeft, Vector2 uvTopRight, RGBA color, VertexBuffer** vertBuffer, IndexBuffer** indBuffer)
{
	std::vector<Vertex> vertices;
	vertices.push_back(Vertex(Vector3(bottomLeft.x, bottomLeft.y, 1.f),	Vector2(uvBottomLeft.x, uvTopRight.y),		color));
	vertices.push_back(Vertex(Vector3(topRight.x, topRight.y, 1.f),		Vector2(uvTopRight.x, uvBottomLeft.y),		color));
	vertices.push_back(Vertex(Vector3(bottomLeft.x, topRight.y, 1.f),	Vector2(uvBottomLeft.x, uvBottomLeft.y),	color));
	vertices.push_back(Vertex(Vector3(topRight.x, bottomLeft.y, 1.f),	Vector2(uvTopRight.x, uvTopRight.y),		color));

	std::vector<DWORD> indices;
	indices.push_back(0);
	indices.push_back(1);
	indices.push_back(2);
	indices.push_back(0);
	indices.push_back(3);
	indices.push_back(1);

	*vertBuffer = m_rhiDevice->CreateVertexBuffer(vertices);
	*indBuffer = m_rhiDevice->CreateIndexBuffer(indices);
}

VertexBuffer* SimpleRenderer::CreateTexturedQuad3DVertexBuffer(Vector3 bottomLeft, Vector3 topRight, Vector3 uvBottomLeft, Vector3 uvTopRight, RGBA color)
{
	Vertex vertices[6] =
	{
		Vertex(Vector3(bottomLeft.x, bottomLeft.y, 1.f),	Vector2(uvBottomLeft.x, uvTopRight.y),		color),
		Vertex(Vector3(topRight.x, topRight.y, 1.f),		Vector2(uvTopRight.x, uvBottomLeft.y),		color),
		Vertex(Vector3(bottomLeft.x, topRight.y, 1.f),		Vector2(uvBottomLeft.x, uvBottomLeft.y),	color),
		Vertex(Vector3(bottomLeft.x, bottomLeft.y, 1.f),	Vector2(uvBottomLeft.x, uvTopRight.y),		color),
		Vertex(Vector3(topRight.x, bottomLeft.y, 1.f),		Vector2(uvTopRight.x, uvTopRight.y),		color),
		Vertex(Vector3(topRight.x, topRight.y, 1.f),		Vector2(uvTopRight.x, uvBottomLeft.y),		color),
	};

	VertexBuffer* quadVBO = m_rhiDevice->CreateVertexBuffer(vertices, 6);

	return quadVBO;
}

VertexBuffer* SimpleRenderer::CreateLine3DVertexBuffer(Vector3 const &startPos, Vector3 const &endPos, RGBA const &color /*= RGBA::WHITE*/)
{
	Vertex vertices[] =
	{
		Vertex(startPos,	Vector2(0.0f, 0.0f), color),
		Vertex(endPos,		Vector2(1.0f, 1.0f), color),
	};

	VertexBuffer *vertexBuffer = m_rhiDevice->CreateVertexBuffer(vertices, 2);
	return vertexBuffer;
}

VertexBuffer* SimpleRenderer::CreateQuad3D(Vector3 const &origin, Vector3 const &right, float negXExtents, float posXExtents, Vector3 const &up, float negYExtents, float posYExtents, RGBA const &color /*= RGBA::WHITE*/)
{
	Vector3 bl = origin + negXExtents * right + negYExtents * up;
	Vector3 tl = origin + negXExtents * right + posYExtents * up;
	Vector3 br = origin + posXExtents * right + negYExtents * up;
	Vector3 tr = origin + posXExtents * right + posYExtents * up;

	Vector3 normal = up.Cross(right);

	Vertex vertices[] =
	{
		Vertex(bl, normal,	Vector2(0.f, 1.f), right, color),
		Vertex(tr, normal,	Vector2(1.f, 0.f), right, color),
		Vertex(tl, normal,	Vector2(0.f, 0.f), right, color),
		Vertex(bl, normal,	Vector2(0.f, 1.f), right, color),
		Vertex(br, normal,	Vector2(1.f, 1.f), right, color),
		Vertex(tr, normal,	Vector2(1.f, 0.f), right, color),
	};
	VertexBuffer *vertexBuffer = m_rhiDevice->CreateVertexBuffer(vertices, 6);
	return vertexBuffer;
}

void SimpleRenderer::DrawQuad3D(Vector3 const &origin, Vector3 const &right, float negXExtents, float posXExtents, Vector3 const &up, float negYExtents, float posYExtents, RGBA const &color /*= RGBA::WHITE*/)
{
	Vector3 bl = origin + negXExtents * right + negYExtents * up;
	Vector3 tl = origin + negXExtents * right + posYExtents * up;
	Vector3 br = origin + posXExtents * right + negYExtents * up;
	Vector3 tr = origin + posXExtents * right + posYExtents * up;

	Vector3 normal = up.Cross(right);

	std::vector<Vertex> vertices;
	vertices.push_back(Vertex(bl, normal,	Vector2(0.f, 1.f), right, color));
	vertices.push_back(Vertex(tr, normal,	Vector2(1.f, 0.f), right, color));
	vertices.push_back(Vertex(tl, normal,	Vector2(0.f, 0.f), right, color));
	vertices.push_back(Vertex(br, normal,	Vector2(1.f, 1.f), right, color));

	std::vector<DWORD> indices;
	indices.push_back(0);
	indices.push_back(1);
	indices.push_back(2);
	indices.push_back(0);
	indices.push_back(3);
	indices.push_back(1);

	std::vector<VertexBuffer*> vba;
	std::vector<IndexBuffer*> iba;
	VertexBuffer *vertexBuffer = m_rhiDevice->CreateVertexBuffer(vertices);
	IndexBuffer *indexBuffer = m_rhiDevice->CreateIndexBuffer(indices);
	vba.push_back(vertexBuffer);
	iba.push_back(indexBuffer);
	DrawVertexBufferArray(PRIMITIVE_TRIANGLES, vba, iba);

	delete vba[0];
	vba[0] = nullptr;
	vba.clear();

	delete iba[0];
	iba[0] = nullptr;
	iba.clear();
}

void SimpleRenderer::DrawQuadDoubleSided3D(Vector3 const &origin, Vector3 const &right, float negXExtents, float posXExtents, Vector3 const &up, float negYExtents, float posYExtents, RGBA const &color /*= RGBA::WHITE*/)
{
	DrawQuad3D(origin, right, negXExtents, posXExtents, up, negYExtents, posYExtents, color);
	DrawQuad3D(Vector3(origin.x + 1.f, origin.y, origin.z), Vector3(-right.x, -right.y, -right.z), negXExtents, posXExtents, Vector3(up.x, up.y, up.z), negYExtents, posYExtents, color);
}

void SimpleRenderer::DrawCube3D(Vector3 const &position, RGBA const &color /*= RGBA::WHITE*/, float scale)
{
	DrawQuad3D(Vector3(position.x, position.y, position.z - scale),	Vector3(-1.f, 0.f, 0.f),	-scale, scale, Vector3(0.f, -1.f, 0.f), -scale, scale, color);
	DrawQuad3D(Vector3(position.x, position.y, position.z + scale),	Vector3(-1.f, 0.f, 0.f),	-scale, scale, Vector3(0.f, 1.f, 0.f),	-scale, scale, color);
	DrawQuad3D(Vector3(position.x + scale, position.y, position.z),	Vector3(0.f, 0.f, 1.f),		-scale, scale, Vector3(0.f, 1.f, 0.f),	-scale, scale, color);
	DrawQuad3D(Vector3(position.x - scale, position.y, position.z),	Vector3(0.f, 0.f, -1.f),	-scale, scale, Vector3(0.f, 1.f, 0.f),	-scale, scale, color);
	DrawQuad3D(Vector3(position.x, position.y + scale, position.z),	Vector3(-1.f, 0.f, 0.f),	-scale, scale, Vector3(0.f, 0.f, -1.f),	-scale, scale, color);
	DrawQuad3D(Vector3(position.x, position.y - scale, position.z),	Vector3(-1.f, 0.f, 0.f),	-scale, scale, Vector3(0.f, 0.f, 1.f),	-scale, scale, color);
}

void SimpleRenderer::DrawLine3D(Vector3 const &startPos, Vector3 const &endPos, RGBA const &color /*= RGBA::WHITE*/)
{
	std::vector<Vertex> vertices;
	vertices.push_back(Vertex(startPos, Vector2(0.f, 0.f), color));
	vertices.push_back(Vertex(endPos, Vector2(1.f, 1.f), color));

	std::vector<DWORD> indices;
	indices.push_back(0);
	indices.push_back(1);

	std::vector<VertexBuffer*> vba;
	std::vector<IndexBuffer*> iba;
	VertexBuffer *vertexBuffer = m_rhiDevice->CreateVertexBuffer(vertices);
	IndexBuffer *indexBuffer = m_rhiDevice->CreateIndexBuffer(indices);
	vba.push_back(vertexBuffer);
	iba.push_back(indexBuffer);
	DrawVertexBufferArray(PRIMITIVE_LINES, vba, iba);

	delete vba[0];
	vba[0] = nullptr;
	vba.clear();

	delete iba[0];
	iba[0] = nullptr;
	iba.clear();
}

void SimpleRenderer::DrawOriginLines3D()
{
	DrawLine3D(Vector3(0.f, 0.f, 0.f), Vector3(1.f, 0.f, 0.f), RGBA::RED);
	DrawLine3D(Vector3(0.f, 0.f, 0.f), Vector3(0.f, 1.f, 0.f), RGBA::GREEN);
	DrawLine3D(Vector3(0.f, 0.f, 0.f), Vector3(0.f, 0.f, 1.f), RGBA::BLUE);
}

// #FIXME
// void SimpleRenderer::DrawCylinder3D(Vector3 const &position, float radius, float height, int numVertexes, const RGBA& color /*= RGBA::WHITE*/)
// {
// 	DrawDisc3D(position, radius, numVertexes, color);

// 	for (int vertexIndex = 0; vertexIndex < numVertexes; ++vertexIndex)
// 	{
// 		float degreesThisVertex = degreesPerVertex * (float)vertexIndex;
// 		float x = radius * CosDegrees(degreesThisVertex);
// 		float z = radius * SinDegrees(degreesThisVertex);
// 		DrawLine3D(Vector3(position.x + x, position.y, position.z + z), Vector3(position.x + x, position.y + height, position.z + z), color);
// 	}

// 	DrawDisc3D(Vector3(position.x, position.y + height, position.z), radius, numVertexes, color);
// }

//#FIXME
// void SimpleRenderer::DrawDisc3D(Vector3 const &position, float radius, int numVertexes, const RGBA& color /*= RGBA::WHITE*/)
// {
// 	float degreesPerVertex = (float)(360.f / numVertexes);
// 	float prevX = position.x;
// 	float prevZ = position.z;
// 	float originalX = 0;
// 	float originalZ = 0;
// 
// 	for (int vertexIndex = 0; vertexIndex < numVertexes; ++vertexIndex)
// 	{
// 		float degreesThisVertex = degreesPerVertex * (float)vertexIndex;
// 		float x = radius * CosDegrees(degreesThisVertex);
// 		float z = radius * SinDegrees(degreesThisVertex);
// 		if (prevX == position.x && prevZ == position.z)
// 		{
// 			originalX = position.x + x;
// 			originalZ = position.z + z;
// 		}
// 		else
// 		{
// 			DrawLine3D(Vector3(position.x + prevX, position.y, position.z + prevZ), Vector3(position.x + x, position.y, position.z + z), color);
// 		}
// 		prevX = x;
// 		prevZ = z;
// 	}
// 
// 	float x = radius * CosDegrees(0.f);
// 	float z = radius * SinDegrees(0.f);
// 	DrawLine3D(Vector3(position.x + prevX, position.y, position.z + prevZ), Vector3(position.x + x, position.y, position.z + z), color);
// }
