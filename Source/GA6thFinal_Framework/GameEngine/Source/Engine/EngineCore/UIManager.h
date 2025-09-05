#pragma once

    class UIComponent;
namespace UI
{

    class Manager
    {
    public:
        void AddMeasureQueue(const std::weak_ptr<UIComponent>& component);
        void AddArrangeQueue(const std::weak_ptr<UIComponent>& component);

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