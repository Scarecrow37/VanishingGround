#pragma once
#include "UmFramework.h"

namespace Command
{
    namespace Hierarchy
    {
        class DetachChildrenCommand : public UmCommand
        {
        public:
            DetachChildrenCommand(std::weak_ptr<GameObject> targetObject);
            virtual ~DetachChildrenCommand() = default;

        private:
            std::weak_ptr<GameObject> _targetObject;
            std::vector<std::weak_ptr<GameObject>> _childrens;

            // UmCommand을(를) 통해 상속됨
            void Execute() override;
            void Undo() override;
        };
    } // namespace Hierarchy
} // namespace Command