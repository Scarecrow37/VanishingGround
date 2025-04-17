#pragma once

class BaseMesh;
class TempObject
{
public:
	const ObjectData& GetObjectData() const { return _objectData; }
	D3D12_CPU_DESCRIPTOR_HANDLE GetShaderResourceCPU(UINT index) { return _texture[index]->GetHandle(); }

public:
	void Initialize(std::unique_ptr<BaseMesh>& mesh, const Vector3& position);
	void AddTexture(std::wstring_view filePath, UINT index);
	void Update(const float deltaTime);
	void Render(ID3D12GraphicsCommandList* commandList);

private:
	ObjectData					_objectData;
	std::shared_ptr<Texture>	_texture[4];
	std::unique_ptr<BaseMesh>	_geometry;

	Vector3						_position;
	Vector3						_rotation;
};