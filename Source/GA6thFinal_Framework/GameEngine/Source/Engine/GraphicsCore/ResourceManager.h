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
		std::weak_ptr<Resource> resource       = _resources[filePath];
        std::shared_ptr<T>      sharedResource = std::static_pointer_cast<T>(resource.lock());

		if (resource.expired())
		{
            sharedResource = std::make_shared<T>();
            sharedResource->LoadResource(filePath);

            _resources[filePath] = sharedResource;

            /*auto temp = MakeShared(type, filePath, sizeof(T));
            sharedResource = std::static_pointer_cast<T> (_resources[type][filePath.data()].lock());
            new (sharedResource.get())(T);
			sharedResource->LoadResource(filePath);
            _resources[type][filePath.data()] = sharedResource;*/
		}

		return sharedResource;
	}

    template <typename T> requires(std::is_base_of_v<Resource, T>)
    void AddResource(std::filesystem::path filePath, std::shared_ptr<T> resource)
    {
        auto iter = _resources.find(filePath);
        if (iter != _resources.end())
        {
            //ASSERT(false, L"Already register resource");
            return;
        }

        _resources[filePath] = resource;
    }

    void Clear();
private:
    std::unordered_map<std::wstring, std::weak_ptr<Resource>> _resources;
};

