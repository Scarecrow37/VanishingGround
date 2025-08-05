#include "pch.h"
#include "RendererFileEvent.h"
#include "Engine/GraphicsCore/RenderPassDataHelper.h"

void RendererFileEvent::OnPostRequestedSave()
{
    auto filePath = UmFileSystem.GetBuildSettingPath();
    filePath /= "GraphicsSetting.inl";

    SaveRenderPassData(filePath.string());
}

void RendererFileEvent::OnPostRequestedLoad()
{
    auto filePath = UmFileSystem.GetBuildSettingPath();
    filePath /= "GraphicsSetting.inl";

    LoadRenderPassData(filePath.string());
}
