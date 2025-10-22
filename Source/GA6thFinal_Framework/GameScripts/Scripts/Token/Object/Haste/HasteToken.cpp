#include "pchScripts.h"
#include "HasteToken.h"
#include "Token/TokenSystem.h"

namespace TokenObject
{
    REFLECT_FUNCTION(Haste1)
    REFLECT_FUNCTION(Haste2)
    REFLECT_FUNCTION(Haste3)
    REGISTER_TOKEN(Haste1)
    REGISTER_TOKEN(Haste2)
    REGISTER_TOKEN(Haste3)
    void Haste1::OnRollRandomSpeed(CharacterBase* source, int& speed) 
    {
        speed += ReflectFields->SpeedIncrease;
    }
    void Haste2::OnRollRandomSpeed(CharacterBase* source, int& speed)
    {
        speed += ReflectFields->SpeedIncrease;
    }
    void Haste3::OnRollRandomSpeed(CharacterBase* source, int& speed)
    {
        speed += ReflectFields->SpeedIncrease;
    }
} // namespace TokenObject