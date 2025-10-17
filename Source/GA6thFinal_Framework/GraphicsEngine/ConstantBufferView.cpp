#include "pch.h"
#include "ConstantBufferView.h"

ConstantBufferView::ConstantBufferView()
{
}

ConstantBufferView::~ConstantBufferView()
{
    _resource->Unmap(0, nullptr);
}

void ConstantBufferView::Initialize(UINT size)
{
    D3D12_HEAP_PROPERTIES hp{
        .Type                 = D3D12_HEAP_TYPE_UPLOAD,
        .CPUPageProperty      = D3D12_CPU_PAGE_PROPERTY_UNKNOWN,
        .MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN,
        .CreationNodeMask     = 0,
        .VisibleNodeMask      = 0,
    };

    D3D12_RESOURCE_DESC rd{
        .Dimension        = D3D12_RESOURCE_DIMENSION_BUFFER,
        .Alignment        = 0,
        .Width            = size,
        .Height           = 1,
        .DepthOrArraySize = 1,
        .MipLevels        = 1,
        .Format           = DXGI_FORMAT_UNKNOWN,
        .SampleDesc{.Count = 1, .Quality = 0},
        .Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR,
        .Flags  = D3D12_RESOURCE_FLAG_NONE,
    };

    HRESULT hr = S_OK;
    ID3D12Device* device = Global::device->GetDevice();
    hr = device->CreateCommittedResource(&hp, D3D12_HEAP_FLAG_NONE, &rd, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&_resource));
    FAILED_CHECK_MESSAGE(hr, L"ConstantBufferView::Initialize device->CreateCommittedResource Failed");

    hr = _resource->Map(0, nullptr, &_data);
    FAILED_CHECK_MESSAGE(hr, L"ConstantBufferView::Initialize _resource->Map Failed");

    _size = size;
}

void ConstantBufferView::UpdateBuffer(void* data)
{
    memcpy(_data, data, _size);
}

void ConstantBufferView::UpdateBufferWithOffset(void* data, size_t offset, size_t dataSize)
{
    if (!data || !_data)
        return;
    if (offset + dataSize > _size)
    {
        GRAPHICS_ASSERT(false, L"ConstantBufferView::UpdateBufferWithOffset : Out of bounds access.");
        return;
    }

    std::memcpy(static_cast<std::uint8_t*>(_data) + offset, data, dataSize);
}
