#include "pchScripts.h"
#include "Stage.h"
#include "ViewModels/Map/StageViewModel.h"
#include "DLLExportDefine.h"

UMREAL_COMPONENT(Stage)

Stage::Stage()
{
    StagePath.SetInputAutoEvent([this]() {
        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload* payLoad = ImGui::AcceptDragDropPayload(DragDropAsset::KEY))
            {
                DragDropAsset::Data* data      = static_cast<DragDropAsset::Data*>(payLoad->Data);
                File::Path           path      = data->GetPath();
                const auto           extension = path.extension();

                if (extension == L".UmScene")
                {
                    ReflectFields->StagePath = UmFileSystem.GetGuidFromPath(path).string();
                }
            }
            ImGui::EndDragDropTarget();
        }
    });
}

Stage::~Stage()
{
    UmWatcher.Unregister<StageViewModel>(_key);
}

void Stage::DeserializedReflectEvent()
{
    _stageEnable.Set(ReflectFields->Enable);
}

void Stage::RegisterStage(const std::string& key, const File::Guid& enableImage, const File::Guid& disableImage)
{
    UmWatcher.Register<StageViewModel>(key, _stageEnable, enableImage, disableImage);
}

void Stage::UpdateData(const std::string& key, const File::Guid& enableImage, const File::Guid& disableImage)
{
    UmWatcher.Unregister<StageViewModel>(key);
    UmWatcher.Register<StageViewModel>(key, _stageEnable, enableImage, disableImage);
    _key = key;
    SetupStage();
}

void Stage::ImGuiDrawPropertysEvent()
{
    if (ImGui::Checkbox("EnableStage", &ReflectFields->Enable))
    {
        _stageEnable = ReflectFields->Enable;
    }
}

void Stage::SetupStage()
{
    static const std::regex rx(R"(^\d+-\d+$)");
    if (std::regex_match(ReflectFields->Stage, rx))
    {
        int consumed = 0;
        std::sscanf(ReflectFields->Stage.c_str(), "%d-%d%n", &_first, &_second, &consumed) == 2 &&
            consumed == static_cast<int>(ReflectFields->Stage.size());
    }
}