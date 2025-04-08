#pragma once

class ImGuiDX11Module : public IAppModule
{
    inline static Microsoft::WRL::ComPtr<ID3D11Device> device;
    inline static Microsoft::WRL::ComPtr<ID3D11DeviceContext> device_context;
public:
    static void SetDevice(ID3D11Device* device);

    ImGuiDX11Module() = default;
    virtual ~ImGuiDX11Module() = default;

    virtual void PreInitialize();
    virtual void ModuleInitialize();

    virtual void PreUnInitialize();
    virtual void ModuleUnInitialize();

    void ImguiBegin();
    void ImguiEnd();
};