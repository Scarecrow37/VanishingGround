#pragma once
#include "IBash.h"

namespace GE
{
	class ILight;
	class ILightSystem : public IBase
	{
	protected:
		explicit ILightSystem() = default;
		virtual ~ILightSystem() = default;
		ILightSystem(const ILightSystem&) = delete;
		ILightSystem(ILightSystem&&) = delete;
		ILightSystem& operator=(const ILightSystem&) = delete;
		ILightSystem& operator=(ILightSystem&&) = delete;

	public:
		virtual void CreateLight(ILight** ppOutLight) = 0;
		virtual void RegisterLight(ILight* pLight) = 0;
		virtual void UnRegisterLight(ILight* pLight) = 0;
		virtual void SetMainLight(ILight* pLight) = 0;
	};
}