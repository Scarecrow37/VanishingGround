#pragma once
#include "ResourceBase.h"

class ConstantBufferView : public ResourceBase
{
public:    
    ConstantBufferView();
    virtual ~ConstantBufferView();

public:
    void Initialize(UINT size);
    void UpdateBuffer(void* data);
    void UpdateBufferWithOffset(void* data, size_t offset, size_t dataSize);

private:
    void* _data{nullptr};
    UINT  _size{0};
};