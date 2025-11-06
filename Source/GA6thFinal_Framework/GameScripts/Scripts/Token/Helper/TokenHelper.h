#pragma once
#include "Token/Common/TokenCommon.h"

class CharacterBase;

namespace TokenHelper
{
    static bool HasTokenFromID(CharacterBase* owner, TokenID tokenID);
    static bool HasTokenFromTag(CharacterBase* owner, const std::string& tag);
}