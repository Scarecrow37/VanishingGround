#pragma once
#include "IBash.h"

namespace GE
{	
	class ISkyBoxRenderer : public IBase
	{
	protected:
		explicit ISkyBoxRenderer() = default;
		virtual ~ISkyBoxRenderer() = default;
		ISkyBoxRenderer(const ISkyBoxRenderer&) = delete;
		ISkyBoxRenderer(ISkyBoxRenderer&&) = delete;
		ISkyBoxRenderer& operator=(const ISkyBoxRenderer&) = delete;
		ISkyBoxRenderer& operator=(ISkyBoxRenderer&&) = delete;

	public:
		virtual void SetSkyBoxTexture(const wchar_t* filePath) = 0;
		virtual void SetBRDFTexture(const wchar_t* filePath) = 0;
		virtual void SetDiffuseTexture(const wchar_t* filePath) = 0;
		virtual void SetSpecularTexture(const wchar_t* filePath) = 0;
	};
}