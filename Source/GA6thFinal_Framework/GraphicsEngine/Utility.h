#pragma once

template <typename T>
void SafeRelease(T& pointer)
{
	if (pointer)
	{
		pointer->Release();
		pointer = nullptr;
	}
}

template <typename T>
void SafeDelete(T& pointer)
{
	if (pointer)
	{
		delete pointer;
		pointer = nullptr;
	}
}

template <typename T>
class Singleton
{
protected:
	Singleton() = default;
	~Singleton() = default;
	Singleton(const Singleton&) = delete;
	Singleton& operator=(const Singleton&) = delete;
	Singleton(Singleton&&) = delete;
	Singleton& operator=(Singleton&&) = delete;

public:
	static T* GetInstance()
	{
		if (nullptr == _pInstance)
			_pInstance = new T;

		return _pInstance;
	}
	static void ResetInstance() { _pInstance = nullptr; }
private:
	static T* _pInstance;
};
template <typename T>
T* Singleton<T>::_pInstance = nullptr;

inline std::vector<uint8_t> ReadData(_In_z_ const wchar_t* name)
{
	std::ifstream inFile(name, std::ios::in | std::ios::binary | std::ios::ate);

#if !defined(WINAPI_FAMILY) || (WINAPI_FAMILY == WINAPI_FAMILY_DESKTOP_APP)
	if (!inFile)
	{
		wchar_t moduleName[_MAX_PATH] = {};
		if (!GetModuleFileNameW(nullptr, moduleName, _MAX_PATH))
			throw std::system_error(std::error_code(static_cast<int>(GetLastError()), std::system_category()), "GetModuleFileNameW");

		wchar_t drive[_MAX_DRIVE];
		wchar_t path[_MAX_PATH];

		if (_wsplitpath_s(moduleName, drive, _MAX_DRIVE, path, _MAX_PATH, nullptr, 0, nullptr, 0))
			throw std::runtime_error("_wsplitpath_s");

		wchar_t filename[_MAX_PATH];
		if (_wmakepath_s(filename, _MAX_PATH, drive, path, name, nullptr))
			throw std::runtime_error("_wmakepath_s");

		inFile.open(filename, std::ios::in | std::ios::binary | std::ios::ate);
	}
#endif

	if (!inFile)
	{
		return std::vector<uint8_t>();
		// throw std::runtime_error("ReadData");
	}

	const std::streampos len = inFile.tellg();

	std::vector<uint8_t> blob;
	blob.resize(size_t(len));

	inFile.seekg(0, std::ios::beg);
	if (!inFile)
		throw std::runtime_error("ReadData");

	inFile.read(reinterpret_cast<char*>(blob.data()), len);
	if (!inFile)
		throw std::runtime_error("ReadData");

	inFile.close();

	return blob;
}

inline void ClearBindResource(ID3D11DeviceContext* pDeviceContext, unsigned int startSlot, unsigned int count)
{
	ID3D11ShaderResourceView* nullSRV = nullptr;

	for (unsigned int i = startSlot; i < startSlot + count; i++)
		pDeviceContext->PSSetShaderResources(i, 1, &nullSRV);
}

#define ERROR_MESSAGE(msg) MessageBox(NULL, msg, L"Error", MB_OK | MB_ICONERROR)
#define FAILED_CHECK_BREAK(hr) if (((HRESULT)(hr)) < 0) __debugbreak()

#ifdef _DEBUG
#define ASSERT(expression, msg) \
if(!expression) { ERROR_MESSAGE(msg); assert(false); }
#else
#define ASSERT(expression, msg) ((void)0)
#endif