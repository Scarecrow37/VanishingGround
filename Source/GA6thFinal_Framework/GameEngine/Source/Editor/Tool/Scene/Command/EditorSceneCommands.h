#pragma once
#include "UmFramework.h"
#include "Editor/Tool/Hierarchy/Command/FocusCommand.h"

namespace Command
{
    namespace EditorScene
    {
        // 커맨드들
        class DestroyGameObjectCommand : public UmCommand
        {
        public:
            DestroyGameObjectCommand(GameObject* object);
            virtual ~DestroyGameObjectCommand();

        private:
            bool Execute() override;
            void Undo() override;

            std::vector<std::shared_ptr<GameObject>> _destroyObjects;
            std::string                              _ownerSceneName;
            bool                                     _active;
            bool                                     _isFocus;
        };

        class NewGameObjectCommand : public UmCommand
        {
        public:
            NewGameObjectCommand(std::string_view type_id, std::string_view name, GameObject** pOutObject = nullptr);
            virtual ~NewGameObjectCommand() = default;

        private:
            GameObject**                _pOutObject;
            std::shared_ptr<GameObject> _newObject;
            std::string                 _ownerScene;
            std::string                 _newName;
            std::string                 _typeName;
            bool                        _active;

            // UmCommand을(를) 통해 상속됨
            bool Execute() override;
            void Undo() override;
        };

        class DestroyComponentCommand : public UmCommand
        {
        public:
            DestroyComponentCommand(Component* component);
            virtual ~DestroyComponentCommand();

        private:
            bool Execute() override;
            void Undo() override;

            std::shared_ptr<Component> _destroyComponent;
            std::weak_ptr<GameObject>  _ownerObject;
            bool                       _enable;
            int                        _index;
        };

        class AddComponentCommand : public UmCommand
        {
        public:
            AddComponentCommand(GameObject* ownerObject, std::string_view type_id);
            virtual ~AddComponentCommand() = default;

        private:
            std::shared_ptr<Component> _addComponent;
            std::weak_ptr<GameObject>  _ownerObject;
            std::string                _typeName;
            int                        _index;

            // UmCommand을(를) 통해 상속됨
            bool Execute() override;
            void Undo() override;
        };

        class DuplicateCommand : public Command::Hierarchy::FocusCommand
        {
            using Super = FocusCommand;

        public:
            DuplicateCommand(GameObject* sourceObject);
            virtual ~DuplicateCommand() override;

            virtual bool Execute() override;
            virtual void Undo() override;

        private:
            std::vector<std::shared_ptr<GameObject>> _sourceObjects;
            std::vector<std::shared_ptr<GameObject>> _destObjects;
            bool                                     _active;
            std::string                              _ownerSceneName;
        };

        class PasteObjectCommand : public Command::Hierarchy::FocusCommand
        {
              using Super = FocusCommand;
        public:
              PasteObjectCommand(std::wstring_view yamlData);
              ~PasteObjectCommand() override;

              bool Execute() override;
              void Undo() override;

        private:
              std::wstring                             _yamlData;
              std::vector<std::shared_ptr<GameObject>> _destObjects;
              bool                                     _active;
              bool                                     _loadSuccess;
              std::string                              _ownerSceneName;

        };

    }
}