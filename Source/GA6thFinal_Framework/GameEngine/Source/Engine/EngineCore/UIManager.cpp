#include "pch.h"
#include "UIManager.h"


void UI::Manager::AddMeasureQueue(UIComponent* component)
{
    _measureQueue.push_back(component);
}

void UI::Manager::AddArrangeQueue(UIComponent* component)
{
    _arrangeQueue.push_back(component);
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
        const auto& element = _measureQueue.front();
        if (const UIComponent* parent = element->Parent; nullptr == parent)
        {
            if (const bool isMeasureDirty = element->IsMeasureDirty; true == isMeasureDirty)
            {
                element->Measure(availableRootSize);
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
        const auto& element = _arrangeQueue.front();
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
        _arrangeQueue.pop_front();
    }
    _arrangeQueue.clear();
}

void UI::Manager::ClearQueues()
{
    _measureQueue.clear();
    _arrangeQueue.clear();
}