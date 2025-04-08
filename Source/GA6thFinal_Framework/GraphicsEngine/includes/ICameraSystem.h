#pragma once
#include "IBash.h"

namespace GE
{
    class ICamera;
    class ICameraSystem : public IBase
    {
    protected:
        explicit ICameraSystem() = default;
        virtual ~ICameraSystem() = default;
        ICameraSystem(const ICameraSystem&) = delete;
        ICameraSystem(ICameraSystem&&) = delete;
        ICameraSystem& operator=(const ICameraSystem&) = delete;
        ICameraSystem& operator=(ICameraSystem&&) = delete;

    public:
        virtual void SetCurrentCamera(const wchar_t* name) = 0;
        virtual void SetShadowCamera(const wchar_t* name) = 0;
        virtual void CreateCamera(ICamera** ppCamera) = 0;
        virtual void RegisterCamera(const wchar_t* name, ICamera* pCaemra) = 0;
        virtual void UnRegisterCamera(const wchar_t* name) = 0;
        virtual void Update() = 0;
    };
}