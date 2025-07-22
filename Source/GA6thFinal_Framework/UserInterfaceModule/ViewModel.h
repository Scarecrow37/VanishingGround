#pragma once

namespace MVVM
{
    template <typename T, typename U>
    class ViewModel
    {
        using ObserverToken = typename IModel<T>::Token;

    public:
        explicit ViewModel(IModel<T>& model)
            : _token(model.AddObserver([this](T value) {
                  if (_callback)
                      _callback(Convert(value));
              }))
        {
        }

        ViewModel(const ViewModel&)            = default;
        ViewModel& operator=(const ViewModel&) = default;
        ViewModel(ViewModel&&)                 = default;
        ViewModel& operator=(ViewModel&&)      = default;
        virtual ~ViewModel()                   = default;

        void SetCallback(const std::function<void(U)>& callback) { _callback = callback; }

    protected:
        virtual U Convert(const T& value) { return static_cast<U>(value); }

    private:
        ObserverToken          _token;
        std::function<void(U)> _callback;
    };
} // namespace MVVM