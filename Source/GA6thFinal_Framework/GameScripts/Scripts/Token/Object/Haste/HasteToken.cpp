#include "pchScripts.h"
#include "HasteToken.h"
#include "Token/TokenSystem.h"

namespace TokenObject
{
    REGISTER_TOKEN(Haste1)
    REGISTER_TOKEN(Haste2)
    REGISTER_TOKEN(Haste3)
    void Haste1::OnRollRandomSpeed(CharacterBase* source, int& speed) 
    {
        speed += GetTokenParam(0);
    }
    void Haste2::OnRollRandomSpeed(CharacterBase* source, int& speed)
    {
        speed += GetTokenParam(0);
    }
    void Haste3::OnRollRandomSpeed(CharacterBase* source, int& speed)
    {
        speed += GetTokenParam(0);
    }
} // namespace TokenObject