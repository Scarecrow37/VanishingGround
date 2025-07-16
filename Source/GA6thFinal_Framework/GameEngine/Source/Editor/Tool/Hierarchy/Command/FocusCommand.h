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
            FocusCommand(std::weak_ptr<GameObject> oldWp, std::weak_ptr<GameObject> newWp, std::string_view commandName = "Focus") 
                : FocusObject(oldWp, newWp, commandName)
            {
            }
            virtual ~FocusCommand() override;

            virtual bool Execute() override;
            virtual void Undo() override;
        };
    } 
} 
