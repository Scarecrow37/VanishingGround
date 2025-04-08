#pragma once
#include "IBash.h"

namespace GE
{	
	class ITextRenderer : public IBase
	{
	protected:
		explicit ITextRenderer() = default;
		virtual ~ITextRenderer() = default;
		ITextRenderer(const ITextRenderer&) = delete;
		ITextRenderer(ITextRenderer&&) = delete;
		ITextRenderer& operator=(const ITextRenderer&) = delete;
		ITextRenderer& operator=(ITextRenderer&&) = delete;

	public:
		virtual void SetFont(const wchar_t* font) = 0;
		virtual void SetFontScale(const float x, const float y) = 0;
		virtual void SetFontColor(const float r, const float g, const float b, const float a) = 0;
		virtual void SetLayerDepth(const float depth) = 0;
		virtual void SetPivot(const float x, const float y) = 0;
		virtual void SetRotationFromDegree(const float degree) = 0;
		virtual void SetPosition(const float x, const float y, const float z) = 0;
		virtual void SetText(const wchar_t* text) = 0;
	};
}