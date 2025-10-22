#pragma once

// 엔진 코어들을 모아놓은 관리 클래스
class EngineCores
{
    friend class Application;
    friend LogLocation::LogLocation(const std::source_location& location);

public:
    EngineCores(Application& app);
    ~EngineCores();

#ifdef _UMEDITOR
    bool IsPlay() const { return _isPlay; }
#else
    inline static constexpr bool IsPlay() { return true; }
#endif
    ETimeSystem              Time;
    ESceneManager            SceneManager;
    EGameObjectFactory       GameObjectFactory;
    EComponentFactory        ComponentFactory;
    ELogger                  Logger;
    EFileSystem              FileSystem;
    Application&             App;
    GraphicsCore             Graphics;
    ECommandManager          CommandManager;
    EFactoryRegister         FactoryRegister;
    Audio::Manager           Audio;
    Watcher::System          Watcher;
    ParticleEffectSerializer ParticleSerializer;
    EGizmoManager            GizmoManager;
    UI::Manager              UserInterface;
    PreferencesSystem        PreferencesSystem;
    ImGuiDebuggerWindow      DebuggerWindow;
    ReflectHelper::ImGuiDraw::InputAutoSetting ImGuiDrawPropertysSetting;
private:
    LogLocation::EngineLocationInfo _locationInfo;

private:
    void UpdateIsPlay();
#ifdef _UMEDITOR
    bool _isPlay;
#endif
};

// 안전한 접근 및 DLL에서 엔진 코어를 접근하기 위한 Wrapper 구조체
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

    SafeEngineCoresPtr(EngineCores* instance) { _instance.reset(instance); }
    void         operator=(std::shared_ptr<EngineCores> rhs) { _instance = rhs; }
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
    operator std::shared_ptr<EngineCores>() { return _instance; }
    operator bool() { return _instance.operator bool(); }

    EngineCores* get() { return _instance.get(); }

private:
    std::shared_ptr<EngineCores> _instance;
};

namespace Global
{
    extern SafeEngineCoresPtr engineCore; // 스크립트에서 엔진 접근을 위한 전역 변수.
}

#define UmCore Global::engineCore
#define UmTime UmCore->Time
#define UmSceneManager UmCore->SceneManager
#define UmLogger UmCore->Logger
#define UmGameObjectFactory UmCore->GameObjectFactory
#define UmComponentFactory UmCore->ComponentFactory
#define UmApplication UmCore->App
#define UmFileSystem UmCore->FileSystem
#define UmCommandManager UmCore->CommandManager
#define UmFactoryRegister UmCore->FactoryRegister
#define UmAudio UmCore->Audio
#define UmWatcher UmCore->Watcher
#define UmGizmoManager UmCore->GizmoManager
#define UmUI UmCore->UserInterface
#define UmPreferences UmCore->PreferencesSystem
// Graphics
#define UmParticleSerializer UmCore->ParticleSerializer
#define UmGraphics UmCore->Graphics
#define UmParticleManager UmGraphics.GetParticleManager()
#define UmResourceManager UmGraphics.GetResourceManager()
#define UmTransition UmGraphics.GetSceneTransitionCore()