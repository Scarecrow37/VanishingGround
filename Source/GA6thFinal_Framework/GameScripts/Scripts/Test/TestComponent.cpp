#include "TestComponent.h"
#include "Scripts/FileSystemTest/FileTestComponent.h"
TestComponent::TestComponent()  = default;
TestComponent::~TestComponent() = default;

using namespace Global;
using namespace u8_literals;

void TestComponent::Update()
{
    ImGui::Begin(u8"테스트 컴포넌트 업데이트 호출중!!!"_c_str);
    {
        transform->ImGuiDrawPropertys();
    }
    ImGui::End();
    static float currTime = 0.f;
    constexpr float addTime  = 1.f;

    //currTime += UmTime.deltaTime();
    //while (addTime <= currTime)
    //{
    //    AddComponent<FileTestComponent>();
    //    currTime -= addTime;
    //}
}

void TestComponent::FixedUpdate() 
{
    //engineCore->EngineLogger.Log(LogLevel::LEVEL_DEBUG, "Fixed Update!");    
}

void TestComponent::OnDestroy()
{
    engineCore->EngineLogger.Log(LogLevel::LEVEL_DEBUG, "OnDestroy!");
}

void TestComponent::OnApplicationQuit()
{
    engineCore->EngineLogger.Log(LogLevel::LEVEL_DEBUG, "OnApplicationQuit!");
}

void TestComponent::Reset()
{
    engineCore->EngineLogger.Log(LogLevel::LEVEL_DEBUG, "Reset!");
}

void TestComponent::Awake()
{
    engineCore->EngineLogger.Log(LogLevel::LEVEL_DEBUG, "Awake!");

    for (int i = LogLevel::LEVEL_TRACE; i <= LogLevel::LEVEL_FATAL; i++)
    {
        engineCore->EngineLogger.Log(i, LogLevel::LogLevelTo_c_str(i));
    }
}

void TestComponent::Start()
{
    engineCore->EngineLogger.Log(LogLevel::LEVEL_DEBUG, "Start!");
}

void TestComponent::OnEnable()
{
    engineCore->EngineLogger.Log(LogLevel::LEVEL_DEBUG, "OnEnable!");
}

void TestComponent::OnDisable()
{
    engineCore->EngineLogger.Log(LogLevel::LEVEL_DEBUG, "OnDisable!");
}

void TestComponent::SerializedReflectEvent()
{
    engineCore->EngineLogger.Log(LogLevel::LEVEL_DEBUG,
                                 "SerializedReflectEvent");
}

void TestComponent::DeserializedReflectEvent()
{
    engineCore->EngineLogger.Log(LogLevel::LEVEL_DEBUG,
                                 "DeserializedReflectEvent");
}