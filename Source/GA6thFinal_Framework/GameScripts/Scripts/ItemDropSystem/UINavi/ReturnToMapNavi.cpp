#include "pchScripts.h"
#include "ReturnToMapNavi.h"

UMREAL_COMPONENT(ReturnToMapNavi)

ReturnToMapNavi::ReturnToMapNavi()
{
    MapScene.SetInputAutoEvent([this]() {
        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload* payLoad = ImGui::AcceptDragDropPayload(DragDropAsset::KEY))
            {

                const DragDropAsset::Data* data = static_cast<DragDropAsset::Data*>(payLoad->Data);
                if (const auto extension = data->GetPath().extension(); extension == L".UmScene")
                {
                    _guidRef                = data->GetGuid();
                    ReflectFields->MapScene = _guidRef.string();
                }
            }
            ImGui::EndDragDropTarget();
        }
    });
}

void ReturnToMapNavi::Submit() 
{
    const File::Path path = _guidRef.ToPath();
    UmSceneManager.LoadScene(path.string());
}

void ReturnToMapNavi::DeserializedReflectEvent()
{
    UINavigationComponent::DeserializedReflectEvent();

    const File::Guid guid = ReflectFields->MapScene;
    if (const auto path = guid.ToPath(); !path.IsNull())
    {
        _guidRef = path.ToGuid();
    }
}