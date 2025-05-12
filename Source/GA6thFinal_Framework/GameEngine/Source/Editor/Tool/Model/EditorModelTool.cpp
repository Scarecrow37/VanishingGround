#include "pch.h"
#include "EditorModelTool.h"
#include "Editor/DynamicCamera/EditorDynamicCamera.h"
#include "Engine/GraphicsCore/MeshRenderer.h"

EditorModelTool::EditorModelTool()
    : _camera(std::make_unique<EditorDynamicCamera>())
    , _meshRenderer(std::make_unique<MeshRenderer>(MeshRenderer::RENDER_TYPE::STATIC, _worldMatrix))
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

void EditorModelTool::OnPreFrame()
{
    if (ImGui::Button("Import", ImVec2(100, 50)))
    {
        // FBX or binary Load
        TCHAR      filter[] = L"Model File (.fbx, .UmModel)\0*.fbx;*.UmModel\0";
        File::Path out;
        if (File::OpenFileNameBrowser(filter, out))
        {
            //UmFileSystem.LoadProject(out);
            ImportFBX(out);
        }
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

void EditorModelTool::OnFrame()
{    
    auto handle = UmRenderer.GetRenderSceneImage("ModelViewer");

    ImVec2 size = ImGui::GetContentRegionAvail();

    ImGui::Image((ImTextureID)handle.ptr, size);
}

void EditorModelTool::OnPostFrame()
{
}

void EditorModelTool::OnFocus()
{
}

void EditorModelTool::OnPopup()
{
}

void EditorModelTool::ImportFBX(std::filesystem::path path)
{
    _meshRenderer->LoadModel(path.c_str());
}

void EditorModelTool::ExportFBX()
{
}