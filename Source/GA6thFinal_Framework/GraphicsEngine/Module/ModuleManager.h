#pragma once

class ModuleManager
{
public:
    ModuleManager();
    ~ModuleManager();

public:
    template <typename T>
    T* GetModule()
    {
        static_assert(std::is_base_of<RenderModule, T>::value, "T must inherit from RenderModule");
        for (auto& module : _modules)
        {
            T* foundModule = dynamic_cast<T*>(module.get());
            if (foundModule)
            {
                return foundModule;
            }
        }
        return nullptr;
    }

public:
    void Initialize();

private:
    std::vector<std::unique_ptr<class RenderModule>> _modules;
};