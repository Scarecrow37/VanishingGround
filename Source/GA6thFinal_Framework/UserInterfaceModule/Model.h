#pragma once

namespace MVVM
{
    template <typename T>
    class ModelBase
    {
        using Callback = std::function<void(const T&)>;

    public:
        ModelBase(const ModelBase&)                = delete;
        ModelBase& operator=(const ModelBase&)     = delete;
        ModelBase(ModelBase&&) noexcept            = default;
        ModelBase& operator=(ModelBase&&) noexcept = default;
        virtual ~ModelBase()                       = default;

        void AddObserver(const Callback& observer) { _observers.push_back(observer); }

        void Notify() const
        {
            for (const auto& observer : _observers)
            {
                observer(_value);
            }
        }

    protected:
        ModelBase() : _value(T()) {}
        explicit ModelBase(T value) : _value(value) {}

        T _value;

    private:
        std::vector<Callback> _observers;
    };

    template <typename T>
    class Model final : public ModelBase<T>
    {

    public:
        Model() = default;
        explicit Model(const T& value) : ModelBase<T>(value) {}

        T Get() const { return ModelBase<T>::_value; }

        void Set(const T& value)
        {
            ModelBase<T>::_value = value;
            ModelBase<T>::Notify();
        }

        void Set(T&& value)
        {
            ModelBase<T>::_value = std::move(value);
            ModelBase<T>::Notify();
        }
    };

    template <class T>
    class Model<std::deque<T>> final : public ModelBase<std::deque<T>>
    {
        using container_type  = std::deque<T>;
        using reference       = typename container_type::reference;
        using const_reference = typename container_type::const_reference;
        using iterator        = typename container_type::iterator;
        using const_iterator  = typename container_type::const_iterator;
        using size_type       = typename container_type::size_type;

    public:
        Model() = default;

        void clear() noexcept
        {
            ModelBase<container_type>::_value.clear();
            ModelBase<container_type>::Notify();
        }

        template <typename... Args>
        reference emplace_back(Args&&... args)
        {
            reference result = ModelBase<container_type>::_value.emplace_back(std::forward<Args>(args)...);
            ModelBase<container_type>::Notify();
            return result;
        }

        bool empty() const noexcept { return ModelBase<container_type>::_value.empty(); }


        const_iterator begin() const noexcept { return ModelBase<container_type>::_value.begin(); }

        const_iterator cbegin() const noexcept { return ModelBase<container_type>::_value.cbegin(); }

        const_iterator end() const noexcept { return ModelBase<container_type>::_value.end(); }

        const_iterator cend() const noexcept { return ModelBase<container_type>::_value.cend(); }

        const_reference front() const noexcept { return ModelBase<container_type>::_value.front(); }

        void pop_front() noexcept
        {
            ModelBase<container_type>::_value.pop_front();
            ModelBase<container_type>::Notify();
        }

        void pop_back() noexcept
        {
            ModelBase<container_type>::_value.pop_back();
            ModelBase<container_type>::Notify();
        }

        size_type size() const noexcept { return ModelBase<container_type>::_value.size(); }

        iterator erase(const_iterator pos)
        {
            iterator result = ModelBase<container_type>::_value.erase(pos);
            ModelBase<container_type>::Notify();
            return result;
        }

        iterator erase(const_iterator first, const_iterator last)
        {
            iterator result = ModelBase<container_type>::_value.erase(first, last);
            ModelBase<container_type>::Notify();
            return result;
        }

        template <typename UniformRandomBitGenerator>
        void shuffle(UniformRandomBitGenerator&& generator)
        {
            std::shuffle(begin(), end(), std::forward<UniformRandomBitGenerator>(generator));
            ModelBase<container_type>::Notify();
        }

        void sort()
        {
            std::sort(begin(), end());
            ModelBase<container_type>::Notify();
        }

        template <typename Compare>
        void sort(Compare compare)
        {
            std::sort(begin(), end(), compare);
            ModelBase<container_type>::Notify();
        }

        template <typename Predicate>
        size_type erase_if(Predicate predicate)
        {
            const auto old_size = size();
            erase(std::remove_if(begin(), end(), predicate), end());
            const auto new_size = size();
            return old_size - new_size;
        }

        template <typename Modifier>
        void ModifyFront(Modifier modifier)
        {
            modifier(front());
            ModelBase<container_type>::Notify();
        }

    protected:
        iterator begin() noexcept { return ModelBase<container_type>::_value.begin(); }

        iterator end() noexcept { return ModelBase<container_type>::_value.end(); }

        reference front() noexcept { return ModelBase<container_type>::_value.front(); }

    };
} // namespace MVVM