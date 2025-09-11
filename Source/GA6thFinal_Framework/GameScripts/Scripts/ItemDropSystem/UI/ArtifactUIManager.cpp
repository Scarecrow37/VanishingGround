#include "pchScripts.h"
#include "ArtifactUIManager.h"
#include "UI/Panels/Grid/GridPanel.h"
#include "UI/Elements/Image/ImageElement.h"
#include "ItemDropSystem/UI/ItemDropUIRootManager.h"
#include "ItemDropSystem/ItemDropSystem.h"
#include "ViewModels/ItemDrop/DropArtifacts/DropArtifactsViewModel.h"

ArtifactUIManager::ArtifactUIManager()
{

}

ArtifactUIManager::~ArtifactUIManager()
{
    if (this == static_instance)
    {
        static_instance = nullptr;
    }
}

void ArtifactUIManager::Reset() 
{
    static_instance    = this;
    _frameGridPanel    = nullptr;
    _gridPanel         = nullptr;
    _categoryGridPanel = nullptr;
    Base::Reset();
}

void ArtifactUIManager::Awake() 
{
    if (this != static_instance)
    {
        UmLogger.Log(LogLevel::LEVEL_WARNING, u8"ArtifactFrameUIManager는 하나만 존재해야 합니다.");
        GameObject::Destroy(this);
    }
    else
    {
        gameObject->AddTag(TAG);
        Base::Awake();
    }
}

void ArtifactUIManager::Start() 
{
    _viewModleHandle = UmWatcher.Watch<DropArtifactsViewModel, std::vector<DropArtifactsUIData>>
    (ItemDropSystem::WATCHER_KEY, [weakPtr = GetWeakPtr()](const std::vector<DropArtifactsUIData>& datas)
    {   
        if (auto thisPtr = weakPtr.lock())
        {
            ArtifactUIManager* thisManager = static_cast<ArtifactUIManager*>(thisPtr.get());
            thisManager->UpdateImageElements(datas);
        }
    });
}

void ArtifactUIManager::OnDestroy() 
{
    UmWatcher.Blind<DropArtifactsViewModel>(ItemDropSystem::WATCHER_KEY, _viewModleHandle);
}

void ArtifactUIManager::ImGuiDrawPropertysEvent() 
{
    if (ImGui::Button("Update Image"))
    {
        UpdateImageElements();
    }

    if (_frameImageElements.empty())
    {
        ImGui::Text((const char*)u8"Frame Grid Panel에 \"Frame Grid Panel\" Tag를 추가해주세요");
    }
    else
    {
        if (ImGui::TreeNodeEx("Frame Grid Panel"))
        {
            for (auto& imageElement : _frameImageElements)
            {
                ImGui::PushID(&imageElement);
                ImGui::Selectable(imageElement->gameObject->ToString().data()); 
                ImGui::PopID();
            }
            ImGui::TreePop();
        }
    }

    if (_imageElements.empty())
    {
        ImGui::Text((const char*)u8"Grid Panel에 \"Grid Panel\" Tag를 추가해주세요");
    }
    else
    {
        if (ImGui::TreeNodeEx("Grid Panel"))
        {
            for (auto& imageElement : _imageElements)
            {
                ImGui::PushID(&imageElement);
                ImGui::Selectable(imageElement->gameObject->ToString().data());
                ImGui::PopID();
            }
            ImGui::TreePop();
        }
    }

    if (_categoryImageElements.empty())
    {
        ImGui::Text((const char*)u8"Category용 Grid Panel에 \"Category Grid Panel\" Tag를 추가해주세요");
    }
    else
    {
        if (ImGui::TreeNodeEx("Category Grid Panel"))
        {
            for (auto& imageElement : _categoryImageElements)
            {
                ImGui::PushID(&imageElement);
                ImGui::Selectable(imageElement->gameObject->ToString().data());
                ImGui::PopID();
            }
            ImGui::TreePop();
        }
    }
}

void ArtifactUIManager::FindImageElements() 
{
    //frame 탐색
    _frameImageElements.clear();
    _frameGridPanel = nullptr;
    Transform::ForeachBFS(transform, 
    [this](Transform* curr) 
    {
        if (_frameGridPanel == nullptr)
        {
            if (curr->gameObject->CompareTag("Frame Grid Panel"))
            {
                _frameGridPanel = curr->gameObject->GetComponent<GridPanel>();
            }
        }
    });

    if (_frameGridPanel)
    {
        Transform& tr = _frameGridPanel->transform;
        for (int i = 0; i < tr.ChildCount; i++)
        {
            Transform* child = tr.GetChild(i);
            if (child)
            {
                ImageElement* frameImage = child->gameObject->GetComponent<ImageElement>();
                if (nullptr == frameImage)
                {
                    child = child->GetChild(0);
                    if (child)
                    {
                        frameImage = child->gameObject->GetComponent<ImageElement>();
                    }
                }

                if (nullptr != frameImage)
                {
                    _frameImageElements.push_back(frameImage);
                }
            }
        }
    }

    //image 탐색
    _imageElements.clear();
    _gridPanel = nullptr;
    Transform::ForeachBFS(transform, [this](Transform* curr) 
    {
        if (nullptr == _gridPanel)
        {
            if (curr->gameObject->CompareTag("Grid Panel"))
            {
                _gridPanel = curr->gameObject->GetComponent<GridPanel>();
            }
        }
    });

    if (_gridPanel)
    {
        Transform& tr = _gridPanel->transform;
        for (int i = 0; i < tr.ChildCount; i++)
        {
            Transform* child = tr.GetChild(i);
            if (child)
            {
                ImageElement* image = child->gameObject->GetComponent<ImageElement>();
                if (nullptr == image)
                {
                    child = child->GetChild(0);
                    if (child)
                    {
                        image = child->gameObject->GetComponent<ImageElement>();
                    }
                }

                if (nullptr != image)
                {
                    _imageElements.push_back(image);
                }
            }
        }
    }
    
    //category 탐색
    _categoryImageElements.clear();
    _categoryGridPanel = nullptr;
    Transform::ForeachBFS(transform, [this](Transform* curr)
    {
        if (nullptr == _categoryGridPanel)
        {
            if (curr->gameObject->CompareTag("Category Grid Panel"))
            {
                _categoryGridPanel = curr->gameObject->GetComponent<GridPanel>();
            }
        }
    });

    if (_categoryGridPanel)
    {
        Transform& tr = _categoryGridPanel->transform;
        for (int i = 0; i < tr.ChildCount; i++)
        {
            Transform* child = tr.GetChild(i);
            if (child)
            {
                ImageElement* categoryImage = child->gameObject->GetComponent<ImageElement>();
                if (nullptr == categoryImage)
                {
                    child = child->GetChild(0);
                    if (child)
                    {
                        categoryImage = child->gameObject->GetComponent<ImageElement>();
                    }
                }

                if (nullptr != categoryImage)
                {
                    _categoryImageElements.push_back(categoryImage);
                }
            }
        }
    }
}

void ArtifactUIManager::ImageUISetup(const std::vector<DropArtifactsUIData>& dropItemsInfo) 
{
    if (ItemDropUIRootManager* rootManager = ItemDropUIRootManager::GetInstance())
    {
        // Frame UI 업데이트
        std::string   frameAsstePath = rootManager->ArtifactsUIFrameAsset;
        File::GuidRef guid           = UmFileSystem.GetGuidFromPath(frameAsstePath);
        if (false == guid.IsNull())
        {
            for (auto& imageElement : _frameImageElements)
            {
                imageElement->SetImage(guid);
            }
        }

        // Artifact UI 업데이트
        for (size_t i = 0; i < dropItemsInfo.size(); i++)
        {
            const DropArtifactsUIData& info         = dropItemsInfo[i];
            File::Guid                 categoryGuid = info.Category;
            File::Guid                 itemGuid     = info.Artifact;

            if (false == itemGuid.IsNull())
            {
                if (i < _imageElements.size())
                {
                    auto& imageElement = _imageElements[i];
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

    if (ItemDropSystem* system = ItemDropSystem::GetInstance())
    {
        int clearCount = system->StageClearCount;
        int endIndex = GetIndices(clearCount);
        for (int i = 0; i < _imageElements.size(); ++i)
        {
            ImageElement* element = _imageElements[i];
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
