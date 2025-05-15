#pragma once
#include "UmFramework.h"

namespace Command
{
    namespace InputAuto
    {
        template <const char* COMMAND_NAME, typename DATA, typename PDATA>
        class InputAutoCommand : public UmCommand
        {
        public:
            InputAutoCommand(const DATA& prev, const DATA& curr, PDATA* pData) 
                :
                UmCommand(COMMAND_NAME),
                _prev(prev),
                _curr(curr),
                _pData(pData)
            {

            }
            virtual ~InputAutoCommand()
            {

            }

        private:
            DATA _prev;
            DATA _curr;
            PDATA* _pData;
            virtual void Execute()
            {
                *_pData = _curr;
            }

            virtual void Undo()
            {
                *_pData = _prev;
            }
        };

    } // namespace InputAuto
}
