#pragma once
template <typename... Args>
class UmDelegate
{
public:
    using FunctionType = std::function<void(Args...)>;
    using Handle       = uint64_t;

    // Add a listener and return a unique handle
    Handle AddListener(FunctionType listener)
    {
        Handle id = _nextId++;

        // 1. Store the handle and the listener function in the vector
        _listeners.emplace_back(id, std::move(listener));

        // 2. Store the mapping from the handle to the vector's index in the map
        _handleToIndexMap[id] = _listeners.size() - 1;

        return id;
    }

    // Remove a specific listener using its handle (Swap and Pop technique)
    bool RemoveListener(Handle id)
    {
        // 1. Find the vector index corresponding to the handle from the map (O(log N))
        auto it = _handleToIndexMap.find(id);
        if (it == _handleToIndexMap.end())
        {
            return false;
        }
        size_t indexOfListenerToRemove = it->second;

        // 2. Swap the element to be removed with the last element in the vector
        //    (This is an O(1) operation for vectors)
        size_t lastIndex = _listeners.size() - 1;
        if (indexOfListenerToRemove != lastIndex)
        {
            // Swap with the last element
            std::swap(_listeners[indexOfListenerToRemove], _listeners[lastIndex]);

            // Update the index mapping for the element that was moved
            Handle handleOfMovedListener              = _listeners[indexOfListenerToRemove].first;
            _handleToIndexMap[handleOfMovedListener] = indexOfListenerToRemove;
        }

        // 3. Remove the last element from the vector (now the one we wanted to delete)
        _listeners.pop_back();

        // 4. Remove the original handle from the map
        _handleToIndexMap.erase(it);

        return true;
    }

    // Invoke all registered listeners (very fast)
    template <typename... TArgs>
    void Invoke(TArgs&&... args)
    {
        static_assert(sizeof...(Args) == sizeof...(TArgs), "Invoke: Argument count mismatch");

        static_assert((std::conjunction_v<std::is_convertible<TArgs, Args>...>),
                      "Invoke: Argument types cannot be converted to delegate signature");

        // Iterating through a vector is very fast due to contiguous memory
        for (const auto& pair : _listeners)
        {
            pair.second(std::forward<TArgs>(args)...);
        }
    }

    // Clear all listeners
    void Clear()
    {
        _listeners.clear();
        _handleToIndexMap.clear();
    }

    // Clear Stats Check
    operator bool() const { return _listeners.empty(); }
    bool operator==(std::nullptr_t) const { return _listeners.empty(); }
    bool operator!=(std::nullptr_t) const { return false == _listeners.empty(); }

private:
    // Stores <Handle, Function> pairs for fast iteration.
    std::vector<std::pair<Handle, FunctionType>> _listeners;

    // Maps <Handle, vector_index> for fast lookup.
    std::map<Handle, size_t> _handleToIndexMap;

    Handle _nextId = 1; // Start IDs from 1
};