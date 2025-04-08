#include "pch.h"
#include "Material.h"
#include "Texture.h"
#include "VertexShader.h"
#include "PixelShader.h"

Material::Material()
{
	_textureMaps.resize((size_t)TextureType::End);
}

void Material::SetParameters(std::shared_ptr<PixelShader>& pixelShader)
{
	for (size_t i = 0; i < _textureMaps.size(); i++)
	{
		ID3D11ShaderResourceView* pTexture = nullptr;

		if (_textureMaps[i].get())
			pTexture = *_textureMaps[i]->Get();

		pixelShader->SetShaderResource((TextureType)i, pTexture);
	}
}

void Material::BindTextureMap(TextureType type, const std::shared_ptr<Texture>& texture)
{
	_textureMaps[(int)type] = texture;

	if (type == TextureType::Opacity && texture != nullptr)
		_isAlpha = true;
}

void Material::Free()
{
}
