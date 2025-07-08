#pragma once

namespace UI
{
    template <typename T, typename U>
    class ViewModel
    {
    public:
        explicit ViewModel(IModel<T>* model) : _model(model)
        {
            _model->AddObserver([this](T value) {
                if (_callback)
                    _callback(Convert(value));
            });
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
        IModel<T>*             _model;
        std::function<void(U)> _callback;
    };
} // namespace UI