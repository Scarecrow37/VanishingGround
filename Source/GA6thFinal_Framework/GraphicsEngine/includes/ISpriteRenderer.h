#pragma once
#include "IBash.h"

namespace GE
{
	struct SpriteDescription
	{
		enum Type { D2D, D3D, D3D_Billboard };
		Type	type;
		float	alpha;
		bool	activeDraw;
	};

	class ISpriteRenderer : public IBase
	{
	protected:
		explicit ISpriteRenderer() = default;
		virtual ~ISpriteRenderer() = default;
		ISpriteRenderer(const ISpriteRenderer&) = delete;
		ISpriteRenderer(ISpriteRenderer&&) = delete;
		ISpriteRenderer& operator=(const ISpriteRenderer&) = delete;
		ISpriteRenderer& operator=(ISpriteRenderer&&) = delete;

	public:
		virtual void GetDesc(SpriteDescription* out) = 0;
		virtual void SetDesc(const SpriteDescription* in) = 0;
		virtual void GetImageSize(float* outWidth, float* outHeight) = 0;
	};
}