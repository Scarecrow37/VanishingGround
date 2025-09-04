#include "pchScripts.h"
#include "ItemDropUIRootManager.h"

ItemDropUIRootManager::ItemDropUIRootManager()
{
    static_instance = this;
    ArtifactsUIFrameAsset.SetInputAutoEvent([this]() 
    {
        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload* payLoad = ImGui::AcceptDragDropPayload(DragDropAsset::KEY))
            {
                DragDropAsset::Data* data      = static_cast<DragDropAsset::Data*>(payLoad->Data);
                File::Path           path      = data->GetPath();
                const auto           extension = path.extension();
                if (extension == L".png")
                {
                    ReflectFields->ArtifactsUIFrameAssetGuid = data->GetGuid().string();
                }
            }
            ImGui::EndDragDropTarget();
        }
        ImGuiHelper::HoveredToolTip(u8"유물 드랍 프레임 UI 에셋 경로입니다.");
    });
}

ItemDropUIRootManager::~ItemDropUIRootManager()
{
    if (this == static_instance)
    {
        static_instance = nullptr;
    }
}

void ItemDropUIRootManager::DeserializedReflectEvent() 
{

}

void ItemDropUIRootManager::Reset() 
{
    
}

void ItemDropUIRootManager::Awake()
{
    if (this != static_instance)
    {
        UmLogger.Log(LogLevel::LEVEL_WARNING, u8"ItemDropUIRootManager는 하나만 존재해야 합니다.");
        GameObject::Destroy(this);
    }
    else
    {
        gameObject->AddTag(ItemDropUIRootManager::TAG);
        gameObject->ActiveSelf = false;
        Base::Awake();
    }
}

