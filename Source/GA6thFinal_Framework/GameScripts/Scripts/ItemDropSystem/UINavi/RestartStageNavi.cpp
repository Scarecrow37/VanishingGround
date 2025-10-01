#include "pchScripts.h"
#include "RestartStageNavi.h"
#include "SceneTransition/TransitionManager.h"
#include "ExcelDataSystem/ExcelDataSystem.h"
#include "UI/Elements/Image/ImageElement.h"

UMREAL_COMPONENT(RestartStageNavi)

namespace RestartUtility
{
    static File::Guid GetSelectBox(RestartStageNavi::SelectBoxType type)
    {
        File::Guid guid;
        if (ExcelDataSystem* system = SingletonComponent<ExcelDataSystem>::GetInstance())
        {
            if (auto db = system->FindExcelDataBase(u8"전투"))
            {
                std::u8string_view rowKey;
                switch (type)
                {
                case RestartStageNavi::SelectBoxType::DEFALUT:
                    rowKey = u8"선택버튼_0";
                    break;
                case RestartStageNavi::SelectBoxType::FOCUS :
                    rowKey = u8"선택버튼_1";
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

RestartStageNavi::RestartStageNavi() 
{
    _imageElement = nullptr;
}

void RestartStageNavi::Submit()
{
    if (const Scene* scene = UmSceneManager.GetMainScene())
    {
        const std::string& path = scene->Path;
        GameObject* transitionManager = SingletonObject<TransitionManager>::GetInstance();
        if (transitionManager)
        {
            auto transitionComponent = transitionManager->GetComponent<TransitionManager>();
            if (transitionComponent)
            {
                transitionComponent->SceneTransitionFade("in", "out", [path]() { UmSceneManager.LoadScene(path); });
            }
        }
    }
}

void RestartStageNavi::FocusIn() 
{
    using namespace RestartUtility;

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

void RestartStageNavi::FocusOut() 
{
    using namespace RestartUtility;

    if (CheckImageElementWithLog(_imageElement))
    {
        File::Guid guid = GetSelectBox(SelectBoxType::DEFALUT);
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

void RestartStageNavi::Start() 
{
    using namespace RestartUtility;
    Base::Start();
    _imageElement = GetComponent<ImageElement>();
    CheckImageElementWithLog(_imageElement);
}

void RestartStageNavi::OnEnable()
{
    UINavigationComponent::OnEnable();

    Focus();
}
