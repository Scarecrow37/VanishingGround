#include "pchScripts.h"
#include "ProclamationHUD.h"
#include "UI/Panels/Description/DescriptionPanel.h"

UMREAL_COMPONENT(ProclamationHUD)

ProclamationHUD::ProclamationHUD() = default;

ProclamationHUD::~ProclamationHUD() = default;

void ProclamationHUD::SetDescriptionText(int assetID, int damage, int count)
{
    if (_descriptionPanel)
    {
        static const char* COLOR_MAP[]        = {"#A63636", "#8BB371", "#E6AC66", "#A977BE", "#98C5D0", "#C5C5C5"};
        constexpr int      COLOR_INDEX_OFFSET = 450060;

        const int colorIndex = assetID - COLOR_INDEX_OFFSET;

        if (damage > 0 && count > 0)
        {
            _descriptionText =
                std::format(R"(<Description><Image asset="{}"/><Text color="{}">{}x{}</Text></Description>)", assetID,
                            COLOR_MAP[colorIndex], damage, count);
        }
        else
        {
            _descriptionText = std::format(R"(<Description><Image asset="{}"/></Description>)", assetID);
        }

        _descriptionPanel->Description = _descriptionText;
    }
}

void ProclamationHUD::Awake()
{
    Component::Awake();

    Transform::ForeachBFS(transform, [this](Transform* tr)
    {
        GameObject& object = tr->gameObject;
        if (object.CompareTag("Description"))
        {
            if (DescriptionPanel* descriptionPanel = object.GetComponent<DescriptionPanel>())
            {
                _descriptionPanel = descriptionPanel;
            }
        }
    });
}