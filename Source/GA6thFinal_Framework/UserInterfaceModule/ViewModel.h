#pragma once

namespace MVVM
{
    template <typename T, typename U>
    class ViewModel
    {
    public:
        explicit ViewModel(Model<T>& model)
        {
            model.AddObserver([this](T value) {
                SetLastValue(value);
                Call();
            });
            model.Notify();
        }

        ViewModel(const ViewModel&)            = default;
        ViewModel& operator=(const ViewModel&) = default;
        ViewModel(ViewModel&&)                 = default;
        ViewModel& operator=(ViewModel&&)      = default;
        virtual ~ViewModel()                   = default;

        void SetCallback(const std::function<void(U)>& callback)
        {
            _callback = callback;
            Call();
        }

    protected:
        void Call()
        {
            if (nullptr != _callback)
                _callback(Convert(_lastValue));
        }

        void SetLastValue(T value)
        {
            _lastValue = value;
        }


        virtual U Convert(const T& value) = 0;

    private:
        std::function<void(U)> _callback;
        T                      _lastValue;
    };
} // namespace MVVM