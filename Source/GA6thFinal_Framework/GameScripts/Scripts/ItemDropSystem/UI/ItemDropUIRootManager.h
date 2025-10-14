#pragma once
#include "UmFramework.h"
#include "ItemDropSystem/Interface/IDropItem.h"
#include "Utility/SingletonHelper.h"

class ArtifactUIManager;
class ItemDropUIRootManager : public Component
{
    USING_PROPERTY(ItemDropUIRootManager)
public:
    inline static constexpr const char* TAG = "Item Drop UI Root";
    ItemDropUIRootManager();
    ~ItemDropUIRootManager() override;

public:
    GETTER_ONLY(ArtifactUIManager*, ArtifactUI) 
    { 
        ArtifactUIManager* artifactUI = nullptr;
        if (auto uiManager = _artifactUIManager.lock())
        {
            artifactUI = uiManager.get();
        }
        return artifactUI;
    }
    /// <summary>
    /// 보상 UI의 Artifact 부분을 관리하는 컴포넌트입니다.
    /// type : ArtifactUIManager*
    /// </summary>
    PROPERTY(ArtifactUI)

public:
    REFLECT_PROPERTY(
    )

protected:
    REFLECT_FIELDS_BEGIN(Component)
    REFLECT_FIELDS_END(ItemDropUIRootManager)

    void DeserializedReflectEvent() override;
    void ImGuiDrawPropertysEvent() override;

    void Reset() override;
    void Awake() override;
    void Start() override;

private:
    SingletonComponent<ItemDropUIRootManager> _singletonComponent{this};
    std::weak_ptr<ArtifactUIManager>          _artifactUIManager;
};
