#pragma once
#include "UmFramework.h"

namespace Command
{
    namespace Hierarchy
    {
        class PackPrefabCommand : public UmCommand
        {
        public:
            PackPrefabCommand(std::weak_ptr<GameObject> targetObject, const File::Guid& guid);
            virtual ~PackPrefabCommand() = default;

        private:
            std::weak_ptr<GameObject> _targetObject;
            File::Guid _currGuid;
            File::Guid _prevGuid;

            // UmCommand을(를) 통해 상속됨
            void Execute() override;
            void Undo() override;
        };
    } // namespace Hierarchy
} // namespace Command