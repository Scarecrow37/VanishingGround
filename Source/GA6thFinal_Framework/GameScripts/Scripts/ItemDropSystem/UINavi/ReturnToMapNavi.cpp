#include "pchScripts.h"
#include "ReturnToMapNavi.h"
#include "SceneTransition/SceneTransitionComponent.h"
#include "ExcelDataSystem/ExcelDataSystem.h"
#include "UI/Elements/Image/ImageElement.h"
#include "RestartStageNavi.h"

UMREAL_COMPONENT(ReturnToMapNavi)

namespace ReturnUtility
{
    static bool CheckImageElementWithLog(ImageElement* ptr)
    {
        if (ptr == nullptr)
        {
            UmLogger.Log(LogLevel::LEVEL_WARNING, u8"Image Element를 찾을수 없습니다.");
            return false;
        }
        else
        {
            return true;
        }
    }
}

ReturnToMapNavi::ReturnToMapNavi()
{
    MapScene.SetInputAutoEvent([this]() {
        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload* payLoad = ImGui::AcceptDragDropPayload(DragDropAsset::KEY))
            {
                const DragDropAsset::Data* data = static_cast<DragDropAsset::Data*>(payLoad->Data);
                if (const auto extension = data->GetPath().extension(); extension == L".UmScene")
                {
                    _Guid                = data->GetGuid();
                    ReflectFields->MapScene = _Guid.string();
                }
            }
            ImGui::EndDragDropTarget();
        }
    });

    _imageElement = nullptr;
}

void ReturnToMapNavi::Submit()
{
    Base::Submit();
    const File::Path& path = _Guid.ToPath();

    GameObject* transitionManager = SingletonObject<SceneTransitionComponent>::GetInstance();
    if (transitionManager)
    {
        auto transitionComponent = transitionManager->GetComponent<SceneTransitionComponent>();
        if (transitionComponent)
        {
            transitionComponent->SceneTransitionFade("in", "out", [path]() { UmSceneManager.LoadScene(path.string()); });
        }
        else
        {
            UmSceneManager.LoadScene(path.string());
        }
    }
}

void ReturnToMapNavi::FocusIn(FocusCallType callType) 
{
    using namespace ReturnUtility;

    if (CheckImageElementWithLog(_imageElement))
    {
        File::Guid guid = RestartStageNavi::GetSelectBox(RestartStageNavi::SelectBoxType::FOCUS);
        if (guid.IsNull())
        {
            UmLogger.Log(LogLevel::LEVEL_WARNING, u8"포커스 이미지를 찾을 수 없습니다.");
        }
        else
        {
            _imageElement->SetImage(guid);
        }
    }
}

void ReturnToMapNavi::FocusOut(FocusCallType callType) 
{
     using namespace ReturnUtility;

    if (CheckImageElementWithLog(_imageElement))
    {
        File::Guid guid = RestartStageNavi::GetSelectBox(RestartStageNavi::SelectBoxType::DEFAULT);
        if (guid.IsNull())
        {
            UmLogger.Log(LogLevel::LEVEL_WARNING, u8"일반 이미지를 찾을 수 없습니다.");
        }
        else
        {
            _imageElement->SetImage(guid);
        }
    }
}

void ReturnToMapNavi::DeserializedReflectEvent()
{
    UINavigationComponent::DeserializedReflectEvent();

    const File::Guid guid = ReflectFields->MapScene;
    if (const auto path = guid.ToPath(); !path.IsNull())
    {
        _Guid = path.ToGuid();
    }
}

void ReturnToMapNavi::Added() 
{
    if (UmCore->IsPlay())
    {
        gameObject->AddTag(TAG);
    }
}

void ReturnToMapNavi::Awake() 
{
    Base::Awake();
    _imageElement = GetComponent<ImageElement>();
}

void ReturnToMapNavi::Start() 
{
    using namespace ReturnUtility;
    Base::Start();
    CheckImageElementWithLog(_imageElement);
    if (_imageElement)
    {
        _imageElement->SetImage(RestartStageNavi::GetSelectBox(RestartStageNavi::SelectBoxType::DEFAULT));
    }
}
