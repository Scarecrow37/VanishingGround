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

    public:
        void AddObserver(const Callback& observer) { _observers.push_back(observer); }

        void Reset() { _observers.clear(); }

        void Notify() const
        {
            for (const auto& observer : _observers)
            {
                observer(_value);
            }
        }

    protected:
        ModelBase() = default;
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

        Model(const T& value) : ModelBase<T>(value) {}

        Model& operator=(const T& value)
        {
            ModelBase<T>::_value = value;
            ModelBase<T>::Notify();
            return *this;
        }

        Model& operator=(T&& value) noexcept(std::is_nothrow_move_assignable_v<T>)
        {
            ModelBase<T>::_value = std::move(value);
            ModelBase<T>::Notify();
            return *this;
        }

    public:
        operator T() const noexcept { return ModelBase<T>::_value; }
        //explicit operator bool() const noexcept { return ModelBase<T>::_value != 0; }

    public:
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

    template <typename T>
    class Model<T*> final : public ModelBase<T*>
    {
    public:
        Model() = default;
        explicit Model(T* value) : ModelBase<T*>(value) {}
        Model& operator=(T* value)
        {
            ModelBase<T*>::_value = value;
            ModelBase<T*>::Notify();
            return *this;
        }

    public:
        const T* operator->() const noexcept { return ModelBase<T*>::_value; }

        const T& operator*() const { return *ModelBase<T*>::_value; }

        auto operator<=>(std::nullptr_t) const noexcept { return ModelBase<T*>::_value <=> nullptr; }

        bool operator==(std::nullptr_t) const noexcept { return ModelBase<T*>::_value == nullptr; }

        explicit operator bool() const noexcept { return ModelBase<T*>::_value != nullptr; }

    public:
        const T* Get() const { return ModelBase<T*>::_value; }

        void Set(T* value)
        {
            ModelBase<T*>::_value = value;
            ModelBase<T*>::Notify();
        }

        void Apply(std::function<void(T*)> modifier) const
        {
            if (ModelBase<T*>::_value)
            {
                modifier(ModelBase<T*>::_value);
                ModelBase<T*>::Notify();
            }
        }
    };

    template <class T>
    class Model<std::vector<T>> final : public ModelBase<std::vector<T>>
    {
        using container_type  = std::vector<T>;
        using reference       = typename container_type::reference;
        using const_reference = typename container_type::const_reference;
        using iterator        = typename container_type::iterator;
        using const_iterator  = typename container_type::const_iterator;
        using size_type       = typename container_type::size_type;

    public:
        Model() = default;
        
        Model(const std::vector<T>& value) : ModelBase<std::vector<T>>(value) {}
        
        Model& operator=(const std::vector<T>& value)
        {
            ModelBase<std::vector<T>>::_value = value;
            ModelBase<std::vector<T>>::Notify();
            return *this;
        }

        Model& operator=(std::vector<T>&& value) noexcept(std::is_nothrow_move_assignable_v<std::vector<T>>)
        {
            ModelBase<std::vector<T>>::_value = std::move(value);
            ModelBase<std::vector<T>>::Notify();
            return *this;
        }

        const_reference operator[](size_type pos) const { return ModelBase<container_type>::_value[pos]; }

        void clear() noexcept
        {
            ModelBase<container_type>::_value.clear();
            ModelBase<container_type>::Notify();
        }

        bool empty() const noexcept { return ModelBase<container_type>::_value.empty(); }

        void push_back(const T& value)
        {
            ModelBase<container_type>::_value.push_back(value);
            ModelBase<container_type>::Notify();
        }

        void push_back(T&& value)
        {
            ModelBase<container_type>::_value.push_back(std::move(value));
            ModelBase<container_type>::Notify();
        }

        void resize(size_type count)
        {
            ModelBase<container_type>::_value.resize(count);
            ModelBase<container_type>::Notify();
        }

        void resize(size_type count, const T& value)
        {
            ModelBase<container_type>::_value.resize(count, value);
            ModelBase<container_type>::Notify();
        }

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

        size_type erase(const T& value)
        {
            auto size = std::erase(ModelBase<container_type>::_value, value);
            ModelBase<container_type>::Notify();
            return size;
        }

        const_reference at(size_type pos) const { return ModelBase<container_type>::_value.at(pos); }

        template <typename Modifier>
        void at(size_type pos, Modifier modifier)
        {
            modifier(ModelBase<container_type>::_value.at(pos));
            ModelBase<container_type>::Notify();
        }

        template <typename UniformRandomBitGenerator>
        void shuffle(UniformRandomBitGenerator&& generator)
        {
            std::ranges::shuffle(ModelBase<container_type>::_value, std::forward<UniformRandomBitGenerator>(generator));
            ModelBase<container_type>::Notify();
        }

        template <typename Modifier>
        void for_each(Modifier modifier)
        {
            std::ranges::for_each(ModelBase<container_type>::_value, modifier);
            ModelBase<container_type>::Notify();
        }

        const_iterator begin() const noexcept { return ModelBase<container_type>::_value.begin(); }
        const_iterator cbegin() const noexcept { return ModelBase<container_type>::_value.cbegin(); }
        const_iterator end() const noexcept { return ModelBase<container_type>::_value.end(); }
        const_iterator cend() const noexcept { return ModelBase<container_type>::_value.cend(); }

        size_type size() const noexcept { return ModelBase<container_type>::_value.size(); }

    protected:
        iterator begin() noexcept { return ModelBase<container_type>::_value.begin(); }
        iterator end() noexcept { return ModelBase<container_type>::_value.end(); }
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

        Model& operator=(const std::deque<T>& value)
        {
            ModelBase<std::deque<T>>::_value = value;
            ModelBase<std::deque<T>>::Notify();
            return *this;
        }

        Model& operator=(std::deque<T>&& value) noexcept(std::is_nothrow_move_assignable_v<std::deque<T>>)
        {
            ModelBase<std::deque<T>>::_value = std::move(value);
            ModelBase<std::deque<T>>::Notify();
            return *this;
        }

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
            std::ranges::shuffle(ModelBase<container_type>::_value, std::forward<UniformRandomBitGenerator>(generator));
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