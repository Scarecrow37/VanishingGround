#pragma once
#include "UmFramework.h"

namespace Command
{
    namespace Hierarchy
    {
        class DropPrefabCommand : public UmCommand
        {
        public:
            DropPrefabCommand(const File::Guid& guid);
            virtual ~DropPrefabCommand() = default;

        private:
            std::weak_ptr<GameObject> _newObject;
            File::Guid                _guid;

            // UmCommand을(를) 통해 상속됨
            void Execute() override;
            void Undo() override;
        };
    } // namespace Hierarchy
} // namespace Command