#include "pch.h"
#include "SSRTechnique.h"

SSRTechnique::SSRTechnique() {}

SSRTechnique::~SSRTechnique() {}

void SSRTechnique::Initialize(ID3D12GraphicsCommandList* commandList) {}

void SSRTechnique::Execute(ID3D12GraphicsCommandList* commandList) 
{
    __super::Execute(commandList);
}
