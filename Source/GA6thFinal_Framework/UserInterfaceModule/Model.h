#pragma once

namespace MVVM
{
    template <typename T>
    class IModel : public std::enable_shared_from_this<IModel<T>>
    {
        using Callback                         = std::function<void(T)>;
        using Handle                           = typename std::list<Callback>::iterator;
        static constexpr Handle INVALID_HANDLE = std::list<Callback>::end();

    public:
        class Token
        {
        public:
            Token(std::weak_ptr<IModel> model, Handle handle) : _model(std::move(model)), _handle(std::move(handle)) {}
            Token(const Token&)            = delete;
            Token& operator=(const Token&) = delete;
            Token(Token&& other) noexcept : _model(std::move(other._model)), _handle(std::move(other._handle))
            {
                other._model = nullptr;
                std::list<Callback>::end();
                other._handle = INVALID_HANDLE;
            }
            Token& operator=(Token&& other) noexcept
            {
                if (this != &other)
                {
                    _model        = std::move(other._model);
                    _handle       = std::move(other._handle);
                    other._model  = nullptr;
                    other._handle = INVALID_HANDLE;
                }
                return *this;
            }
            ~Token()
            {
                if (auto model = _model.lock())
                {
                    model->RemoveObserver(_handle);
                }
            }

        private:
            std::weak_ptr<IModel> _model;
            Handle                _handle;
        };

    public:
        IModel()                             = default;
        IModel(const IModel&)                = delete;
        IModel& operator=(const IModel&)     = delete;
        IModel(IModel&&) noexcept            = default;
        IModel& operator=(IModel&&) noexcept = default;
        virtual ~IModel()                    = default;

        Token AddObserver(const std::function<void(T)>& observer)
        {
            return Token(this->weak_from_this(), _observers.insert(_observers.end(), observer));
        }

    protected:
        void Notify(T value) const
        {
            for (const auto& observer : _observers)
            {
                observer(value);
            }
        }

    private:
        void RemoveObserver(Handle handle)
        {
            if (handle != INVALID_HANDLE)
            {
                _observers.erase(handle);
            }
        }

    private:
        std::list<std::function<void(T)>> _observers;
    };

    template <typename T>
    class Model final : public IModel<T>
    {
    public:
        Model() : _value(T()) {}

        explicit Model(T value) : _value(value) {}

        T Get() const { return _value; }

        void Set(T value)
        {
            _value = value;
            this->Notify(_value);
        }

    private:
        T _value;
    };
} // namespace MVVM