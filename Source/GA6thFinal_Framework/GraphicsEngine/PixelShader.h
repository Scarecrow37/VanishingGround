#pragma once
#include "Shader.h"

class Sampler;
class PixelShader : public Shader
{
	enum class Type { CBuffer, Sampler };
	struct Slot
	{
		unsigned int start;
		unsigned int count;
	};

	struct SharedSlot
	{
		std::wstring name;
		Slot slot;
		Type type;
	};

public:
	explicit PixelShader() = default;
	virtual ~PixelShader();

public:
	ID3D11PixelShader* Get() const { return _pPixelShader; }

public:
	void SetPixelShader();
	void SetShaderResource(TextureType type, ID3D11ShaderResourceView* pShaderResourece);

	// Resource을(를) 통해 상속됨
	HRESULT LoadResource(const std::filesystem::path& filePath) override;

private:
	void Free() override;

private:
	std::vector<Slot>			_inputBindSlot;
	std::vector<SharedSlot>		_sharedBindSlot;
	std::shared_ptr<Sampler>	_sampler;
	ID3D11PixelShader*			_pPixelShader{ nullptr };
};