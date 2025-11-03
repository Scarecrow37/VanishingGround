#include "pchScripts.h"
#include "RestartStageNavi.h"
#include "SceneTransition/SceneTransitionComponent.h"
#include "ExcelDataSystem/ExcelDataSystem.h"
#include "UI/Elements/Image/ImageElement.h"
#include "ItemDropSystem/ItemDropSystem.h"

UMREAL_COMPONENT(RestartStageNavi)

namespace RestartUtility
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

RestartStageNavi::RestartStageNavi() 
{
    _imageElement = nullptr;
}

void RestartStageNavi::Submit()
{
    Base::Submit();
    if (_clearCount < 3)
    {
        if (const Scene* scene = UmSceneManager.GetMainScene())
        {
            const std::string& path              = scene->Path;
            GameObject*        transitionManager = SingletonObject<SceneTransitionComponent>::GetInstance();
            if (transitionManager)
            {
                auto transitionComponent = transitionManager->GetComponent<SceneTransitionComponent>();
                if (transitionComponent)
                {
                    transitionComponent->SceneTransitionFade("in", "out", [path]() { UmSceneManager.LoadScene(path); });
                }
            }
            else
            {
                UmSceneManager.LoadScene(path);
            }
        }
    }
}

void RestartStageNavi::FocusIn(FocusCallType callType)
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

void RestartStageNavi::FocusOut(FocusCallType callType)
{
    using namespace RestartUtility;
    if (CheckImageElementWithLog(_imageElement))
    {
        if (_clearCount < 3)
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
        else
        {
            File::Guid guid = GetSelectBox(SelectBoxType::DISABLE);
            _imageElement->SetImage(guid);
        }
    }
}

void RestartStageNavi::Awake() 
{
    Base::Awake();
    _imageElement = GetComponent<ImageElement>();
    gameObject->AddTag(TAG);
}

void RestartStageNavi::Start()
{
    using namespace RestartUtility;
    Base::Start();
    CheckImageElementWithLog(_imageElement);
    if (ItemDropSystem* dropSystem = SingletonComponent<ItemDropSystem>::GetInstance())
    {
        _clearCount = dropSystem->StageClearCount;
    }

    if (_clearCount < 3)
    {
        if (_imageElement)
        {
            _imageElement->SetImage(GetSelectBox(SelectBoxType::DEFAULT));
            Enable = true;
        }
    }
    else
    {
        if (_imageElement)
        {
            _imageElement->SetImage(GetSelectBox(SelectBoxType::DISABLE));
            Enable = false;
        }
    }
}
