#pragma once
#include "UmFramework.h"
namespace Command
{
    namespace Hierarchy
    {
        class FocusCommand : public Command::Inspector::FocusObject
        {
            using Super = Command::Inspector::FocusObject;

        public:
            FocusCommand(std::weak_ptr<GameObject> oldWp, std::weak_ptr<GameObject> newWp) : FocusObject(oldWp, newWp)
            {
            }
            virtual ~FocusCommand() override;

            virtual void Execute() override;
            virtual void Undo() override;
        };
    } 
} 
