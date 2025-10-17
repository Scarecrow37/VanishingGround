#pragma once

template <typename T>
class GraphicsPointer
{
public:
    GraphicsPointer() noexcept : _pointer(nullptr) {}
    GraphicsPointer(decltype(nullptr)) noexcept : _pointer(nullptr) {}
    GraphicsPointer(T* ptr) noexcept : _pointer(ptr) { AddReference(); }
    GraphicsPointer(const GraphicsPointer& other) noexcept : _pointer(other._pointer) { AddReference(); }
    GraphicsPointer(GraphicsPointer&& other) noexcept : _pointer(other._pointer) { other._pointer = nullptr; }
    ~GraphicsPointer() noexcept { Release(); }

public:
    GraphicsPointer& operator=(decltype(nullptr)) noexcept
    {
        Release();
        return *this;
    }

    GraphicsPointer& operator=(T* ptr) noexcept
    {
        if (_pointer != ptr)
        {
            GraphicsPointer(ptr).Swap(*this);
        }
        return *this;
    }

    GraphicsPointer& operator=(const GraphicsPointer& other) noexcept
    {
        if (_pointer != other._pointer)
        {
            GraphicsPointer(other).Swap(*this);
        }
        return *this;
    }

    GraphicsPointer& operator=(GraphicsPointer&& other) noexcept
    {
        GraphicsPointer(static_cast<GraphicsPointer&&>(other)).Swap(*this);
        return *this;
    }

    T**      operator&() noexcept { return &_pointer; }
    T*       operator->() const noexcept { return _pointer; }
    bool     operator==(const GraphicsPointer& other) const noexcept { return _pointer == other._pointer; }
    explicit operator bool() const noexcept { return _pointer != nullptr; }

public:
    T*   Get() const noexcept { return _pointer; }

    void Reset() noexcept
    {
        Release();
    }

    void Reset(T* ptr) noexcept { Release(); }

    void Swap(GraphicsPointer& other) noexcept
    {
        T* temp        = _pointer;
        _pointer       = other._pointer;
        other._pointer = temp;
    }

    void Swap(GraphicsPointer&& other) noexcept
    {
        T* temp        = _pointer;
        _pointer       = other._pointer;
        other._pointer = temp;
    }

private:
    void AddReference() noexcept
    {
        if (_pointer)
        {
            _pointer->AddReference();
        }
    }

    void Release() noexcept
    {
        if (_pointer)
        {
            _pointer->Release();
            _pointer = nullptr;
        }
    }

private:
    T* _pointer;
};