#pragma once

template <typename T>
class SharedResource
{
    friend class SharedResource;

public:
    SharedResource()
        : _pointer(new T*(nullptr))
        , _referenceCount(new unsigned long long(1))
    {
    }

    SharedResource(const SharedResource& other)
    {
        this->_pointer = other._pointer;
        this->_referenceCount = other._referenceCount;

        AddReference();
    }

    template <typename U> requires std::derived_from<U, T>
    SharedResource(const SharedResource<U>& other)
    {
        this->_pointer        = reinterpret_cast<T**>(other._pointer);
        this->_referenceCount = other._referenceCount;

        AddReference();
    }

    ~SharedResource() { Release(); }

public:
    T* Get() const { return *_pointer; }

public:
    void ChangeResource(T* pointer)
    {
        if (*_pointer)
            delete (*_pointer);

        (*_pointer) = pointer;
    }

public:
    operator T*() const
    { 
        return *_pointer;
    }

    T* const* operator&() const
    { 
        return &(*_pointer);
    }

    T* operator->() const
    { 
        return *_pointer;
    }

    SharedResource& operator=(const SharedResource& other)
    {
        Release();

        _pointer = other._pointer;
        _referenceCount = other._referenceCount;
        ++(*_referenceCount);

        return *this;
    }

    template <typename T2> requires std::derived_from<T2, T>
    SharedResource& operator=(const SharedResource<T2>& other)
    {
        Release();

        _pointer        = other._pointer;
        _referenceCount = other._referenceCount;
        ++(*_referenceCount);

        return *this;
    }

    SharedResource& operator=(T* other)
    {
        ChangeResource(other);

        return *this;
    }

private:
    template <typename T, typename... Args>
    friend SharedResource<T> MakeSharedResource(Args... args);

private:
    void AddReference() { (*_referenceCount)++; }
    void Release()
    {
        --(*_referenceCount);

        if (0 == (*_referenceCount))
        {
            if (*_pointer)
                delete (*_pointer);

            delete _pointer;
            delete _referenceCount;
        }
    }

private:
    unsigned long long* _referenceCount;
    T**                 _pointer;
};

template <typename T, typename... Args>
SharedResource<T> MakeSharedResource(Args... args)
{
    SharedResource<T> sharedResource;
    (*sharedResource._pointer) = new T(args...);

    return sharedResource;
}