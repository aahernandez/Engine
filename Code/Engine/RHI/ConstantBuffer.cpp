#include "Engine/RHI/ConstantBuffer.hpp"
#include "Engine/RHI/RHIDevice.hpp"
#include "Engine/RHI/RHIDeviceContext.hpp"

// #define MATRIX_BUFFER_

ConstantBuffer::ConstantBuffer(RHIDevice *owner, void const *buffer, size_t const size)
{
	m_bufferSize = size;

	D3D11_BUFFER_DESC desc;
	memset(&desc, 0, sizeof(desc));

	desc.ByteWidth = (unsigned int)m_bufferSize;
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA initialData;
	memset(&initialData, 0, sizeof(initialData));
	initialData.pSysMem = buffer;

	m_dxBuffer = nullptr;
	owner->m_dxDevice->CreateBuffer(&desc, &initialData, &m_dxBuffer);
}

ConstantBuffer::~ConstantBuffer()
{
	Destroy();
}

bool ConstantBuffer::Update(RHIDeviceContext *context, void const *buffer)
{
	D3D11_MAPPED_SUBRESOURCE resource;

	ID3D11DeviceContext *dxContext = context->m_dxContext;
	if (SUCCEEDED(dxContext->Map(m_dxBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0U, &resource))) {
		memcpy(resource.pData, buffer, m_bufferSize);
		dxContext->Unmap(m_dxBuffer, 0);

		return true;
	}

	return false;
}

void ConstantBuffer::Destroy()
{
	if (m_dxBuffer != nullptr)
		m_dxBuffer->Release();
	m_dxBuffer = nullptr;
}
