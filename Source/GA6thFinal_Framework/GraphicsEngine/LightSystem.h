#pragma once
#include "includes/ILightSystem.h"

class Light;
class LightSystem : public GE::ILightSystem
{
public:
	explicit LightSystem() = default;
	virtual ~LightSystem() = default;

public:
	std::list<Light*>& GetLights();
	Light* GetMainLight();

public:
	// ILightSystem을(를) 통해 상속됨
	void CreateLight(GE::ILight** ppOutLight) override;
	void RegisterLight(GE::ILight* pLight) override;
	void UnRegisterLight(GE::ILight* pLight) override;
	void SetMainLight(GE::ILight* pLight) override;
	void Release() override;
	
private:
	std::list<Light*> _lights;
	Light* _pMainLight{ nullptr };
};