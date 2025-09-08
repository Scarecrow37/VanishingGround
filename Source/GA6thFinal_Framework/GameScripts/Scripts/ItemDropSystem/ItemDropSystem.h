#pragma once
#include "UmFramework.h"
#include "Interface/IDropItem.h"

class ItemDropSystem : public Component
{
    USING_PROPERTY(ItemDropSystem)
    inline static ItemDropSystem* static_instance = nullptr;

public:
    static ItemDropSystem* GetInstance(std::source_location location = std::source_location::current()) 
    { 
        if (nullptr == static_instance)
        {
            UmLogger.Log(LogLevel::LEVEL_WARNING, u8"ItemDropSystem가 존재하지 않습니다.", location);
        }
        return static_instance; 
    }

    ItemDropSystem();
    ~ItemDropSystem() override;

public:
    /// <summary>
    /// Artifact 아이템들을 랜덤으로 뽑아서 정보를 반환합니다.
    /// </summary>
    std::vector<DropItemInfo> RollArtifacts();
    
    /// <summary>
    /// 현재 보상 아이템을 설정합니다. UI도 갱신됩니다.
    /// </summary>
    /// <param name="itemInfos :"></param>
    void SetDropItem(const std::vector<DropItemInfo>& itemInfos);

public:
    REFLECT_PROPERTY()

protected:
    REFLECT_FIELDS_BEGIN(Component)

    REFLECT_FIELDS_END(ItemDropSystem)

protected:
    void ImGuiDrawPropertysEvent() override;
    void Awake() override;

private:
    MVVM::Model<std::vector<DropItemInfo>> _dropItemsModel;

};
