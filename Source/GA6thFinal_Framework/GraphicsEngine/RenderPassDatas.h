#pragma once

using RenderPassProperties = std::unordered_map<std::string, std::any>;
using RenderPassImages = std::unordered_map<std::string, std::unordered_map<std::string, std::unordered_map<std::string, std::vector<D3D12_GPU_DESCRIPTOR_HANDLE>>>>;

class RenderPassDatas
{
public:
    RenderPassDatas()  = default;
    ~RenderPassDatas() = default;

public:
    const std::any&         GetRenderPassProperty(std::string_view passName) const { return _renderPassProperties.at(passName.data()); }
    RenderPassProperties&   GetRenderPassProperties() { return _renderPassProperties; }
    const RenderPassImages& GetRenderPassImages() const { return _renderPassImages; }

public:
    void AddRenderPassImage(std::string_view sceneName, std::string_view passName, std::string_view dataName, D3D12_GPU_DESCRIPTOR_HANDLE handle)
    {
        _renderPassImages[sceneName.data()][passName.data()][dataName.data()].push_back(handle);
    }

    void AddRenderPassProperty(std::string_view passName, std::any value)
    {
        _renderPassProperties.try_emplace(passName.data(), value);
    }

private:
    RenderPassImages     _renderPassImages;
    RenderPassProperties _renderPassProperties;
};