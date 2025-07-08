#pragma once

namespace UI
{
    template <typename T>
    class IModel
    {
    public:
        void AddObserver(const std::function<void(T)>& observer) { _observer.push_back(observer); }

    protected:
        void Notify(T value) const
        {
            for (const auto& observer : _observer)
            {
                observer(value);
            }
        }

    private:
        std::vector<std::function<void(T)>> _observer;
    };

    template <typename T>
    class Model : public IModel<T>
    {
    public:
        Model() : _value(T()) {}

        explicit Model(T value) : _value(value) {}

        T Get() const { return _value; }

        void Set(T value) { _value = value; }

    private:
        T _value;
    };
} // namespace UI