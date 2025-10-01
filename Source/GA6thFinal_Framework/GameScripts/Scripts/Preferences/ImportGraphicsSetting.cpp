#include "pchScripts.h"
#include "ImportGraphicsSetting.h"
#include "Engine/GraphicsCore/RenderPassDataHelper.h"

UMREAL_COMPONENT(ImportGraphicsSetting)

ImportGraphicsSetting::ImportGraphicsSetting()
{
    FilePath.SetInputAutoEvent([this]() {
        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload* payLoad = ImGui::AcceptDragDropPayload(DragDropAsset::KEY))
            {
                const DragDropAsset::Data* data = static_cast<DragDropAsset::Data*>(payLoad->Data);
                File::Path                 path = data->GetPath();
                if (const auto extension = path.extension(); extension == L".inl")
                {
                    ReflectFields->Guid = data->GetGuid().string();
                }
            }
            ImGui::EndDragDropTarget();
        }
    });
}

ImportGraphicsSetting::~ImportGraphicsSetting() = default;

void ImportGraphicsSetting::Reset()
{
    if (!ReflectFields->Guid.empty())
    {
        LoadRenderPassData(UmFileSystem.GetPathFromGuid(ReflectFields->Guid).string());
    }
}