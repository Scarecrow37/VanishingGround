#pragma once
#include "IBash.h"

namespace GE
{
	struct MeshDescription
	{
		unsigned int postEffectFlag;
		bool activeShadow;
		bool activeDraw;
		bool zWrite;
	};

	enum class Type { Static, Skeletal, SkyBox };
	class IMeshRenderer : virtual public IBase
	{
	protected:
		explicit IMeshRenderer() = default;
		virtual ~IMeshRenderer() = default;
		IMeshRenderer(const IMeshRenderer&) = delete;
		IMeshRenderer(IMeshRenderer&&) = delete;
		IMeshRenderer& operator=(const IMeshRenderer&) = delete;
		IMeshRenderer& operator=(IMeshRenderer&&) = delete;

	public:
		virtual void Query(void** ppOut) = 0;
		virtual void GetDesc(MeshDescription* out) = 0;
		virtual void SetDesc(const MeshDescription* in) = 0;
	};
}