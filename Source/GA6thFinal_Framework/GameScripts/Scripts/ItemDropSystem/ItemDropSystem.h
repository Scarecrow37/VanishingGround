#pragma once
#include "UmFramework.h"
class IDropItem;

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
    /// Artifact 아이템들을 랜덤으로 뽑아서 UI에 출력합니다.
    /// </summary>
    void RollArtifacts();

public:
    REFLECT_PROPERTY()

protected:
    REFLECT_FIELDS_BEGIN(Component)

    REFLECT_FIELDS_END(ItemDropSystem)

protected:
    void ImGuiDrawPropertysEvent() override;
    void Awake() override;

private:
    MVVM::Model<std::vector<IDropItem*>> _dropItemsModel;
    std::vector<IDropItem*> _dropItems;

};
