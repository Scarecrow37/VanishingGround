#pragma once
#include "IBash.h"

namespace GE
{
	class IUnlitRenderer : public IBase
	{
	protected:
		explicit IUnlitRenderer() = default;
		virtual ~IUnlitRenderer() = default;
		IUnlitRenderer(const IUnlitRenderer&) = delete;
		IUnlitRenderer(IUnlitRenderer&&) = delete;
		IUnlitRenderer& operator=(const IUnlitRenderer&) = delete;
		IUnlitRenderer& operator=(IUnlitRenderer&&) = delete;

	public:
		virtual void SetPostEffectLayer(unsigned int layer) = 0;
	};
}