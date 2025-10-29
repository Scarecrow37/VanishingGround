#include "pchScripts.h"
#include "EraseRevelationUIManager.h"
#include "UI/Elements/Image/ImageElement.h"
#include "UI/Panels/Description/DescriptionPanel.h"
#include "ItemDropSystem/UINavi/EraseRevelationNavi.h"
#include "UI/Elements/Text/TextElement.h"
#include "ItemDropSystem/UI/ArtifactUIManager.h"
#include "ItemDropSystem/UI/ItemDropUIRootManager.h"
#include "RevelationSystem/RevelationSystem.h"
#include "Input/InputOkCancelComponent/InputOkCancelComponent.h"

UMREAL_COMPONENT(EraseRevelationUIManager)

EraseRevelationUIManager::EraseRevelationUIManager() 
    :
    _singletonComponent(this)
{

}
EraseRevelationUIManager::~EraseRevelationUIManager() = default;

void EraseRevelationUIManager::OpenUI(const size_t artifactObtainIndex)
{
    if (RevelationSystem* system = SingletonComponent<RevelationSystem>::GetInstance())
    {
        auto& revelations = system->GetPlayerElementList();
        if (3 < revelations.size())
        {
            // UI 활성화
            gameObject->SetActive(true);

            // UI 갱신
            for (size_t i = 0; i < _revelationImages.size(); ++i)
            {
                EraseRevelationNavi* navi = nullptr;
                if (i < _focusNaviElements.size())
                {
                    navi = _focusNaviElements[i];
                }
                ImageElement* iconImage = _revelationImages[i];
                if (i < revelations.size())
                {
                    auto& revelation = revelations[i];
                    if (revelation)
                    {
                        // 이미지 설정
                        DropItemInfo      info   = revelation->GetItemInfo();
                        int               iconID = DropItemInfo::GetArtifactIconID(info);
                        const File::Guid& guid   = UmFileSystem.GetGuidFromAssetID(iconID);
                        iconImage->SetImage(guid);
                        iconImage->Enable = true;

                        if (navi)
                        {
                            navi->Enable = true;
                            navi->SetItemInfo(info);
                        }
                    }
                }
                else
                {
                    iconImage->Enable = false;
                    if (navi)
                    {
                        navi->Enable = false;
                    }
                }
            }

            // UI 포커스
            if (0 < _focusNaviElements.size())
            {
                _focusNaviElements[0]->Focus();
                _artifactObtainIndex = artifactObtainIndex;
            }
        }
        else
        {
            UmLogger.Log(LogLevel::LEVEL_WARNING, u8"계시 지우기는 계시가 4개 이상일때만 가능합니다.");
        }       
    }
}

void EraseRevelationUIManager::CloseUI() 
{
    if (ItemDropUIRootManager* rootManager = SingletonComponent<ItemDropUIRootManager>::GetInstance())
    {
        rootManager->AutoFocus(false);
    }
    gameObject->SetActive(false);
}

void EraseRevelationUIManager::EraseRevelation(int slot) 
{
    if (RevelationSystem* system = SingletonComponent<RevelationSystem>::GetInstance())
    {
        system->RemovePlayerElement(slot);
    }
    if (ArtifactUIManager* manager = SingletonComponent<ArtifactUIManager>::GetInstance())
    {
        manager->ObtainFocusNavi(_artifactObtainIndex);
    }
    CloseUI();
}

void EraseRevelationUIManager::SetRevelationInfoUI(const DropItemInfo& info) 
{
    if (_revelation.Name)
    {
        _revelation.Name->Text = info.Name;
    }
    if (_revelation.Icon)
    {
        int iconID = DropItemInfo::GetArtifactIconID(info);
        const File::Guid& guid = UmFileSystem.GetGuidFromAssetID(iconID);
        _revelation.Icon->SetImage(guid);
    }
    if (_revelation.Description)
    {
        _revelation.Description->Description = DropItemInfo::GetArtifactDescription(info);
    }
    if (_revelation.Flavor)
    {
        //TODO: 플레이버 텍스트 추가 필요
        _revelation.Flavor->Description = "";
    }
    if (_revelation.Keyword)
    {
        //키워드 텍스트 추가 필요
        _revelation.Keyword->Description = "";
    }
}

void EraseRevelationUIManager::SetWarningIcon(int slot) 
{
    if (auto image = _warningImage.lock())
    {
        if (RevelationSystem* system = SingletonComponent<RevelationSystem>::GetInstance())
        {
            auto& list = system->GetPlayerElementList();
            if (0 <= slot && slot < static_cast<int>(list.size()))
            {
                auto& revelation = list[slot];
                if (revelation)
                {
                    DropItemInfo info = revelation->GetItemInfo();
                    int id = DropItemInfo::GetArtifactIconID(info);
                    image->SetImage(UmFileSystem.GetGuidFromAssetID(id));
                }
            }
        }
    }
}

void EraseRevelationUIManager::Added() 
{
    if (_singletonComponent.TrySingleTon())
    {
        gameObject->AddTag(TAG);
        gameObject->SetActive(true);
    }
}

void EraseRevelationUIManager::Awake()
{
    Base::Awake();
    if (_singletonComponent.IsSingleTon())
    {      
        BindInputAction(ControllerButton::B, Action::PRESSED, this, &EraseRevelationUIManager::OnButtonDownB);
        FindElements();
    }
}

void EraseRevelationUIManager::Start() 
{
    Base::Start();
    _closeFlag = true;
}

void EraseRevelationUIManager::Update() 
{
    Base::Update();
    if (_closeFlag)
    {
        CloseUI();
        _closeFlag = false;
    }
}


void EraseRevelationUIManager::FindElements()
{
    for (int i = 0; i < transform->ChildCount; ++i)
    {
        Transform* child = transform->GetChild(i);
        if (child)
        {
            if (child->gameObject->CompareTag("Revelation Select"))
            {
                _focusNaviElements.clear();
                _revelationImages.clear();
                Transform::ForeachDFS(*child, [this](Transform* curr) 
                {
                    GameObject& object = curr->gameObject;
                    if (object.CompareTag("Focus"))
                    {
                        if (EraseRevelationNavi* navi = object.GetComponent<EraseRevelationNavi>())
                        {
                            navi->_mySlot = static_cast<int>(_focusNaviElements.size());
                            _focusNaviElements.push_back(navi);
                        }                   
                    }
                    else if (object.CompareTag("Icon"))
                    {                      
                        if (ImageElement* icon = object.GetComponent<ImageElement>())
                        {
                            _revelationImages.push_back(icon);
                        }
                    }
                });
            }
            else if (child->gameObject->CompareTag("Revelation Info"))
            {
                Transform::ForeachDFS(*child, [this](Transform* curr) 
                {
                    GameObject& object = curr->gameObject;
                    if (object.CompareTag("Name"))
                    {
                        _revelation.Name = object.GetComponent<TextElement>();
                    }
                    else if(object.CompareTag("Icon"))
                    {
                        _revelation.Icon = object.GetComponent<ImageElement>();
                    }
                    else if (object.CompareTag("Description"))
                    {
                        _revelation.Description = object.GetComponent<DescriptionPanel>();
                    }
                    else if (object.CompareTag("Flavor"))
                    {
                        _revelation.Flavor = object.GetComponent<DescriptionPanel>();
                    }
                    else if (object.CompareTag("Keyword"))
                    {
                        _revelation.Keyword = object.GetComponent<DescriptionPanel>();
                    }
                });
            }
            else if (child->gameObject->CompareTag("Warning Panel"))
            {
                GameObject& object      = child->gameObject;
                if (InputOkCancelComponent* okCancel = object.GetComponent<InputOkCancelComponent>())
                {
                    _inputOkCancelComponent = okCancel->GetWeakPtrAs<InputOkCancelComponent>();
                }            

                Transform::ForeachBFS(object.transform, [this](Transform* curr)
                {   
                    GameObject& currObject = curr->gameObject;
                    if (currObject.CompareTag("Icon"))
                    {
                        if (ImageElement* image = currObject.GetComponent<ImageElement>())
                            _warningImage = image->GetWeakPtrAs<ImageElement>();
                    }
                });
            }
        }
    }
}

void EraseRevelationUIManager::OnButtonDownB(const Input::Controller&)
{
    bool isActiveWarning = false;
    if (auto okCancel = _inputOkCancelComponent.lock())
    {
        isActiveWarning = okCancel->EnableInHierarchy;
    }
    if (gameObject->ActiveInHierarchy && false == isActiveWarning)
    {
        _closeFlag = true;
    }
}