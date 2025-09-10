#pragma once

namespace MVVM
{
    template <typename T, typename U>
    class ViewModel
    {
    public:
        class Handle
        {
            friend class ViewModel<T, U>;

            static constexpr size_t INVALID_INDEX = SIZE_MAX;

        public:
            Handle() : _index(INVALID_INDEX) {}
            explicit Handle(const size_t index) : _index(index) {}

        private:
            size_t _index;
        };

        using Callback = std::function<void(U)>;

    public:
        explicit ViewModel(Model<T>& model)
        {
            model.AddObserver([this](T value) {
                std::ranges::for_each(_callbacks, [this, &value](const Callback& callback) {
                    if (nullptr != callback)
                        callback(Convert(value));
                });
            });
            model.Notify();
        }

        ViewModel(const ViewModel&)            = default;
        ViewModel& operator=(const ViewModel&) = default;
        ViewModel(ViewModel&&)                 = default;
        ViewModel& operator=(ViewModel&&)      = default;
        virtual ~ViewModel()                   = default;

        Handle AddCallback(const Callback& callback)
        {
            for (auto it = _callbacks.begin(); it != _callbacks.end(); ++it)
            {
                if (nullptr == *it)
                {
                    *it = callback;
                    return Handle(std::distance(_callbacks.begin(), it));
                }
            }

            _callbacks.push_back(callback);
            return Handle(_callbacks.size() - 1);
        }

        void RemoveCallback(const Handle& handle)
        {
            if (handle._index < _callbacks.size())
            {
                _callbacks[handle._index] = nullptr;
            }
        }

    protected:

        virtual U Convert(const T& value) = 0;

    private:
        std::vector<Callback> _callbacks;

    };
} // namespace MVVM