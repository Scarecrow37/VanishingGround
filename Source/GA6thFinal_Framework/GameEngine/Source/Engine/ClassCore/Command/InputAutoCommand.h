#pragma once
#include "UmFramework.h"

namespace Command
{
    namespace InputAuto
    {
        template <typename DATA, typename DATA_POINTER>
        class InputAutoCommand : public UmCommand
        {
        public:
            InputAutoCommand(std::string_view name, const DATA& prev, const DATA& curr, DATA_POINTER* pData) 
                :
                UmCommand(name),
                _prev(prev),
                _curr(curr),
                _pData(pData)
            {

            }
            virtual ~InputAutoCommand()
            {

            }

            virtual void Execute() 
            { 
                *_pData = _curr; 
            }
            virtual void Undo() 
            { 
                *_pData = _prev; 
            }

        private:
            DATA _prev;
            DATA _curr;
            DATA_POINTER* _pData;
        };
    } // namespace InputAuto
}
