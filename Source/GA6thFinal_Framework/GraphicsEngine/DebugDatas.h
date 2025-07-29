#pragma once

using SRVDatas = std::unordered_map<std::string, std::vector<D3D12_GPU_DESCRIPTOR_HANDLE>>;
using PassDatas = std::unordered_map<std::string, SRVDatas>;

class DebugDatas
{
public:
    DebugDatas()  = default;
    ~DebugDatas() = default;

public:
    const std::unordered_map<std::string, PassDatas>& GetDebugDataImages() const { return _debugDatas; } 

public:
    void AddDebugData(std::string_view sceneName, std::string_view passName, std::string_view dataName, D3D12_GPU_DESCRIPTOR_HANDLE handle)
    {
        _debugDatas[sceneName.data()][passName.data()][dataName.data()].push_back(handle);
    }

private:
    std::unordered_map<std::string, PassDatas> _debugDatas;
};