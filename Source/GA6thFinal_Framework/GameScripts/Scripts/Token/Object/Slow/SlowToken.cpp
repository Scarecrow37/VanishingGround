#include "pchScripts.h"
#include "SlowToken.h"
#include "Token/TokenSystem.h"

namespace TokenObject
{
    REGISTER_TOKEN(Slow1)
    REGISTER_TOKEN(Slow2)
    REGISTER_TOKEN(Slow3)
    void Slow1::OnRollRandomSpeed(CharacterBase* source, int& speed) 
    {
        speed -= GetTokenParam(0);
    }
    void Slow2::OnRollRandomSpeed(CharacterBase* source, int& speed) 
    {
        speed -= GetTokenParam(0);
    }
    void Slow3::OnRollRandomSpeed(CharacterBase* source, int& speed) 
    {
        speed -= GetTokenParam(0);
    }
} // namespace TokenObject