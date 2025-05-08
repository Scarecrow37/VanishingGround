#pragma once
#include "UmFramework.h"

namespace Command
{
    namespace Hierarchy
    {
        class NewGameObjectCommand : public UmCommand
        {
        public:
            NewGameObjectCommand(std::string_view type_id, std::string_view name);
            virtual ~NewGameObjectCommand() = default;

        private:
            std::weak_ptr<GameObject> _newObject;
            std::string _newName;
            std::string _typeName;

            // UmCommand을(를) 통해 상속됨
            void Execute() override;
            void Undo() override;
        };
    } // namespace Hierarchy
} // namespace Command