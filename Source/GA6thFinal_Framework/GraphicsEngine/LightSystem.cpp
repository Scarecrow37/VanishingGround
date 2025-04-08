#include "pch.h"
#include "LightSystem.h"
#include "Light.h"

std::list<Light*>& LightSystem::GetLights()
{
	return _lights;
}

Light* LightSystem::GetMainLight()
{
	// ASSERT(_pMainLight, L"Mainlight is not set.");
	return _pMainLight;
}

void LightSystem::CreateLight(GE::ILight** ppOutLight)
{
	(*ppOutLight) = new Light;
}

void LightSystem::RegisterLight(GE::ILight* pLight)
{
	auto compare = [pLight](const auto* light)
		{ return static_cast<Light*>(pLight)->GetID() == light->GetID();};

	if (std::ranges::any_of(_lights, compare))
	{
		ASSERT(false, L"Already registered light.");
		return;
	}

	_lights.push_back(static_cast<Light*>(pLight));
}

void LightSystem::UnRegisterLight(GE::ILight* pLight)
{
	std::erase_if(_lights, [pLight](const auto* light)
		{ return static_cast<Light*>(pLight)->GetID() == light->GetID(); });
}

void LightSystem::SetMainLight(GE::ILight* pLight)
{
	Light* pMainLight = static_cast<Light*>(pLight);

	if (GE::ILight::Directional != pMainLight->GetType())
	{
		ASSERT(false, L"Only directional light can be set");
		return;
	}

	_pMainLight = pMainLight;
}

void LightSystem::Release()
{
	delete this;
}