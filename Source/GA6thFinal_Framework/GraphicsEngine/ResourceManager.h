#pragma once

class Resource;
class ResourceManager
{
public:
	ResourceManager();
	~ResourceManager();

public:
	template<typename T> requires (std::is_base_of_v<Resource, T>)
	std::shared_ptr<T> LoadResource(std::filesystem::path filePath)
	{      
        if (true == std::filesystem::exists(filePath))
        {
            filePath = std::filesystem::absolute(filePath);
            filePath = filePath.generic_string();
        }

		std::weak_ptr<Resource> resource       = _resources[typeid(T)][filePath];
        std::shared_ptr<T>      sharedResource = std::static_pointer_cast<T>(resource.lock());

		if (resource.expired())
		{
            sharedResource = std::make_shared<T>();
            sharedResource->LoadResource(filePath);

            _resources[typeid(T)][filePath] = sharedResource;
		}

		return sharedResource;
	}

    template <typename T> requires(std::is_base_of_v<Resource, T>)
    void AddResource(std::filesystem::path filePath, std::shared_ptr<T> resource)
    {
        _resources[typeid(T)][filePath] = resource;
        //auto resources = _resources[typeid(T)];
        //resources.try_emplace(filePath, resource);
    }

private:
    std::unordered_map<std::type_index, std::unordered_map<std::wstring, std::weak_ptr<Resource>>> _resources;
};

