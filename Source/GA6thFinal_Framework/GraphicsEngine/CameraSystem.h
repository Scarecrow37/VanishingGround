#pragma once
#include "includes/ICameraSystem.h"

class Camera;
class CameraSystem : public GE::ICameraSystem
{
public:
	explicit CameraSystem() = default;
	virtual ~CameraSystem() = default;

public:
	Camera* GetCurrentCamera() const { return _pCurrentCamera; }
	Camera* GetShadowCaemra()const { return _pShadowCamera; }
	void SetCurrentCamera(const wchar_t* name) override;
	void SetShadowCamera(const wchar_t* name) override;

public:
	// ICameraSystem을(를) 통해 상속됨
	void Release() override;
	void CreateCamera(GE::ICamera** ppCamera) override;
	void RegisterCamera(const wchar_t* name, GE::ICamera* pCaemra) override;
	void UnRegisterCamera(const wchar_t* name) override;

public:
	void Update() override;

private:
	std::unordered_map<std::wstring, Camera*> _cameras;
	Camera* _pCurrentCamera{ nullptr };
	Camera* _pShadowCamera{ nullptr };
};