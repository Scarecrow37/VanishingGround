#include "pchScripts.h"
#include "EraseRevelationUIManager.h"
#include "UI/Elements/Image/ImageElement.h"
#include "UI/Panels/Description/DescriptionPanel.h"
#include "ItemDropSystem/UINavi/EraseRevelationNavi.h"
#include "UI/Elements/Text/TextElement.h"
#include "ItemDropSystem/UI/ArtifactUIManager.h"
#include "ItemDropSystem/UI/ItemDropUIRootManager.h"
#include "RevelationSystem/RevelationSystem.h"

UMREAL_COMPONENT(EraseRevelationUIManager)

EraseRevelationUIManager::EraseRevelationUIManager() 
    :
    _singletonComponent(this)
{

}
EraseRevelationUIManager::~EraseRevelationUIManager() = default;

void EraseRevelationUIManager::OpenUI(const size_t artifactObtainIndex)
{
    gameObject->SetActive(true);
    if (RevelationSystem* system = SingletonComponent<RevelationSystem>::GetInstance())
    {
        auto& revelations = system->GetPlayerElementList();
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
    }
    if (0 < _focusNaviElements.size())
    {
        _focusNaviElements[0]->Focus();
        _artifactObtainIndex = artifactObtainIndex;
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
    if (_revationInfo.Name)
    {
        _revationInfo.Name->Text = info.Name;
    }
    if (_revationInfo.Icon)
    {
        int iconID = DropItemInfo::GetArtifactIconID(info);
        const File::Guid& guid = UmFileSystem.GetGuidFromAssetID(iconID);
        _revationInfo.Icon->SetImage(guid);
    }
    if (_revationInfo.Description)
    {
        _revationInfo.Description->Description = DropItemInfo::GetArtifactDescription(info);
    }
    if (_revationInfo.Flavor)
    {
        //TODO: 플레이버 텍스트 추가 필요
        _revationInfo.Flavor->Description = "";
    }
    if (_revationInfo.Keyword)
    {
        //키워드 텍스트 추가 필요
        _revationInfo.Keyword->Description = "";
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
                        _revationInfo.Name = object.GetComponent<TextElement>();
                    }
                    else if(object.CompareTag("Icon"))
                    {
                        _revationInfo.Icon = object.GetComponent<ImageElement>();
                    }
                    else if (object.CompareTag("Description"))
                    {
                        _revationInfo.Description = object.GetComponent<DescriptionPanel>();
                    }
                    else if (object.CompareTag("Flavor"))
                    {
                        _revationInfo.Flavor = object.GetComponent<DescriptionPanel>();
                    }
                    else if (object.CompareTag("Keyword"))
                    {
                        _revationInfo.Keyword = object.GetComponent<DescriptionPanel>();
                    }
                });
            }
        }
    }
}

void EraseRevelationUIManager::OnButtonDownB(const Input::Controller&)
{
    if (gameObject->ActiveInHierarchy)
    {
        _closeFlag = true;
    }
}