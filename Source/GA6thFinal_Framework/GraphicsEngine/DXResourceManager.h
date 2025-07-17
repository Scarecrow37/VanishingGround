#pragma once

class ResourceBase;
class DXResourceManager
{
public:
    DXResourceManager();
    ~DXResourceManager();

public:
    template <typename T> requires(std::is_base_of_v<ResourceBase, T>)
	void AddResource(SharedResource<T> resourceBase)
	{
        auto& typeID = typeid(T);

        auto iter = std::find_if(_resources[typeID].begin(), _resources[typeID].end(),
                                 [&resourceBase](const SharedResource<ResourceBase>& resource) {
                                     return resource.Get() == resourceBase.Get();
                                 });

        if (iter == _resources[typeID].end())
            _resources[typeID].push_back(resourceBase);
	}

    void ResizeResource(DXGI_MODE_DESC prevMode);

private:
    std::unordered_map<std::type_index, std::vector<SharedResource<ResourceBase>>> _resources;
};

