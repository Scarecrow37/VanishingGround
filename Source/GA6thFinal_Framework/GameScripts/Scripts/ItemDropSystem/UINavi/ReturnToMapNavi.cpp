#include "pchScripts.h"
#include "ReturnToMapNavi.h"
#include "SceneTransition/SceneTransitionComponent.h"
#include "ExcelDataSystem/ExcelDataSystem.h"
#include "UI/Elements/Image/ImageElement.h"

UMREAL_COMPONENT(ReturnToMapNavi)

namespace ReturnUtility
{
    static File::Guid GetSelectBox(ReturnToMapNavi::SelectBoxType type)
    {
        File::Guid guid;
        if (ExcelDataSystem* system = SingletonComponent<ExcelDataSystem>::GetInstance())
        {
            if (auto db = system->FindExcelDataBase(u8"전투"))
            {
                std::u8string_view rowKey;
                switch (type)
                {
                case ReturnToMapNavi::SelectBoxType::DEFAULT:
                    rowKey = u8"나가기/ 다음전투 선택버튼 포커스 안됨";
                    break;
                case ReturnToMapNavi::SelectBoxType::FOCUS:
                    rowKey = u8"나가기/ 다음전투 선택버튼 포커스 됨";
                    break;
                default:
                    break;
                }

                size_t rowIndex = db->FindRowIndex(rowKey, u8"Description");
                if (rowIndex != db->FIND_INDEX_FAIL)
                {
                    std::string_view data = db->FindData(rowIndex, u8"ID");
                    if (data != db->FIND_STR_FAIL)
                    {
                        int id = std::stoi(data.data());
                        guid   = UmFileSystem.GetGuidFromAssetID(id);

                        if (guid.IsNull())
                        {
                            std::string message = "Asset ID : ";
                            message += std::to_string(id);
                            message += " is not found";
                            UmLogger.Log(LogLevel::LEVEL_WARNING, message);
                        }
                    }
                }
            }
        }
        return guid;
    }

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
        File::Guid guid = GetSelectBox(SelectBoxType::FOCUS);
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
        File::Guid guid = GetSelectBox(SelectBoxType::DEFAULT);
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
        _imageElement->SetImage(GetSelectBox(SelectBoxType::DEFAULT));
    }
}
