#pragma once
#include "UmFramework.h"

/*이 클래스를 맴버로 들고있는 오브젝트는 DontDestroyOnLoad를 통한 싱글톤으로 만듭니다.*/
template <typename T>
class SingletonObject
{
    inline static GameObject* SingleToneObject = nullptr;
public:
    inline static GameObject* GetInstance() { return SingleToneObject; }

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
    /// owner의 오브젝트를 싱글톤으로 설정합니다. (에디터 모드일때만 적용됩니다.)
    /// </summary>
    void SetSingleTone()
    {
        if (false == Global::IsPlay())
        {
            SingleToneObject = &_owner->gameObject;
        }
    }

    /// <summary>
    /// owner의 오브젝트를 싱글톤으로 만듭니다. DontDestroyOnLoad로 등록되면 true를 반환합니다. 플레이 모드일때만 적용됩니다.
    /// </summary>
    /// <returns>성공 여부</returns>
    bool TrySingleTone()
    {
        if (true == Global::IsPlay())
        {
            if (nullptr == SingleToneObject || SingleToneObject == &_owner->gameObject)
            {
                SingleToneObject = &_owner->gameObject;
                GameObject::DontDestroyOnLoad(SingleToneObject);
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

