#include "pchScripts.h"
#include "TestComponent.h"

UMREAL_COMPONENT(TestComponent)

TestComponent::TestComponent()
{
    ObjectDrop.SetInputAutoEvent([this]
    {
        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload* payLoad = ImGui::AcceptDragDropPayload(DragDropTransform::KEY))
            {
                using Data                = DragDropTransform::Data;
                Data* data                = (Data*)payLoad->Data;
                ReflectFields->objectName = data->pTransform->gameObject->Name;
            }

            ImGui::EndDragDropTarget();
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
}

void TestComponent::FixedUpdate() 
{
    //UmLogger.Log(LogLevel::LEVEL_DEBUG, "Fixed Update!");    
}

void TestComponent::OnDestroy()
{
    std::string message = "OnDestroy!";
    message += " Frame : ";
    message += std::to_string(UmTime.FrameCount());
    UmLogger.Log(LogLevel::LEVEL_DEBUG, message);
}

void TestComponent::OnApplicationQuit()
{
    std::string message = "OnApplicationQuit!";
    message += " Frame : ";
    message += std::to_string(UmTime.FrameCount());
    UmLogger.Log(LogLevel::LEVEL_DEBUG, message);
}

void TestComponent::OnLoadScene(Scene& scene, LoadSceneMode mode) 
{
    std::string message = "OnLoadScene! ";
    message += (std::string)scene.Path;
    message += ", ";
    message += rfl::enum_to_string(mode);
    message += " Frame : ";
    message += std::to_string(UmTime.FrameCount());
    UmLogger.Log(LogLevel::LEVEL_DEBUG, message);
}

void TestComponent::Reset()
{
    std::string message = "Reset!";
    message += " Frame : ";
    message += std::to_string(UmTime.FrameCount());
    UmLogger.Log(LogLevel::LEVEL_DEBUG, message);
}

void TestComponent::Awake()
{
    std::string message = "Awake!";
    message += " Frame : ";
    message += std::to_string(UmTime.FrameCount());
    UmLogger.Log(LogLevel::LEVEL_DEBUG, message);
    if (ReflectFields->TestDontDestroyOnLoad)
    {
        GameObject::DontDestroyOnLoad(gameObject);
    }
}

void TestComponent::Start()
{
    std::string message = "Start!";
    message += " Frame : ";
    message += std::to_string(UmTime.FrameCount());
    UmLogger.Log(LogLevel::LEVEL_DEBUG, message);
}

void TestComponent::OnEnable()
{
    std::string message = "OnEnable!";
    message += " Frame : ";
    message += std::to_string(UmTime.FrameCount());
    UmLogger.Log(LogLevel::LEVEL_DEBUG, message);
}

void TestComponent::OnDisable()
{
    std::string message = "OnDisable!";
    message += " Frame : ";
    message += std::to_string(UmTime.FrameCount());
    UmLogger.Log(LogLevel::LEVEL_DEBUG, message);
}

void TestComponent::SerializedReflectEvent()
{
    std::string message = "SerializedReflectEvent!";
    message += " Frame : ";
    message += std::to_string(UmTime.FrameCount());
    UmLogger.Log(LogLevel::LEVEL_DEBUG, message);
}

void TestComponent::DeserializedReflectEvent()
{
    std::string message = "DeserializedReflectEvent!";
    message += " Frame : ";
    message += std::to_string(UmTime.FrameCount());
    UmLogger.Log(LogLevel::LEVEL_DEBUG, message);
}

void TestComponent::ImGuiDrawPropertysEvent()
{
#ifdef _UMEDITOR
    if (ImGui::TreeNode("Add Component test"))
    {
        if (ImGui::Button(u8"테스트 컴포넌트 추가"_c_str))
        {
            AddComponent<TestComponent>();
        }
        ImGui::TreePop();
    }

    if (ImGui::TreeNode("Set Active test"))
    {
        if (ImGui::Button(u8"오브젝트 Active 동시 변경 테스트"_c_str))
        {
            gameObject->SetActive(false);
            gameObject->SetActive(true);
            gameObject->SetActive(false);
            gameObject->SetActive(true);
        }
        if (ImGui::Button(u8"컴포넌트 Active 동시 변경 테스트"_c_str))
        {
            Enable = false;
            Enable = true;
            Enable = false;
            Enable = true;
        }
        ImGui::TreePop();
    }

    if (ImGui::TreeNode("Destroy test"))
    {
        if (ImGui::Button(u8"오브젝트 생성 파괴 동시 테스트"_c_str))
        {
            auto object = NewGameObject("test");
            object->AddComponent<TestComponent>();
            GameObject::Destroy(object.get());
        }
        ImGui::TreePop();
    }

    ImGuiDrawEditGimzmoes();
#endif
}

void TestComponent::OnDrawDebug() 
{
#ifdef _UMEDITOR

#endif
}

void TestComponent::OnDrawDebugSelected() 
{
#ifdef _UMEDITOR
    DrawGizmoIcon();
    DrawGuizmo();
#endif
}

void TestComponent::PushGizmo()
{
#ifdef _UMEDITOR
    int size = (int)_gizmoes.size();
    auto& [gizmo, matrix, icon] = _gizmoes.emplace_back(this, Matrix::Identity, SceneGizmo::DefaultIcon::TETHER);
    gizmo.SetIconTexture(icon);
    gizmo.EventListener.AddListener([this, index = size]() { _selectGizmoIndex = index; });

    //matrix 포인터 이동하기 때문에 다시 설정해야함.
    for (auto& [gizmo, matrix, icon] : _gizmoes)
    {
        gizmo.SetOwnerMatrix(matrix);
    }
#endif
}

void TestComponent::PopGizmo() 
{
#ifdef _UMEDITOR
    _gizmoes.pop_back();
#endif
}

void TestComponent::DrawGuizmo() 
{
#ifdef _UMEDITOR
    if (0 <= _selectGizmoIndex && _selectGizmoIndex < _gizmoes.size())
    {
        auto& [gizmo, matrix, icon] = _gizmoes[_selectGizmoIndex];
        gizmo.DrawImGuizmo();
    }
#endif
}

void TestComponent::DrawGizmoIcon() 
{
#ifdef _UMEDITOR
    for (auto& [gizmo, matrix, icon] : _gizmoes)
    {
        gizmo.DrawIcon();
    }
#endif
}

void TestComponent::ImGuiDrawEditGimzmoes()
{
#ifdef _UMEDITOR
    if (ImGui::TreeNode("Gizmo test"))
    {
        for (auto& [gizmo, matrix, gizmoIcon] : _gizmoes)
        {
            ImGui::PushID(&gizmo);
            constexpr auto defaultIcons = rfl::get_enumerator_array<SceneGizmo::DefaultIcon>();
            std::string    iconName     = rfl::enum_to_string(gizmoIcon);
            if (ImGui::BeginCombo("##gizmo icon", iconName.c_str()))
            {
                int id = 0;
                for (auto& [name, icon] : defaultIcons)
                {
                    ImGui::PushID(id++);
                    if (ImGui::Selectable(name.data(), name == iconName))
                    {
                        gizmoIcon = icon;
                        gizmo.SetIconTexture(gizmoIcon);
                    }
                    ImGui::PopID();
                }
                ImGui::EndCombo();
            }
            ImGui::PopID();
        }
        if (ImGui::Button("Push gizmo"))
        {
            PushGizmo();
        }
        if (ImGui::Button("Pop gizmo"))
        {
            PopGizmo();
        }
        ImGui::TreePop();
    }
#endif
}
