#include "pch.h"
#include "TempObject.h"
#include "BaseMesh.h"

void TempObject::Initialize(std::unique_ptr<BaseMesh>& mesh, const Vector3& position)
{
	_geometry = std::move(mesh);
	_position = position;
}

void TempObject::AddTexture(std::wstring_view filePath, UINT index)
{
	_texture[index] = UmResourceManager.LoadResource<Texture>(filePath.data());
}

void TempObject::Update(const float deltaTime)
{
	XMVECTOR quaternion = XMQuaternionRotationRollPitchYaw(0.f, 0.f, 0.f);

	XMMATRIX scale = XMMatrixScaling(1.f, 1.f, 1.f);
	XMMATRIX rotation = XMMatrixRotationQuaternion(quaternion);
	XMMATRIX translate = XMMatrixTranslationFromVector(_position);

	_objectData.World = scale * rotation * translate;
}

void TempObject::Render(ID3D12GraphicsCommandList* commandList)
{
	_geometry->Render(commandList);
}
