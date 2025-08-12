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

    for (const auto& element : _measureQueue)
    {
        if (const UIComponent* parent = element->Parent; nullptr == parent)
        {
            if (const bool isMeasureDirty = element->IsMeasureDirty; true == isMeasureDirty)
            {
                element->Measure(availableRootSize);
            }
        }
    }
    _measureQueue.clear();
}

void UI::Manager::ProcessArrangeQueue(const SIZE& finalRootSize)
{
    for (const auto& element : _arrangeQueue)
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
    _arrangeQueue.clear();
}

void UI::Manager::ClearQueues()
{
    _measureQueue.clear();
    _arrangeQueue.clear();
}