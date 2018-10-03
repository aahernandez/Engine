#include "Engine/RHI/RHIDeviceContext.hpp"
#include "Engine/RHI/RasterState.hpp"
#include "Engine/RHI/BlendState.hpp"
#include "Engine/RHI/DepthStencilState.hpp"
#include "Engine/RHI/ShaderProgram.hpp"
#include "Engine/RHI/Texture2D.hpp"
#include "Engine/RHI/Sampler.hpp"
#include "Engine/RHI/ConstantBuffer.hpp"
#include "Engine/RHI/VertexBuffer.hpp"
#include "Engine/RHI/IndexBuffer.hpp"
#include "Engine/RHI/StructuredBuffer.hpp"
#include "Engine/RHI/ComputeShaderProgram.hpp"
#include "Engine/RHI/ComputeJob.hpp"

RHIDeviceContext::RHIDeviceContext(RHIDevice *owner, ID3D11DeviceContext *ctx)
{
	m_device = owner;
	m_dxContext = ctx;
}

RHIDeviceContext::~RHIDeviceContext()
{
	Destroy();
}

// void RHIDeviceContext::Draw(e_PrimitiveType topology, VertexBuffer *vbo)
// {
// 	D3D11_PRIMITIVE_TOPOLOGY d3dPrim;
// 	switch (topology) {
// 	case PRIMITIVE_LINES:
// 		d3dPrim = D3D11_PRIMITIVE_TOPOLOGY_LINELIST;
// 		break;
// 	case PRIMITIVE_TRIANGLES:
// 	default:
// 		d3dPrim = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
// 	}
// 
// 	m_dxContext->IASetPrimitiveTopology(d3dPrim);
// 	unsigned int stride = sizeof(Vertex);
// 	unsigned int offsets = 0;
// 	m_dxContext->IASetVertexBuffers(0, 1, &vbo->m_dxBuffer, &stride, &offsets);
// 
// 	m_dxContext->Draw(vbo->m_vertexCount, 0);
// }

void RHIDeviceContext::Draw(e_PrimitiveType topology, VertexBuffer *vbo, IndexBuffer *ibo)
{
	D3D11_PRIMITIVE_TOPOLOGY d3dPrim;
	switch (topology) {
	case PRIMITIVE_LINES:
		d3dPrim = D3D11_PRIMITIVE_TOPOLOGY_LINELIST;
		break;
	case PRIMITIVE_TRIANGLES:
	default:
		d3dPrim = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	}

	m_dxContext->IASetPrimitiveTopology(d3dPrim);
	unsigned int stride = sizeof(Vertex);
	unsigned int offsets = 0;
	m_dxContext->IASetVertexBuffers(0, 1, &vbo->m_dxBuffer, &stride, &offsets);
	m_dxContext->IASetIndexBuffer( ibo->m_dxBuffer, DXGI_FORMAT_R32_UINT, 0);

	m_dxContext->DrawIndexed(ibo->m_indexCount, 0, 0);
}

void RHIDeviceContext::ClearState()
{

}

void RHIDeviceContext::Flush()
{

}

void RHIDeviceContext::ClearColorTarget(Texture2D *output, RGBA const &color)
{
	float clearColor[4] = { 0.004f, 0.475f, 0.435f, 1.0f };
	color.GetAsFloats(clearColor[0], clearColor[1], clearColor[2], clearColor[3]);
	m_dxContext->ClearRenderTargetView(output->m_dxRTV, clearColor);
}

void RHIDeviceContext::ClearDepthTarget(Texture2D *output, float depth, unsigned char stencil)
{
	if (output != nullptr)
	{
		m_dxContext->ClearDepthStencilView(output->m_dxDSV,
			D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
			depth, 
			stencil);
	}
}

void RHIDeviceContext::SetColorTarget(Texture2D *target, Texture2D *depthStencilTarget /*= nullptr*/)
{
	if (target->IsRenderTarget())
	{
		m_dxContext->OMSetRenderTargets(1, &target->m_dxRTV,
			depthStencilTarget != nullptr ? depthStencilTarget->m_dxDSV : nullptr);
	}
}

void RHIDeviceContext::SetRasterState(RasterState *r)
{
	m_dxContext->RSSetState(r->m_dxState);
}

void RHIDeviceContext::SetViewport(unsigned int x, unsigned int y, unsigned int width, unsigned int height)
{
	D3D11_VIEWPORT viewport;
	memset(&viewport, 0, sizeof(viewport));
	viewport.TopLeftX = (FLOAT)x;
	viewport.TopLeftY = (FLOAT)y;
	viewport.Width = (FLOAT)width;
	viewport.Height = (FLOAT)height;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

	m_dxContext->RSSetViewports(1, &viewport);
}

void RHIDeviceContext::SetShader(ShaderProgram *shader)
{
	m_dxContext->VSSetShader(shader->m_dxVertexShader, nullptr, 0U);
	m_dxContext->PSSetShader(shader->m_dxFragmentShader, nullptr, 0U);
	m_dxContext->IASetInputLayout(shader->m_dxInputLayout);
}

void RHIDeviceContext::SetComputeShader(ComputeShaderProgram *shader)
{
	m_dxContext->CSSetShader(shader->m_dxComputeShader, nullptr, 0U);
	m_dxContext->IASetInputLayout(shader->m_dxInputLayout);
}

void RHIDeviceContext::SetTexture(unsigned int textureIndex, Texture2D *texture)
{
	m_dxContext->VSSetShaderResources(textureIndex, 1, &texture->m_dxSRV);
	m_dxContext->PSSetShaderResources(textureIndex, 1, &texture->m_dxSRV);
}

void RHIDeviceContext::BindUAV(unsigned int textureIndex, Texture2D *texture)
{
	m_dxContext->CSSetUnorderedAccessViews(textureIndex, 1, &texture->m_dxUAV, nullptr);
}

void RHIDeviceContext::UnbindUAV()
{
	ID3D11UnorderedAccessView* UAV_NULL = NULL;
	m_dxContext->CSSetUnorderedAccessViews(0, 1, &UAV_NULL, 0);
}

void RHIDeviceContext::SetSampler(unsigned int samplerIndex, Sampler *sampler)
{
	m_dxContext->VSSetSamplers(samplerIndex, 1, &sampler->m_dxSampler);
	m_dxContext->PSSetSamplers(samplerIndex, 1, &sampler->m_dxSampler);
}

void RHIDeviceContext::SetConstantBuffer(unsigned int index, ConstantBuffer *constantBuffer)
{
	m_dxContext->VSSetConstantBuffers(index, 1, &constantBuffer->m_dxBuffer);
	m_dxContext->PSSetConstantBuffers(index, 1, &constantBuffer->m_dxBuffer);
}

void RHIDeviceContext::SetComputeConstantBuffer(unsigned int index, ConstantBuffer *constantBuffer)
{
	m_dxContext->CSSetConstantBuffers(index, 1, &constantBuffer->m_dxBuffer);
}

void RHIDeviceContext::SetStructuredBuffer(unsigned int bindPoint, StructuredBuffer *buffer)
{
	m_dxContext->VSSetShaderResources(bindPoint, 1, &buffer->dxSRV);
	m_dxContext->PSSetShaderResources(bindPoint, 1, &buffer->dxSRV);
}

void RHIDeviceContext::DispatchComputeJob(ComputeJob *job)
{
	SetComputeShader(job->m_computeShader);
	BindUAV(job->m_textureIndex, job->m_uavTexture);
	m_dxContext->Dispatch(job->m_gridDimensions.x, job->m_gridDimensions.y, job->m_gridDimensions.z);
	UnbindUAV();
}

void RHIDeviceContext::SetDepthStencilState(DepthStencilState *dss)
{
	m_dxContext->OMSetDepthStencilState(dss->dxState, 0U);
}

void RHIDeviceContext::Destroy()
{
	if (m_dxContext != nullptr)
		m_dxContext->Release();
	m_dxContext = nullptr;

	m_device = nullptr;
}

void RHIDeviceContext::SetBlendState(BlendState *bs)
{
	float constant[] = {1.f, 1.f, 1.f, 1.f};
	m_dxContext->OMSetBlendState(bs->m_dxState, constant, 0xffffffff);
	bs = nullptr;
}
