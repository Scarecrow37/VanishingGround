#pragma once
#include "UmFramework.h"
#include "ItemDropSystem/Interface/IDropItem.h"
#include "Utility/SingletonHelper.h"

class ArtifactUIManager;
class ItemInfoUIManager;
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
    // 보상 UI의 Artifact 부분을 관리하는 컴포넌트입니다.
    // type : ArtifactUIManager*
    PROPERTY(ArtifactUI)

    GETTER_ONLY(ItemInfoUIManager*, ItemInfoUI) 
    { 
        ItemInfoUIManager* infoUI = nullptr;
        if (auto uiManager = _itemInfoUIManager.lock())
        {
            infoUI = uiManager.get();
        }
        return infoUI;
    }
    // 보상 UI의 포커스된 아이템 정보를 표시하는 UI를 관리하는 컴포넌트입니다.
    // type : ItemInfoUIManager*
    PROPERTY(ItemInfoUI)

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
    std::weak_ptr<ItemInfoUIManager>          _itemInfoUIManager;

};
