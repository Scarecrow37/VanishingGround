#include "pchScripts.h"
#include "TooltipDescriptionPanel.h"
#include "Scripts/UI/Contents/TooltipElement.h"

UMREAL_COMPONENT(TooltipDescriptionPanel)

TooltipDescriptionPanel::TooltipDescriptionPanel() = default;

void TooltipDescriptionPanel::SetTooltip(const int id)
{
    SetTooltips(std::span(&id, 1));
}

void TooltipDescriptionPanel::SetTooltips(const std::initializer_list<int> ids)
{
    SetTooltips(std::span(ids));
}

void TooltipDescriptionPanel::SetTooltips(const std::span<const int> ids)
{
    ClearTooltips();

    for (int i = 0; i < ids.size(); ++i)
    {
        if (i < transform->GetChildCount())
        {
            if (const Transform* childTransform = transform->GetChild(i))
            {
                GameObject& childObject = childTransform->gameObject;
                childObject.SetActive(true);
                if (const TooltipElement* tooltip = childObject.GetComponentDynamic<TooltipElement>())
                {
                    tooltip->SetTooltip(ids[i]);
                }
            }
        }
        else
        {
            std::weak_ptr<TooltipElement> weakTooltip = CreateTooltipElements();

            if (const std::shared_ptr<TooltipElement> tooltip = weakTooltip.lock())
            {
                tooltip->gameObject->SetActive(true);
                tooltip->SetTooltip(ids[i]);
            }
        }
    }
}


struct DragDropEvent
{
    bool operator()(File::Guid& out) const
    {
        bool result = false;
        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload* payLoad = ImGui::AcceptDragDropPayload(DragDropAsset::KEY))
            {
                const DragDropAsset::Data* data = static_cast<DragDropAsset::Data*>(payLoad->Data);
                const File::Path&          path = data->GetPath();
                if (const File::Path extension = path.extension(); extension == L".UmPrefab")
                {
                    result = true;
                    out    = path.ToGuid();
                }
                else
                {
                    UmLogger.Log(LogLevel::LEVEL_WARNING,
                                 reinterpret_cast<const char*>(u8"프리팹은 .UmPrefab 파일만 지정할 수 있습니다."));
                }
            }
            ImGui::EndDragDropTarget();
        }
        return result;
    }
};

void TooltipDescriptionPanel::Reset()
{
    HorizontalPanel::Reset();

    TooltipElementPrefab.SetInputAutoEvent([this]() {
        if (File::Guid dragDropGuid; DragDropEvent()(dragDropGuid))
        {
            ReflectFields->TooltipElementPrefabGuid = dragDropGuid.string();
        }
    });
}

void TooltipDescriptionPanel::ImGuiDrawPropertysEvent()
{
    HorizontalPanel::ImGuiDrawPropertysEvent();

    static int tooltipId[4]{0, 0, 0, 0};

    ImGui::InputInt4("ID", tooltipId, 0);
    ImGui::SameLine();
    if (ImGui::Button("Set Tooltips"))
    {
        SetTooltips(tooltipId);
    }
}

std::weak_ptr<TooltipElement> TooltipDescriptionPanel::CreateTooltipElements()
{
    std::weak_ptr<TooltipElement> weakTooltip;
    const File::Guid              prefab(ReflectFields->TooltipElementPrefabGuid);
    if (const std::shared_ptr<GameObject> object = UmGameObjectFactory.DeserializeToGuid(prefab))
    {
        if (TooltipElement* tooltip = object->GetComponentDynamic<TooltipElement>())
        {
            object->transform->SetParent(transform);
            tooltip->FindComponents();
            weakTooltip = tooltip->GetWeakPtrAs<TooltipElement>();
        }
        else
        {
            GameObject::Destroy(object.get());
            UmLogger.Log(LogLevel::LEVEL_WARNING,
                         reinterpret_cast<const char*>(u8"TooltipElement 프리팹이 올바르지 않습니다."));
        }
    }
    return weakTooltip;
}

void TooltipDescriptionPanel::ClearTooltips() const
{
    Transform::ForeachBFS(transform, [](const Transform* childTransform, const int depth) {
        if (depth == 1)
        {
            childTransform->gameObject->SetActive(false);
        }
    });
}
