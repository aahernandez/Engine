#include "Engine/RHI/StructuredBuffer.hpp"
#include "Engine/RHI/RHIDevice.hpp"
#include "Engine/RHI/RHIDeviceContext.hpp"

StructuredBuffer::StructuredBuffer(RHIDevice *owner, void const *buffer, unsigned int objectSize, unsigned int objectCount)
{
	dxSRV = nullptr;
	buffer_size = objectSize * objectCount;
	obj_count = objectCount;
	obj_size = objectSize;

	D3D11_BUFFER_DESC vb_desc;
	memset(&vb_desc, 0, sizeof(vb_desc));

	vb_desc.ByteWidth = (unsigned int)buffer_size;      
	vb_desc.Usage = D3D11_USAGE_DYNAMIC;                
														
														
	vb_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	vb_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;     
	vb_desc.StructureByteStride = objectSize;           
	vb_desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;

	D3D11_SUBRESOURCE_DATA initial_data;
	memset(&initial_data, 0, sizeof(initial_data));
	initial_data.pSysMem = buffer;

	dxBuffer = nullptr;
	owner->m_dxDevice->CreateBuffer(&vb_desc, &initial_data, &dxBuffer);
	CreateViews(owner);
}

StructuredBuffer::~StructuredBuffer()
{
	DX_SAFE_RELEASE(dxBuffer);
	DX_SAFE_RELEASE(dxSRV);
}

bool StructuredBuffer::CreateViews(RHIDevice *owner)
{
	D3D11_SHADER_RESOURCE_VIEW_DESC desc;
	MemZero(&desc);

	desc.Format = DXGI_FORMAT_UNKNOWN;
	desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	desc.Buffer.ElementOffset = 0U;
	desc.Buffer.NumElements = obj_count;

	owner->m_dxDevice->CreateShaderResourceView(dxBuffer, &desc, &dxSRV);

	return true;
}

bool StructuredBuffer::Update(RHIDeviceContext *context, void const *buffer)
{
	D3D11_MAPPED_SUBRESOURCE resource;

	ID3D11DeviceContext *dx_context = context->m_dxContext;
	if (SUCCEEDED(dx_context->Map(dxBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0U, &resource))) {
		memcpy(resource.pData, buffer, buffer_size);
		dx_context->Unmap(dxBuffer, 0);

		return true;
	}

	return false;
}

