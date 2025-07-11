#pragma once

//Factory용 Class에 RegisterConstructor 함수가 구현되어 있어야합니다.
//cpp에서 사용해야합니다.
#define REGISTER_CLASS(FACTORY, CLASS)                                                  \
namespace FACTORY##_##CLASS##_IsRegister                                                \
    {                                                                                   \
    static bool IsRegister = EFactoryRegister::AddRegisterFunc(                         \
    [&]()                                                                               \
    {                                                                                   \
        FACTORY::RegisterConstructor(typeid(CLASS).name(), [](){return new CLASS;} );   \
    });                                                                                 \
};

//Factory용 클래스가 상속받아야 합니다.
template<typename T>
class FactoryConstructor
{
public:
    FactoryConstructor() = default;
    virtual ~FactoryConstructor() = default;

    /// <summary>
    /// 팩토리에 생성자를 등록합니다. 키로는 클래스의 typeid().name()이 사용됩니다.
    /// </summary>
    /// <param name="key :">키</param>
    /// <param name="func :">함수</param>
    /// <returns></returns>
    inline static bool RegisterConstructor(std::string_view key, std::function<T*()> func)
    {
        _staticConstructorMap[key.data()] = func;
        return true;
    }

    /// <summary>
    /// 팩토리에 등록된 객체를 생성하는 템플릿 함수입니다.
    /// </summary>
    /// <typeparam name="T"></typeparam>
    /// <returns></returns>
    template<typename T>
    inline static T* NewInstance()
    {
        const char* key = typeid(T).name();
        return NewInstanceWithKey(key);
    }

    /// <summary>
    /// 직접 키를통해 인스턴스를 생성합니다.
    /// </summary>
    /// <param name="key :">생성할 객체 키</param>
    /// <returns></returns>
    inline static T* NewInstanceWithKey(std::string_view key)
    {
        auto findIter = _staticConstructorMap.find(key.data());
        if (findIter != _staticConstructorMap.end())
        {
            return findIter->second();
        }
        return nullptr;
    }

    /// <summary>
    /// 팩토리에 등록된 모든 생성자들을 반환해줍니다.
    /// </summary>
    /// <returns></returns>
    inline static const std::unordered_map<std::string, std::function<T*()>>& GetInstanceConstructors()
    {
        return _staticConstructorMap;
    }
private:
    inline static std::unordered_map<std::string, std::function<T*()>> _staticConstructorMap;
};

class EFactoryRegister
{
private:
    inline static std::vector<std::function<void()>> _staticRegisterFuntions;

public:
    /// <summary>
    /// UmCores 초기화 이후 호출될 Factory용 초기화 함수들을 등록합니다.
    /// </summary>
    /// <typeparam name="Func"></typeparam>
    /// <param name="func"></param>
    /// <returns></returns>
    template<typename Func>
    static bool AddRegisterFunc(Func func) 
    {
        _staticRegisterFuntions.push_back(func);
        return true;
    };

    /// <summary>
    /// Register 함수들을 실행합니다.
    /// </summary>
    static void InvokeRegister()
    {
        for (auto& item : _staticRegisterFuntions)
        {
            item();
        }
        _staticRegisterFuntions.clear();
    }
};