#pragma once

namespace MVVM
{
    template <typename T, typename U>
    class ViewModel
    {
    public:
        explicit ViewModel(Model<T>& model) : _callback(nullptr)
        {
            model.AddObserver([this](T value) {
                if (nullptr != _callback)
                    _callback(Convert(value));
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
        }

    protected:

        virtual U Convert(const T& value) = 0;

    private:
        std::function<void(U)> _callback;
    };
} // namespace MVVM