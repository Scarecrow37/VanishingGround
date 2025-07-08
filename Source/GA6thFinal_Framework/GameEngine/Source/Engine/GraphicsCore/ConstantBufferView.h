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

private:
    void* _data{nullptr};
    UINT  _size{0};
};