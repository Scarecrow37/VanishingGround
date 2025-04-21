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
    void Update();
    void RegisterLoadQueue(const std::pair<std::filesystem::path, RESOURCE_TYPE>& data);
    void Clear();
    //std::shared_ptr<Resource> MakeShared(std::string_view type, std::wstring_view path, size_t size);

private:
    std::unordered_map<std::wstring, std::weak_ptr<Resource>> _resources;
    std::set<std::pair<std::wstring, RESOURCE_TYPE>>          _loadQueue;
    std::list<std::shared_ptr<Resource>>                      _tempResource;
};

