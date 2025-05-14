#pragma once
#include "UmFramework.h"

namespace Command
{
    namespace Hierarchy
    {
        class SetParentCommand : public UmCommand
        {
        public:
            SetParentCommand(std::weak_ptr<GameObject> targetObject,
                             std::weak_ptr<GameObject> prevParent,
                             std::weak_ptr<GameObject> currParent);

            SetParentCommand(std::weak_ptr<GameObject> targetObject, 
                             const std::nullptr_t& prevNull,
                             std::weak_ptr<GameObject> currParent);

            SetParentCommand(std::weak_ptr<GameObject> targetObject, 
                             std::weak_ptr<GameObject> prevParent,
                             const std::nullptr_t& currNull);

            virtual ~SetParentCommand() = default;

        private:
            std::weak_ptr<GameObject> _targetObject;
            std::weak_ptr<GameObject> _prevParent;
            std::weak_ptr<GameObject> _currParent;
            bool _isPrevRoot;
            bool _isCurrRoot;

            // UmCommand을(를) 통해 상속됨
            void Execute() override;
            void Undo() override;
        };
    }
}