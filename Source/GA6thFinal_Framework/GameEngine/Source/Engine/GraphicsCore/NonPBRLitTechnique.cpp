#include "pch.h"
#include "NonPBRLitTechnique.h"

NonPBRLitTechnique::NonPBRLitTechnique() {}

NonPBRLitTechnique::~NonPBRLitTechnique() {}

void NonPBRLitTechnique::Initialize(ID3D12GraphicsCommandList* commandList) {}

void NonPBRLitTechnique::Execute(ID3D12GraphicsCommandList* commandList)
{
    __super::Execute(commandList);
}