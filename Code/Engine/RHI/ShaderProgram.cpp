#include "Engine/RHI/ShaderProgram.hpp"
#include "Engine/Math/Vertex.hpp"
#include "Engine/RHI/RHIDevice.hpp"
#include "Engine/Core/FileUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

static ID3DBlob* CompileHLSLToShaderBlob(char const *optFilename, void const *sourceCode, size_t const sourceCodeSize, char const *entrypoint, char const* target);

ShaderProgram::ShaderProgram(RHIDevice *owner, ID3D11VertexShader *vs, ID3D11PixelShader *fs, ID3DBlob *vsBytecode, ID3DBlob *fsBytecode)
	: m_device(owner)
	, m_dxVertexShader(vs)
	, m_dxFragmentShader(fs)
	, m_vsByteCode(vsBytecode)
	, m_fsByteCode(fsBytecode)
{
	m_dxInputLayout = nullptr;
	CreateInputLayout();
}

ShaderProgram::~ShaderProgram()
{
	Destroy();
}

void ShaderProgram::CreateInputLayout()
{
	D3D11_INPUT_ELEMENT_DESC desc[8];
	memset(desc, 0, sizeof(desc));

	desc[0].SemanticName = "POSITION";
	desc[0].SemanticIndex = 0;
	desc[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	desc[0].InputSlot = 0U;
	desc[0].AlignedByteOffset = offsetof(Vertex, position);
	desc[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	desc[0].InstanceDataStepRate = 0U;

	desc[1].SemanticName = "NORMAL";
	desc[1].SemanticIndex = 0;
	desc[1].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	desc[1].InputSlot = 0U;
	desc[1].AlignedByteOffset = offsetof(Vertex, normal);
	desc[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	desc[1].InstanceDataStepRate = 0U;

	desc[2].SemanticName = "UV";
	desc[2].SemanticIndex = 0;
	desc[2].Format = DXGI_FORMAT_R32G32_FLOAT;
	desc[2].InputSlot = 0U;
	desc[2].AlignedByteOffset = offsetof(Vertex, uv);
	desc[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	desc[2].InstanceDataStepRate = 0U;

	desc[3].SemanticName = "COLOR";
	desc[3].SemanticIndex = 0;
	desc[3].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	desc[3].InputSlot = 0U;
	desc[3].AlignedByteOffset = offsetof(Vertex, color);
	desc[3].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	desc[3].InstanceDataStepRate = 0U;

	desc[4].SemanticName = "TANGENT";
	desc[4].SemanticIndex = 0;
	desc[4].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	desc[4].InputSlot = 0U;
	desc[4].AlignedByteOffset = offsetof(Vertex, tangent);
	desc[4].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	desc[4].InstanceDataStepRate = 0U;

	desc[5].SemanticName = "BONE_INDICES";
	desc[5].SemanticIndex = 0;
	desc[5].Format = DXGI_FORMAT_R32G32B32A32_UINT;
	desc[5].InputSlot = 0U;
	desc[5].AlignedByteOffset = offsetof(Vertex, boneIndices);
	desc[5].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	desc[5].InstanceDataStepRate = 0U;

	desc[6].SemanticName = "BONE_WEIGHTS";
	desc[6].SemanticIndex = 0;
	desc[6].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	desc[6].InputSlot = 0U;
	desc[6].AlignedByteOffset = offsetof(Vertex, boneWeights);
	desc[6].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	desc[6].InstanceDataStepRate = 0U;

	desc[7].SemanticName = "INDEX";
	desc[7].SemanticIndex = 0;
	desc[7].Format = DXGI_FORMAT_R32_UINT;
	desc[7].InputSlot = 0U;
	desc[7].AlignedByteOffset = offsetof(Vertex, index);
	desc[7].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	desc[7].InstanceDataStepRate = 0U;

	m_device->m_dxDevice->CreateInputLayout(desc,
		ARRAYSIZE(desc),
		m_vsByteCode->GetBufferPointer(),
		m_vsByteCode->GetBufferSize(),
		&m_dxInputLayout);
}

bool ShaderProgram::LoadFromFile(char const *filename)
{
	ID3D11Device *dxDevice = m_device->m_dxDevice;

	size_t srcSize = 0U;
	void *srcCode = FileReadToBuffer(filename, &srcSize);
	if (srcCode == nullptr) {
		// ASSERT_OR_DIE( 0, "File not found. %s", filename );
		return nullptr;
	}

	ID3DBlob *vsCode = nullptr;
	ID3DBlob *fsCode = nullptr;
	ID3D11VertexShader *vs = nullptr;
	ID3D11PixelShader *fs = nullptr;

	vsCode = CompileHLSLToShaderBlob(filename, srcCode, srcSize, "VertexFunction", "vs_5_0");
	fsCode = CompileHLSLToShaderBlob(filename, srcCode, srcSize, "FragmentFunction", "ps_5_0");
	delete srcCode;

	if ((vsCode != nullptr)
		&& (fsCode != nullptr)) {


		dxDevice->CreateVertexShader(vsCode->GetBufferPointer(),
			vsCode->GetBufferSize(),
			nullptr,
			&vs);

		dxDevice->CreatePixelShader(fsCode->GetBufferPointer(),
			fsCode->GetBufferSize(),
			nullptr,
			&fs);

		if ((vs != nullptr) && (fs != nullptr)) {
			Destroy();

			m_vsByteCode = vsCode;
			m_fsByteCode = fsCode;
			m_dxVertexShader = vs;
			m_dxFragmentShader = fs;
			CreateInputLayout();

			return true;
		}
	}

	DX_SAFE_RELEASE(vsCode);
	DX_SAFE_RELEASE(fsCode);
	DX_SAFE_RELEASE(vs);
	DX_SAFE_RELEASE(fs);

	return false;
}

void ShaderProgram::Destroy()
{
	DX_SAFE_RELEASE(m_dxVertexShader);
	DX_SAFE_RELEASE(m_dxFragmentShader);
	DX_SAFE_RELEASE(m_dxInputLayout);
	DX_SAFE_RELEASE(m_vsByteCode);
	DX_SAFE_RELEASE(m_fsByteCode);
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

	return code;
}