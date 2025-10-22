#include "pchScripts.h"
#include "ArtifactUIManager.h"
#include "UI/Panels/Grid/GridPanel.h"
#include "UI/Elements/Image/ImageElement.h"
#include "ItemDropSystem/UI/ItemDropUIRootManager.h"
#include "ItemDropSystem/ItemDropSystem.h"
#include "ViewModels/ItemDrop/DropArtifacts/DropArtifactsViewModel.h"
#include "ItemDropSystem/UINavi/ArtifactButtonNavi.h"

UMREAL_COMPONENT(ArtifactUIManager)

File::Guid ArtifactUIManager::GetObtainFrameGuid()
{
    return UmFileSystem.GetGuidFromAssetID(460009); //일단 하드코딩...
} 

ArtifactUIManager::ArtifactUIManager()
{
    _gridPanel = nullptr;
    _obtainFlag = false;
}

ArtifactUIManager::~ArtifactUIManager()
{

}

void ArtifactUIManager::Reset() 
{
    _singletonComponent.SetSingleTon();
    Base::Reset();
}

void ArtifactUIManager::Awake() 
{
    if (_singletonComponent.TrySingleTon())
    {
        gameObject->AddTag(TAG);
        Base::Awake();
    }
}

void ArtifactUIManager::Start() 
{
    if (_singletonComponent.IsSingleTon())
    {
        //Watch
        _viewModelHandle = UmWatcher.Watch<DropArtifactsViewModel, std::vector<DropArtifactsUIData>>
        (ItemDropSystem::WATCHER_KEY, [weakPtr = GetWeakPtr()](const std::vector<DropArtifactsUIData>& datas)
        {   
            if (auto thisPtr = weakPtr.lock())
            {
                ArtifactUIManager* thisManager = static_cast<ArtifactUIManager*>(thisPtr.get());
                thisManager->UpdateImageElements(datas);
            }
        });

        //UI 갱신
        if (ItemDropSystem* system = SingletonComponent<ItemDropSystem>::GetInstance())
        {
            system->NotifyUIModel();
        }
    }
}

void ArtifactUIManager::OnDestroy() 
{
    UmWatcher.Blind<DropArtifactsViewModel>(ItemDropSystem::WATCHER_KEY, _viewModelHandle);
}

void ArtifactUIManager::ImGuiDrawPropertysEvent() 
{
    if (ImGui::Button("Update Image"))
    {
        UpdateImageElements();
    }

    auto CheckContainer = [](auto& container, const char* lable, const char* helpMessage) 
    {
        if (container.empty())
        {
            ImGui::Text(helpMessage);
        }
        else
        {
            if (ImGui::TreeNodeEx(lable))
            {
                for (auto& imageElement : container)
                {
                    ImGui::PushID(&imageElement);
                    ImGui::Selectable(imageElement->gameObject->ToString().data());
                    ImGui::PopID();
                }
                ImGui::TreePop();
            }
        }
    };

    CheckContainer(_frameImageElements, "Frame Image Elements", (const char*)u8"Frame Image들을 찾을 수 없습니다.");
    CheckContainer(_iconElements, "Icon Elements", (const char*)u8"Icon Image들을 찾을 수 없습니다.");
    CheckContainer(_categoryImageElements, "Category Elements", (const char*)u8"Category Image들을 찾을 수 없습니다.");
    CheckContainer(_focusImageElements, "Focus Elements", (const char*)u8"Focus Image들을 찾을 수 없습니다.");
    CheckContainer(_focusNaviElements, "Focus Navi Elements", (const char*)u8"ArtifactNavi들을 찾을 수 없습니다.");
}

void ArtifactUIManager::FindImageElements() 
{
    //Grid 탐색
    for (int i = 0; i < transform->ChildCount; i++)
    {
        Transform* curr = transform->GetChild(i);
        if (_gridPanel == nullptr)
        {
            if (curr->gameObject->CompareTag("Grid Panel"))
            {
                _gridPanel = curr->gameObject->GetComponent<GridPanel>();
            }
        }
        else
        {
            break;
        }
    }

    if (nullptr == _gridPanel)
    {
        UmLogger.Log(LogLevel::LEVEL_WARNING, u8"보상 UI의 Grid Panel을 찾을 수 없습니다.");
    }

    //탐색
    _frameImageElements.clear();
    _iconElements.clear();
    _categoryImageElements.clear();
    _focusImageElements.clear();
    _focusNaviElements.clear();
    Transform::ForeachDFS(transform, [this](Transform* curr) 
    {
        GameObject& gameObject = curr->gameObject;
        if (gameObject.CompareTag("Frame"))
        {
            if (ImageElement* element = gameObject.GetComponent<ImageElement>())
            {
                _frameImageElements.push_back(element);
            }
        }
        else if (gameObject.CompareTag("Icon"))
        {
            if (ImageElement* element = gameObject.GetComponent<ImageElement>())
            {
                _iconElements.push_back(element);
            }
        }
        else if (gameObject.CompareTag("Category"))
        {
            if (ImageElement* element = gameObject.GetComponent<ImageElement>())
            {
                _categoryImageElements.push_back(element);
            }
        }
        else if (gameObject.CompareTag("Focus"))
        {
            if (ImageElement* element = gameObject.GetComponent<ImageElement>())
            {
                _focusImageElements.push_back(element);
            }
            if (ArtifactButtonNavi* navi = gameObject.GetComponent<ArtifactButtonNavi>())
            {
                navi->_buttonIndex = _focusNaviElements.size();              
                _focusNaviElements.push_back(navi);
            }
        }
    });
}

void ArtifactUIManager::ImageUISetup(const std::vector<DropArtifactsUIData>& dropItemsInfo) 
{
    if (ItemDropUIRootManager* rootManager = SingletonComponent<ItemDropUIRootManager>::GetInstance())
    {
        // Artifact UI 업데이트
        for (size_t i = 0; i < dropItemsInfo.size(); i++)
        {
            const DropArtifactsUIData& info         = dropItemsInfo[i];
            File::Guid                 categoryGuid = info.Category;
            File::Guid                 itemGuid     = info.Artifact;

            if (false == itemGuid.IsNull())
            {
                if (i < _iconElements.size())
                {
                    auto& imageElement = _iconElements[i];
                    imageElement->SetImage(itemGuid);
                }
            }

            if (false == categoryGuid.IsNull())
            {
                if (i < _categoryImageElements.size())
                {
                    auto& imageElement = _categoryImageElements[i];
                    imageElement->SetImage(categoryGuid);
                }
            }
        }
    }
}

void ArtifactUIManager::ImageUIUnlock() 
{
    auto GetIndices = [](int n)
    {
        if (n < 1)
        {
            return -1;
        }
        int startIndex = (n - 1) * 2;
        return startIndex + 1;
    };

    if (ItemDropSystem* system = SingletonComponent<ItemDropSystem>::GetInstance())
    {
        int clearCount = system->StageClearCount;
        int endIndex = GetIndices(clearCount);
        for (int i = 0; i < _iconElements.size(); ++i)
        {
            ImageElement* element = _iconElements[i];
            if (endIndex < i)
            {
                element->Enable = false;
            }
            else
            {
                element->Enable = true;
            }
        }
        for (int i = 0; i < _categoryImageElements.size(); ++i)
        {
            ImageElement* element = _categoryImageElements[i];
            if (endIndex < i)
            {
                element->Enable = true;
            }
            else
            {
                element->Enable = false;
            }       
        }
        for (int i = 0; i < _focusNaviElements.size(); ++i)
        {
            ArtifactButtonNavi* navi = _focusNaviElements[i];
            ImageElement*  iconImage = _iconElements[i];
            if (endIndex < i)
            {
                navi->Enable = false;
            }
            else
            {
                if (ItemDropSystem* dropSystem = SingletonComponent<ItemDropSystem>::GetInstance())
                {
                    if (false == dropSystem->IsObtainArtifact(i))
                    {
                        navi->Enable = true;
                        iconImage->Alpha = 1.0f;
                    }
                    else
                    {
                        navi->Enable = false;
                        iconImage->Alpha = 0.5f;
                    }
                }
            }       
        }
        for (size_t i = 0; i < _frameImageElements.size(); ++i)
        {
            ImageElement* frameImage = _frameImageElements[i];
            ImageElement* iconImage  = _iconElements[i];
            if (ItemDropSystem* dropSystem = SingletonComponent<ItemDropSystem>::GetInstance())
            {
                if (dropSystem->IsObtainArtifact(i))
                {
                    frameImage->SetImage(GetObtainFrameGuid());
                    iconImage->Alpha = 0.5f;
                }
            }
        }
    }
}

bool ArtifactUIManager::FocusNavi(size_t index)
{
    if (index < _focusNaviElements.size())
    {
        if (ItemDropSystem* system = SingletonComponent<ItemDropSystem>::GetInstance())
        {
            ArtifactButtonNavi* navi = _focusNaviElements[index];
            if (navi->Enable)
            {
                navi->Focus();
                return true;
            }
        }
    }
    return false;
}

void ArtifactUIManager::ObtainFocusNavi(size_t index) 
{
    if (index < _focusNaviElements.size())
    {
        ArtifactButtonNavi* navi = _focusNaviElements[index];
        navi->Enable             = false;
        _obtainFlag              = true;

        if (index < _frameImageElements.size())
        {
            ImageElement* frameImage = _frameImageElements[index];
            ImageElement* iconImage  = _iconElements[index];
            frameImage->SetImage(GetObtainFrameGuid());
            iconImage->Alpha = 0.5f;
        }

        if (ArtifactButtonNavi::GetLastFocusIndex() == index)
        {
            if (ItemDropUIRootManager* manager = SingletonComponent<ItemDropUIRootManager>::GetInstance())
            {
                manager->AutoFocus(false);
            }
        }

        if (ItemDropSystem* dropSystem = SingletonComponent<ItemDropSystem>::GetInstance())
        {
            dropSystem->SetObtainArtifact(index);
        }
    }
}

void ArtifactUIManager::SetNaviDropItemInfo(const DropItemInfo& info, size_t index) 
{
    if (index < _focusNaviElements.size())
    {
        ArtifactButtonNavi* navi = _focusNaviElements[index];
        navi->SettingItem(info);
    }
}

void ArtifactUIManager::UpdateImageElements(const std::vector<DropItemInfo>& dropItemsInfo) 
{
    std::vector<DropArtifactsUIData> uiDatas = DropArtifactsViewModel::ConvertData(dropItemsInfo);
    UpdateImageElements(uiDatas);
}

void ArtifactUIManager::UpdateImageElements(const std::vector<DropArtifactsUIData>& dropItemsInfo)
{
    FindImageElements();
    ImageUISetup(dropItemsInfo);
    ImageUIUnlock();
}

void ArtifactUIManager::UpdateUnlock() 
{
    FindImageElements();
    ImageUIUnlock();
}
