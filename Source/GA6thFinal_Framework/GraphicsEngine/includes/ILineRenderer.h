#pragma once
#include "IUnlitRenderer.h"

namespace GE
{
	class ILineRenderer : public IUnlitRenderer
	{
	protected:
		explicit ILineRenderer() = default;
		virtual ~ILineRenderer() = default;
		ILineRenderer(const ILineRenderer&) = delete;
		ILineRenderer(ILineRenderer&&) = delete;
		ILineRenderer& operator=(const ILineRenderer&) = delete;
		ILineRenderer& operator=(ILineRenderer&&) = delete;

	public:
		virtual void SetSourcePosition(float* pArray) = 0;
		virtual void SetDestinationPosition(float* pArray) = 0;
		virtual void SetSegment(unsigned int segment) = 0;
		virtual void SetBaseTexture(const wchar_t* filePath) = 0;
		virtual void SetMaskTexture(const wchar_t* filePath) = 0;
		virtual void SetGradientTexture(const wchar_t* filePath) = 0;
		virtual void Update(float deltaTime) = 0;
	};
}