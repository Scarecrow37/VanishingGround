#pragma once
#include "UmFramework.h"
#include "ItemDropSystem/Interface/IDropItem.h"
#include "ViewModels/ItemDrop/DropArtifacts/DropArtifactsViewModel.h"
#include "Utility/SingletonHelper.h"

class ImageElement;
class GridPanel;
class ArtifactButtonNavi;

class ArtifactUIManager : public Component
{
    USING_PROPERTY(ArtifactUIManager)
    static File::Guid GetObtainFrameGuid();

public:
    inline static constexpr const char* TAG = "Artifact UI Manager";
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

    /// <summary>
    /// 포커싱할 버튼을 인덱스를 통해 설정합니다. 활성화된 버튼만 포커스 할 수 있습니다.
    /// </summary>
    /// <param name="index :">포커스할 버튼의 Index</param>
    /// <returns>성공시 true</returns>
    bool FocusNavi(size_t index);

    /// <summary>
    /// 전달받은 인덱스의 보상 버튼을 사용 완료된 버튼으로 설정합니다.
    /// </summary>
    /// <param name="index"></param>
    void ObtainFocusNavi(size_t index);

    /// <summary>
    /// 보상 획득 여부 플래그 입니다. ResetObtainFlag로 초기화 할 수 있습니다.
    /// </summary>
    /// <returns></returns>
    bool IsObtainActive() const { return _obtainFlag; }
    void ResetObtainFlag() { _obtainFlag = false; }

    /// <summary>
    /// Navi의 아이템 정보를 설정합니다. 해당 정보를 기준으로 버튼에 동작이 달라집니다.
    /// </summary>
    /// <param name="info :">사용할 아이템 정보</param>
    /// /// <param name="index :">버튼의 Index</param>
    void SetNaviDropItemInfo(const DropItemInfo& info, size_t index);


public:
    REFLECT_PROPERTY()

protected:
    REFLECT_FIELDS_BEGIN(Component)
    REFLECT_FIELDS_END(ArtifactUIManager)

    void Reset() override;
    void Awake() override;
    void Start() override;
    void OnDestroy() override;

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
    SingletonComponent<ArtifactUIManager> _singletonComponent{this};

    GridPanel*                       _gridPanel;
    std::vector<ImageElement*>       _iconElements;
    std::vector<ImageElement*>       _frameImageElements;
    std::vector<ImageElement*>       _categoryImageElements;
    std::vector<ImageElement*>       _focusImageElements;
    std::vector<ArtifactButtonNavi*> _focusNaviElements;

    DropArtifactsViewModel::Handle _viewModelHandle;

    bool _obtainFlag;

private:
    inline static constexpr float OBTAIN_ICON_ALPHA = 0.4f;

};
