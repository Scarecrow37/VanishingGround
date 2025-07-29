#pragma once

using RenderPassDataPair = std::pair<std::any, std::unordered_map<std::string, std::vector<D3D12_GPU_DESCRIPTOR_HANDLE>>>;
using RenderPassProperties = std::unordered_map<std::string, std::unordered_map<std::string, RenderPassDataPair>>;

class RenderPassDatas
{
public:
    RenderPassDatas()  = default;
    ~RenderPassDatas() = default;

public:
    RenderPassProperties&   GetRenderPassProperties() { return _renderPassProperties; }
    const std::any&         GetRenderPassProperty(std::string_view sceneName, std::string_view passName) const;

public:
    void AddRenderPassDatas(std::string_view sceneName, std::string_view passName, std::string_view dataName, D3D12_GPU_DESCRIPTOR_HANDLE handle)
    {
        auto& [property, handles] = _renderPassProperties[sceneName.data()][passName.data()];
        handles[dataName.data()].push_back(handle);
    }

    void AddRenderPassProperty(std::string_view sceneName, std::string_view passName, std::any value)
    {
        auto& [property, handles] = _renderPassProperties[sceneName.data()][passName.data()];
        property                  = value;
    }

private:
    RenderPassProperties _renderPassProperties;
};