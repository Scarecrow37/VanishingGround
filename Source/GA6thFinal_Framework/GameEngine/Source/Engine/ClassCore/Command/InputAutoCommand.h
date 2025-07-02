#pragma once
#include "UmFramework.h"

namespace Command
{
    namespace InputAuto
    {
        template <typename DATA, typename DATA_POINTER, typename weak_ptr_type>
        class InputAutoCommand : public UmCommand
        {
        public:
            InputAutoCommand(std::string_view name, 
                             const DATA& prev, 
                             const DATA& curr,
                             DATA_POINTER* pData, 
                             weak_ptr_type& weak) 
                :
                UmCommand(name),
                _prev(prev),
                _curr(curr),
                _pData(pData),
                _weak(weak)
            {

            }
            virtual ~InputAutoCommand()
            {

            }

            virtual void Execute() 
            { 
                if (false == _weak.expired())
                {
                    *_pData = _curr; 
                }            
            }
            virtual void Undo() 
            { 
                if (false == _weak.expired())
                {
                    *_pData = _prev;
                }
            }

        private:
            DATA _prev;
            DATA _curr;
            DATA_POINTER* _pData;
            weak_ptr_type _weak;
        };
    } // namespace InputAuto
}
