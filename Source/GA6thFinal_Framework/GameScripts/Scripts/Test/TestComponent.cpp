#include "TestComponent.h"
#include "Scripts/FileSystemTest/FileTestComponent.h"
TestComponent::TestComponent()
{
    ObjectDrop.SetDragDropFunc([this]
    {
        if (const ImGuiPayload* payLoad = ImGui::AcceptDragDropPayload(DragDropTransform::KEY))
        {
            using Data = DragDropTransform::Data;
            Data* data = (Data*)payLoad->Data;
            ReflectFields->objectName = data->pTransform->gameObject->Name;
        }
    });

}
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
    //UmLogger.Log(LogLevel::LEVEL_DEBUG, "Fixed Update!");    
}

void TestComponent::OnDestroy()
{
    UmLogger.Log(LogLevel::LEVEL_DEBUG, "OnDestroy!");
}

void TestComponent::OnApplicationQuit()
{
    UmLogger.Log(LogLevel::LEVEL_DEBUG, "OnApplicationQuit!");
}

void TestComponent::Reset()
{
    UmLogger.Log(LogLevel::LEVEL_DEBUG, "Reset!");
}

void TestComponent::Awake()
{
    UmLogger.Log(LogLevel::LEVEL_DEBUG, "Awake!");
}

void TestComponent::Start()
{
    UmLogger.Log(LogLevel::LEVEL_DEBUG, "Start!");
}

void TestComponent::OnEnable()
{
    UmLogger.Log(LogLevel::LEVEL_DEBUG, "OnEnable!");
}

void TestComponent::OnDisable()
{
    UmLogger.Log(LogLevel::LEVEL_DEBUG, "OnDisable!");
}

void TestComponent::SerializedReflectEvent()
{
    UmLogger.Log(LogLevel::LEVEL_DEBUG,
                                 "SerializedReflectEvent");
    Vector3 factor = emit.GetFactor();
    std::memcpy(ReflectFields->_factor.data(), &factor, sizeof(ReflectFields->_factor));
}

void TestComponent::DeserializedReflectEvent()
{
    UmLogger.Log(LogLevel::LEVEL_DEBUG,
                                 "DeserializedReflectEvent");

    emit.SetFactor(Vector3{ReflectFields->_factor.data()});
}