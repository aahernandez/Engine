#pragma once
#if !defined( __RHI_SHADERPROGRAM__ )
#define __RHI_SHADERPROGRAM__

#include "Engine/RHI/DX11.hpp"

class RHIDevice;

enum e_ShaderStage
{
	SHADER_VERTEX,    
	SHADER_FRAGMENT, 
};

class ShaderProgram
{
public:
	RHIDevice *m_device;
	ID3DBlob *m_vsByteCode;
	ID3DBlob *m_fsByteCode;
	ID3D11InputLayout *m_dxInputLayout;
	ID3D11PixelShader *m_dxFragmentShader;
	ID3D11VertexShader *m_dxVertexShader;


	ShaderProgram(RHIDevice *owner,	ID3D11VertexShader *vs, ID3D11PixelShader *fs, ID3DBlob *vsBytecode, ID3DBlob *fsBytecode);
	~ShaderProgram();

	void CreateInputLayout();
	bool LoadFromFile( char const *filename );
	void Destroy();

	inline bool IsValid() const { return (m_dxVertexShader != nullptr) && (m_dxFragmentShader != nullptr); }
};

#endif 
