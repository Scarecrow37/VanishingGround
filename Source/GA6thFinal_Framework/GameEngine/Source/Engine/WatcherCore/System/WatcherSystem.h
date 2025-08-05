#pragma once

namespace Watcher
{
    class System
    {
        template <typename T>
        class Registry
        {
        public:
            static bool Register(const std::string& key, std::shared_ptr<T> value)
            {
                auto [iterator, result] = Map().try_emplace(key, value);
                return result;
            }

            static void Unregister(const std::string& key) { Map().erase(key); }

            static std::shared_ptr<T> Get(const std::string& key) { return Map().at(key); }

        private:
            static std::unordered_map<std::string, std::shared_ptr<T>>& Map()
            {
                static std::unordered_map<std::string, std::shared_ptr<T>> instance;
                return instance;
            }
        };

    public:
        template <typename T, typename... Args>
        void Register(const std::string& key, Args&&... args)
        {
            if (!Registry<T>::Register(std::string(key), std::make_shared<T>(std::forward<Args>(args)...)))
            {
                throw std::invalid_argument("Already registered key: " + key);
            }
        }

        template <typename T>
        void Unregister(const std::string& key)
        {
             Registry<T>::Unregister(std::string(key));
        }

        template <typename T, typename U>
        void Watch(const std::string& key, const std::function<void(U)>& callback)
        {
            std::shared_ptr<T> viewModel;
            try
            {
                viewModel = Registry<T>::Get(key);
            }
            catch (std::out_of_range&)
            {
                throw std::invalid_argument("ViewModel not found for key: " + key);
            }
            if (nullptr == viewModel)
            {
                throw std::logic_error("ViewModel is null for key: " + key);
            }
            viewModel->SetCallback(callback);
        }

        template <typename T>
        void Blind(const std::string& key)
        {
            std::shared_ptr<T> viewModel;
            try
            {
                viewModel = Registry<T>::Get(key);
            }
            catch (...)
            {
                viewModel = nullptr;
            }
            if (nullptr != viewModel)
                viewModel->SetCallback(nullptr);
        }
    };
} // namespace MVVM