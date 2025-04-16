#pragma once
#include "Resource.h"

class Resource;
class ResourceManager : public Base
{
public:
	explicit ResourceManager() = default;
	virtual ~ResourceManager() = default;

public:
	template<typename T>
	requires (std::is_base_of_v<Resource, T>)
	std::shared_ptr<T> LoadResource(const std::filesystem::path& filePath)
	{
		std::type_index typeIndex = std::type_index(typeid(T));
		const wchar_t* path = filePath.c_str();

		std::weak_ptr<Resource> resource = _resources[typeIndex][path];
		std::shared_ptr<T> sharedResource = std::static_pointer_cast<T>(resource.lock());

		if (resource.expired())
		{
			sharedResource = std::make_shared<T>();
			sharedResource->LoadResource(path);

			_resources[typeIndex][path] = sharedResource;
		}

		return sharedResource;
	}	

private:
	// Base을(를) 통해 상속됨
	void Free() override;

private:
	std::unordered_map<std::type_index, std::unordered_map<std::wstring, std::weak_ptr<Resource>>> _resources;
};