#include "pchScripts.h"
#include "StageView.h"
#include "UI/Elements/Image/ImageElement.h"

StageView::StageView() = default;
StageView::~StageView() = default;

void StageView::Watch(const std::string& key)
{
    if (false == key.empty())
    {
        UmWatcher.Blind<StageViewModel>(key, _handle);
        try
        {
            _handle = UmWatcher.Watch<StageViewModel, File::Guid>(key, [this](const File::Guid value) { GetComponent<ImageElement>()->SetImage(value); });
        }
        catch (const std::exception& e)
        {
            UmLogger.Log(LogLevel::LEVEL_ERROR, "Watch Failed.");
            UmLogger.Log(LogLevel::LEVEL_ERROR, e.what());
        }
    }
    else
    {
        UmLogger.Log(LogLevel::LEVEL_ERROR, "StageView: WatchKey is empty.");
    }
}