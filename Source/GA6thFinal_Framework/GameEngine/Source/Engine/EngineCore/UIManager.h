#pragma once

    class UIComponent;
namespace UI
{

    class Manager
    {
    public:
        void AddMeasureQueue(const UIComponent* component);
        void AddArrangeQueue(const UIComponent* component);

        void Update(const SIZE& rootSize);

    private:
        void ProcessMeasureQueue(const SIZE& availableRootSize);
        void ProcessArrangeQueue(const SIZE& finalRootSize);

        void ClearQueues();

    private:
        std::deque<std::weak_ptr<UIComponent>> _measureQueue;
        std::deque<std::weak_ptr<UIComponent>> _arrangeQueue;
    };
} // namespace UI