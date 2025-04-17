#pragma once

class ImGuiDX12Module : public IAppModule
{
    inline static ComPtr<ID3D12Device> device;

public:
    static void SetDevice(ImGuiDX12Module* device);

    ImGuiDX12Module()          = default;
    virtual ~ImGuiDX12Module() = default;

    virtual void PreInitialize();
    virtual void ModuleInitialize();

    virtual void PreUnInitialize();
    virtual void ModuleUnInitialize();

    void ImguiBegin();
    void ImguiEnd();

private:
    ComPtr<ID3D12DescriptorHeap> _imguiDescriptorHeap = nullptr;
};