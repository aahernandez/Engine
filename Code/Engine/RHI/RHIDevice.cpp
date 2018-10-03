#include "Engine/RHI/RHIDevice.hpp"
#include "Engine/RHI/RHIDeviceContext.hpp"
#include "Engine/RHI/ShaderProgram.hpp"
#include "Engine/Core/FileUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Renderer/Window.hpp"
#include "Engine/RHI/VertexBuffer.hpp"
#include "Engine/RHI/IndexBuffer.hpp"
#include "Engine/Core/EngineConfig.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/RHI/ComputeShaderProgram.hpp"

static ID3DBlob* CompileHLSLToShaderBlob(char const *opt_filename, void const *source_code, size_t const source_code_size, char const *entrypoint, char const* target);

RHIDevice::RHIDevice(RHIInstance *owner, ID3D11Device *dxd)
{
	m_instanceOwner = owner;
	m_dxDevice = dxd;
	m_immediateContext = nullptr;
	m_dxSwapChain = nullptr;
	m_dxContext = nullptr;
}

RHIDevice::~RHIDevice()
{
	D3D11Cleanup();
}

RHIDeviceContext* RHIDevice::GetImmediateContext()
{
	return m_immediateContext;
}

ShaderProgram* RHIDevice::CreateShaderFromHLSLFile(char const* filename)
{
	size_t srcSize = 0U;
	void *srcCode = FileReadToBuffer(filename, &srcSize);
	if (srcCode == nullptr) {
		ASSERT_OR_DIE(false, Stringf("File not found. %s", filename) );
		return nullptr;
	}

	ID3DBlob *vsByteCode = nullptr;
	ID3DBlob *fsByteCode = nullptr;

	vsByteCode = CompileHLSLToShaderBlob(filename, srcCode, srcSize, "VertexFunction", "vs_5_0");
	fsByteCode = CompileHLSLToShaderBlob(filename, srcCode, srcSize, "FragmentFunction", "ps_5_0");
	free(srcCode);

	if (vsByteCode != nullptr && fsByteCode != nullptr) {

		ID3D11VertexShader *vs = nullptr;
		ID3D11PixelShader *fs = nullptr;

		m_dxDevice->CreateVertexShader(vsByteCode->GetBufferPointer(),
			vsByteCode->GetBufferSize(),
			nullptr,
			&vs);

		m_dxDevice->CreatePixelShader(fsByteCode->GetBufferPointer(),
			fsByteCode->GetBufferSize(),
			nullptr,
			&fs);

		ShaderProgram *program = new ShaderProgram(this, vs, fs, vsByteCode, fsByteCode);
		vsByteCode = nullptr;
		fsByteCode = nullptr;
		vs = nullptr;
		fs = nullptr;
		return program;
	}

	return nullptr;
}

ComputeShaderProgram* RHIDevice::CreateComputeShaderFromHLSLFile(char const* filename)
{
	size_t srcSize = 0U;
	void *srcCode = FileReadToBuffer(filename, &srcSize);
	if (srcCode == nullptr)
	{
		ASSERT_OR_DIE(false, Stringf("File not found. %s", filename));
		return nullptr;
	}

	ID3DBlob *csByteCode = nullptr;

	csByteCode = CompileHLSLToShaderBlob(filename, srcCode, srcSize, "Main", "cs_5_0");
	free(srcCode);

	if (csByteCode != nullptr)
	{
		ID3D11ComputeShader *cs = nullptr;

		m_dxDevice->CreateComputeShader(csByteCode->GetBufferPointer(),
			csByteCode->GetBufferSize(),
			nullptr,
			&cs);

		ComputeShaderProgram *program = new ComputeShaderProgram(this, cs, csByteCode);
		csByteCode = nullptr;
		cs = nullptr;
		return program;
	}

	return nullptr;
}

VertexBuffer* RHIDevice::CreateVertexBuffer(Vertex *vertices, unsigned int vertex_count)
{
	VertexBuffer *buffer = new VertexBuffer(this, vertices, vertex_count);
	return buffer;
}

VertexBuffer* RHIDevice::CreateVertexBuffer(std::vector<Vertex> const &vertices)
{
	VertexBuffer *buffer = new VertexBuffer(this, vertices);
	return buffer;
}

VertexBuffer* RHIDevice::CreateVertexBuffer(std::vector<Vertex> const &vertices, int startIndex, int count)
{
	VertexBuffer *buffer = new VertexBuffer(this, vertices, startIndex, count);
	return buffer;
}

IndexBuffer* RHIDevice::CreateIndexBuffer(DWORD *indices, unsigned int indexCount)
{
	IndexBuffer *buffer = new IndexBuffer(this, indices, indexCount);
	return buffer;
}

IndexBuffer* RHIDevice::CreateIndexBuffer(std::vector<DWORD> const &indices)
{
	IndexBuffer *buffer = new IndexBuffer(this, indices);
	return buffer;
}

// IndexBuffer* RHIDevice::CreateIndexBuffer(std::vector<unsigned long> const &indices)
// {
// 	IndexBuffer *buffer = new IndexBuffer(this, indices);
// 	return buffer;
// }

IndexBuffer* RHIDevice::CreateIndexBuffer(std::vector<DWORD> const &indices, int startIndex, int count)
{
	IndexBuffer *buffer = new IndexBuffer(this, indices, startIndex, count);
	return buffer;
}

void RHIDevice::D3D11Cleanup()
{
	DX_SAFE_RELEASE(m_dxSwapChain);
	DX_SAFE_RELEASE(m_dxContext);
	DX_SAFE_RELEASE(m_dxDevice);

	m_instanceOwner = nullptr;
	m_immediateContext = nullptr;
}

static ID3DBlob* CompileHLSLToShaderBlob( char const *optFilename, void const *sourceCode, size_t const sourceCodeSize, char const *entrypoint, char const* target  ) 
{

	DWORD compileFlags = 0U;
#if defined(DEBUG_SHADERS)
	compileFlags |= D3DCOMPILE_DEBUG;
	compileFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
	compileFlags |= D3DCOMPILE_WARNINGS_ARE_ERRORS; 
#else 
	compileFlags |= D3DCOMPILE_OPTIMIZATION_LEVEL3; 
#endif

	ID3DBlob *code = nullptr;
	ID3DBlob *errors = nullptr;

	HRESULT hr = ::D3DCompile( sourceCode, 
		sourceCodeSize,                   
		optFilename,                       
		nullptr,                            
		D3D_COMPILE_STANDARD_FILE_INCLUDE,  
		entrypoint,                         
		target,                             
		compileFlags,                      
		0,                                  
		&code,                              
		&errors );                          

	if (FAILED(hr) || (errors != nullptr)) {
		if (errors != nullptr) {
			char *errorString = (char*) errors->GetBufferPointer();
			DebuggerPrintf("Failed to compile [%s].", optFilename);
			DebuggerPrintf("Compiler gave the following output:");
			DebuggerPrintf("%s", errorString);
			DX_SAFE_RELEASE(errors);
		}
	} 

	errors = nullptr;

	return code;
}