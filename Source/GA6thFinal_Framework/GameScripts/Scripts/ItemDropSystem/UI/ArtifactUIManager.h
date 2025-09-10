#pragma once
#include "UmFramework.h"
#include "ItemDropSystem/Interface/IDropItem.h"
#include "ViewModels/ItemDrop/DropArtifacts/DropArtifactsViewModel.h"

class ImageElement;
class GridPanel;

class ArtifactUIManager : public Component
{
    USING_PROPERTY(ArtifactUIManager)
    inline static ArtifactUIManager* static_instance = nullptr;

public:
    inline static constexpr const char* TAG = "Artifact UI Manager";

    static ArtifactUIManager* GetInstance(std::source_location location = std::source_location::current()) 
    {
        if (static_instance)
        {
            if (false == static_instance->gameObject->IsValid())
            {
                static_instance = nullptr;
            }
        }
       
        if (nullptr == static_instance)
        {
            UmLogger.Log(LogLevel::LEVEL_WARNING, u8"ArtifactUIManager가 존재하지 않습니다.", location);
        }
        return static_instance;
    }
    ArtifactUIManager();
    ~ArtifactUIManager() override;

public:
    /// <summary>
    /// DropItemInfo에 따라 UI를 갱신합니다.
    /// </summary>
    void UpdateImageElements(const std::vector<DropItemInfo>& dropItemsInfo);

    /// <summary>
    /// DropArtifactsUIData에 따라 UI를 갱신합니다.
    /// </summary>
    void UpdateImageElements(const std::vector<DropArtifactsUIData>& dropItemsInfo = std::vector<DropArtifactsUIData>());

    /// <summary>
    /// 보상 해금 정보 UI를 갱신합니다.
    /// </summary>
    void UpdateUnlock();

public:
    REFLECT_PROPERTY()

protected:
    REFLECT_FIELDS_BEGIN(Component)
    REFLECT_FIELDS_END(ArtifactUIManager)

    void Reset() override;
    void Awake() override;
    void Start() override;

    void ImGuiDrawPropertysEvent() override;

    /// <summary>
    /// Image Element들을 찾아서 등록합니다.
    /// </summary>
    void FindImageElements();

    /// <summary>
    /// UI 정보에 맞게 UI 이미지를 갱신합니다.
    /// </summary>
    void ImageUISetup(const std::vector<DropArtifactsUIData>& dropItemsInfo);

    /// <summary>
    /// UI 클리어 횟수정보에 맞게 UI Active를 갱신합니다.
    /// </summary>
    void ImageUIUnlock();

private:
    GridPanel*                 _frameGridPanel;
    std::vector<ImageElement*> _frameImageElements;

    GridPanel*                 _gridPanel;
    std::vector<ImageElement*> _imageElements;

    GridPanel*                 _categoryGridPanel;
    std::vector<ImageElement*> _categoryImageElements;
};
