#pragma once

    class UIComponent;
namespace UI
{

    class Manager
    {
    public:
        void AddMeasureQueue(UIComponent* component);
        void AddArrangeQueue(UIComponent* component);

        void Update(const SIZE& rootSize);

    private:
        void ProcessMeasureQueue(const SIZE& availableRootSize);
        void ProcessArrangeQueue(const SIZE& finalRootSize);

        void ClearQueues();

    private:
        std::deque<UIComponent*> _measureQueue;
        std::deque<UIComponent*> _arrangeQueue;
    };
} // namespace UI