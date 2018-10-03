#pragma once
#if !defined( __RHI_COMPUTESAHDER__ )
#define __RHI_COMPUTESAHDER__

#include "Engine/RHI/DX11.hpp"

class RHIDevice;

class ComputeShaderProgram
{
public:
	RHIDevice *m_device;
	ID3DBlob *m_csByteCode;
	ID3D11InputLayout *m_dxInputLayout;
	ID3D11ComputeShader *m_dxComputeShader;


	ComputeShaderProgram(RHIDevice *owner, ID3D11ComputeShader *cs, ID3DBlob *csBytecode);
	~ComputeShaderProgram();

	void CreateInputLayout();
	bool LoadFromFile(char const *filename);
	void Destroy();

	inline bool IsValid() const { return m_dxComputeShader != nullptr; }
};

#endif 
