#include "pchScripts.h"
#include "MapPlayerHPView.h"
#include "UI/Elements/Image/ImageElement.h"
#include "UI/Elements/Text/TextElement.h"
#include "DLLExportDefine.h"

UMREAL_COMPONENT(MapPlayerHPView)

MapPlayerHPView::MapPlayerHPView()  = default;

MapPlayerHPView::~MapPlayerHPView()
{
    UmWatcher.Blind<MapPlayerHPViewModel>("PlayerHP", _handle);
}

void MapPlayerHPView::Awake()
{    
    try
    {
        if (_hp = GetComponent<TextElement>(); nullptr == _hp)
        {
            _hp = &AddComponent<TextElement>();
        }

        _handle = UmWatcher.Watch<MapPlayerHPViewModel, PlayerHP>("PlayerHP", [this](const PlayerHP value) {
            _hp->Text = std::format("{} / {}", value.CurrentHP, value.MaxHP);

            if (auto gage = GameObject::Find("Gage").lock(); gage)
            {
                if (auto component = gage->GetComponent<ImageElement>(); component)
                {
                    component->SetLinearFill(value.CurrentHP / (float)value.MaxHP);
                }
            }
        });
    }
    catch (const std::exception& e)
    {
        UmLogger.Log(LogLevel::LEVEL_ERROR, "Watch Failed.");
        UmLogger.Log(LogLevel::LEVEL_ERROR, e.what());
    }
}