#pragma once
#include "UmFramework.h"

/*이 클래스를 맴버로 들고있는 오브젝트는 DontDestroyOnLoad를 통한 싱글톤으로 만듭니다.*/
template <typename T>
class SingletonObject
{
    inline static GameObject* SingleToneObject = nullptr;
public:
    inline static GameObject* GetInstance(std::source_location location = std::source_location::current()) 
    { 
        if (nullptr == SingleToneObject)
        {
            std::string message = typeid(SingletonObject<T>).name();
            message += (const char*)"가 존재하지 않습니다.";
            UmLogger.Log(LogLevel::LEVEL_WARNING, message, location);
        }
        return SingleToneObject; 
    }

public:
    SingletonObject(T* owner) { _owner = owner; }
    ~SingletonObject()
    {
        if (&_owner->gameObject == SingleToneObject)
        {
            SingleToneObject = nullptr;
        }
    }

    /// <summary>
    /// 이 오브젝트의 싱글톤 여부를 반환합니다.
    /// </summary>
    bool IsSingleTon() { return &_owner->gameObject == SingleToneObject; }

    /// <summary>
    /// owner의 오브젝트를 싱글톤으로 설정합니다. (에디터 모드일때만 적용됩니다.)
    /// </summary>
    void SetSingleTon()
    {
        if (false == Global::IsPlay())
        {
            SingleToneObject = &_owner->gameObject;
        }
    }

    /// <summary>
    /// owner의 오브젝트를 싱글톤으로 만듭니다. 등록되면 true를 반환합니다. 플레이 모드일때만 적용됩니다.
    /// </summary>
    /// <param name="dontDestroyOnLoad :">DontDestroyOnLoad 여부</param>
    /// <returns>성공 여부</returns>
    bool TrySingleTon(bool dontDestroyOnLoad)
    {
        if (true == Global::IsPlay())
        {
            if (nullptr == SingleToneObject || SingleToneObject == &_owner->gameObject)
            {
                SingleToneObject = &_owner->gameObject;
                if (dontDestroyOnLoad)
                {
                    GameObject::DontDestroyOnLoad(SingleToneObject);
                }
                return true;
            }
            else
            {
                GameObject::Destroy(_owner->gameObject);
            }
        }
        return false;
    }

private:
    T* _owner = nullptr;
};


/*이 클래스를 맴버로 들고있는 컴포넌트를 싱글톤으로 만듭니다.*/
template <typename T>
class SingletonComponent
{
    inline static T* SingleToneComponent = nullptr;

public:
    inline static T* GetInstance(std::source_location location = std::source_location::current()) 
    {
        if (nullptr == SingleToneComponent)
        {
            std::string message = typeid(T).name();
            message += (const char*)"가 존재하지 않습니다.";
            UmLogger.Log(LogLevel::LEVEL_WARNING, message, location);
        }
        return SingleToneComponent; 
    }

public:
    SingletonComponent(T* owner) { _owner = owner; }
    ~SingletonComponent()
    {
        if (_owner == SingleToneComponent)
        {
            SingleToneComponent = nullptr;
        }
    }
    
    /// <summary>
    /// 이 컴포넌트의 싱글톤 여부를 반환합니다.
    /// </summary>
    /// <returns>결과 bool</returns>
    bool IsSingleTon() { return _owner == SingleToneComponent; }
      
    /// <summary>
    /// owner의 오브젝트를 싱글톤으로 설정합니다. (Reset에서 호출해야합니다.)
    /// </summary>
    void SetSingleTon()
    {
        if (false == Global::IsPlay())
        {
            SingleToneComponent = _owner;
        }
    }

    /// <summary>
    /// owner의 오브젝트를 싱글톤으로 만듭니다. 싱글톤 객체로 등록되면 true를 반환합니다. (Awake에서 호출해야합니다.)
    /// </summary>
    /// <returns>성공 여부</returns>
    bool TrySingleTon()
    {
        if (true == Global::IsPlay())
        {
            if (nullptr == SingleToneComponent)
            {
                SingleToneComponent = _owner;
                return true;
            }
            else
            {
                GameObject::Destroy(_owner);
            }
        }
        return false;
    }

private:
    T* _owner = nullptr;
};
