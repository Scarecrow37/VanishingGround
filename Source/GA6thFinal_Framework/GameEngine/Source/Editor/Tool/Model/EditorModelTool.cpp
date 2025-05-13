#include "pch.h"
#include "EditorModelTool.h"
#include "../../DynamicCamera/EditorDynamicCamera.h"

EditorModelTool::EditorModelTool()
    : _camera(std::make_unique<EditorDynamicCamera>())
{
    SetLabel("ModelViewer");
}

void EditorModelTool::OnTickGui()
{
}

void EditorModelTool::OnStartGui()
{
    std::shared_ptr<Camera> camera = UmRenderer.GetCamera("ModelViewer");
    ASSERT((nullptr != camera), L"Camera is nullptr");
    _camera->SetTarget(camera);
}

void EditorModelTool::OnEndGui()
{
}

void EditorModelTool::OnPreFrameBegin()
{
    if (ImGui::Button("Import", ImVec2(100, 50)))
    {
        // FBX or binary Load
    }

    ImGuiWindow* window = ImGui::GetCurrentWindow();
    ImRect       rect   = window->Rect(); // 윈도우 전체 영역

    if (ImGui::BeginDragDropTargetCustom(rect, window->ID))
    {
        if (const ImGuiPayload* payLoad = ImGui::AcceptDragDropPayload(DragDropAsset::KEY))
        {
            DragDropAsset::Data* data    = (DragDropAsset::Data*)payLoad->Data;
            auto                 context = data->pContext->lock();

            if (nullptr != context)
            {
                int a = 0;
                /*const auto& path = context->GetPath();
                if (path.extension() == L".fbx")
                {
                    ReflectFields->Guid = path.ToGuid().string();
                    UmResourceManager.RegisterLoadQueue({path, RESOURCE_TYPE::MODEL});
                }*/
            }
        }

        ImGui::EndDragDropTarget();
    }
}

void EditorModelTool::OnPostFrameBegin()
{    
    auto handle = UmRenderer.GetRenderSceneImage("ModelViewer");

    ImVec2 size = ImGui::GetContentRegionAvail();

    ImGui::Image((ImTextureID)handle.ptr, size);
}

void EditorModelTool::OnFrameEnd()
{
}

void EditorModelTool::OnFrameFocused()
{
}

void EditorModelTool::OnFramePopupOpened()
{
}