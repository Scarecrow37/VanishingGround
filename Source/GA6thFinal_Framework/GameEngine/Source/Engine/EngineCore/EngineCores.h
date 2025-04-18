﻿#pragma once

//엔진 코어들을 모아놓은 관리 클래스
class EngineCores
{
public:
    EngineCores();
    ~EngineCores();

    ETimeSystem Time;
    ESceneManager SceneManager;
    EGameObjectFactory GameObjectFactory;
    EComponentFactory ComponentFactory;
    EEngineLogger EngineLogger;
};

//안전한 접근 및 DLL에서 엔진 코어를 접근하기 위한 Wrapper 구조체
struct SafeEngineCoresPtr
{
    struct Engine
    {
        /*엔진 코어를 생성합니다.*/
        static void CreateEngineCores();

        /*엔진 코어를 파괴합니다.*/
        static void DestroyEngineCores();
    };
    SafeEngineCoresPtr();
    ~SafeEngineCoresPtr();

    SafeEngineCoresPtr(EngineCores* instance)
    {
        _instance.reset(instance);
    }
    void operator=(std::shared_ptr<EngineCores> rhs)
    {
        _instance = rhs;
    }
    EngineCores* operator->()
    {
        if (_instance == nullptr)
        {     
            assert(!"엔진 코어가 생성되지 않았습니다.");
#ifndef _DEBUG          
            __debugbreak();
#endif
        }
        return _instance.get();
    }
    operator std::shared_ptr<EngineCores>()
    {
        return _instance;
    }
    operator bool()
    {
        return _instance.operator bool();
    }
private:
    std::shared_ptr<EngineCores> _instance;
};

namespace Global
{
    extern SafeEngineCoresPtr engineCore; //스크립트에서 엔진 접근을 위한 전역 변수.
}


