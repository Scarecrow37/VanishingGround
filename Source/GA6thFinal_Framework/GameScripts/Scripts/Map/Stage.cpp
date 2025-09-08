#include "pchScripts.h"
#include "Stage.h"
#include "Map/ViewModels/StageViewModel.h"

Stage::Stage() = default;
Stage::~Stage()
{
    UmWatcher.Unregister<StageViewModel>(ReflectFields->Key);
}

void Stage::DeserializedReflectEvent()
{
    _stageEnable.Set(ReflectFields->Enable);
}

void Stage::RegisterStage(const std::string& key, const File::Guid& enableImage, const File::Guid& disableImage)
{
    UmWatcher.Register<StageViewModel>(key, _stageEnable, enableImage, disableImage);
    ReflectFields->Key = key;
}

void Stage::UpdateData(const std::string& key, const File::Guid& enableImage, const File::Guid& disableImage)
{
    UmWatcher.Unregister<StageViewModel>(ReflectFields->Key);
    UmWatcher.Register<StageViewModel>(key, _stageEnable, enableImage, disableImage);
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
        std::sscanf(ReflectFields->Stage.c_str(), "%d - %d %n", &_first, &_second, &consumed) == 2 &&
            consumed == static_cast<int>(ReflectFields->Stage.size());
    }
}