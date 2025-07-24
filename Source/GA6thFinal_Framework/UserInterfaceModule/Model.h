#pragma once

namespace MVVM
{
    template <typename T>
    class Model
    {
        using Callback = std::function<void(T)>;

    public:
        Model() : _value(T()) {}

        explicit Model(T value) : _value(value) {}
        Model(const Model&)                = delete;
        Model& operator=(const Model&)     = delete;
        Model(Model&&) noexcept            = default;
        Model& operator=(Model&&) noexcept = default;
        virtual ~Model()                   = default;

        T Get() const { return _value; }

        void Set(T value)
        {
            _value = value;
            Notify();
        }

        void AddObserver(const Callback& observer) { _observers.push_back(observer); }

        void Notify() const
        {
            for (const auto& observer : _observers)
            {
                observer(_value);
            }
        }

    private:
        std::vector<Callback> _observers;
        T _value;
    };
} // namespace MVVM