#include "pch.h"
#include "UIManager.h"


void UI::Manager::AddMeasureQueue(const UIComponent* component)
{
    const std::shared_ptr<UIComponent> uiComponent =
        std::static_pointer_cast<UIComponent>(component->GetWeakPtr().lock());
    _measureQueue.push_back(uiComponent);
}

void UI::Manager::AddArrangeQueue(const UIComponent* component)
{
    const std::shared_ptr<UIComponent> uiComponent =
        std::static_pointer_cast<UIComponent>(component->GetWeakPtr().lock());
    _arrangeQueue.push_back(uiComponent);
}

void UI::Manager::Update(const SIZE& rootSize)
{
    ProcessMeasureQueue(rootSize);
    ProcessArrangeQueue(rootSize);
}

void UI::Manager::ProcessMeasureQueue(const SIZE& availableRootSize)
{
    while (false == _measureQueue.empty())
    {
        if (const auto& element = _measureQueue.front().lock(); element != nullptr)
        {
            if (const UIComponent* parent = element->Parent; nullptr == parent)
            {
                if (const bool isMeasureDirty = element->IsMeasureDirty; true == isMeasureDirty)
                {
                    element->Measure(availableRootSize);
                }
            }
        }
        _measureQueue.pop_front();
    }
    _measureQueue.clear();
}

void UI::Manager::ProcessArrangeQueue(const SIZE& finalRootSize)
{
    while (false == _arrangeQueue.empty())
    {
        if (const auto& element = _arrangeQueue.front().lock(); element != nullptr)
        {
            if (UIComponent* parent = element->Parent; nullptr == parent)
            {
                if (const bool isArrangeDirty = element->IsArrangeDirty; true == isArrangeDirty)
                {
                    constexpr POINT zero = {.x = 0, .y = 0};
                    element->Arrange(zero, finalRootSize);
                }
            }
            else if (const bool isParentArrangeDirty = parent->IsArrangeDirty; false == isParentArrangeDirty)
            {
                parent->Arrange();
            }
        }
        _arrangeQueue.pop_front();
    }
    _arrangeQueue.clear();
}

void UI::Manager::ClearQueues()
{
    _measureQueue.clear();
    _arrangeQueue.clear();
}