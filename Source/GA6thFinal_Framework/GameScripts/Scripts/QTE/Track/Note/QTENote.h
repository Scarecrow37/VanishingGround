#pragma once
#include "QTENoteData.h"

namespace QTE
{
    enum class NoteType
    {
        DEFAULT = 0,
    };
    class Note : public Timeline::EventContext
    {
    public:
        USING_PROPERTY(Note)
        Note()          = default;
        virtual ~Note() = default;

        REFLECT_PROPERTY(WeaponAnimation, WeaponAnimationDelay)
        GETTER(std::string_view, WeaponAnimation) { return ReflectFields->WeaponAnimationKey; }
        SETTER(std::string_view, WeaponAnimation) { ReflectFields->WeaponAnimationKey = value; }
        PROPERTY(WeaponAnimation)
        GETTER(float, WeaponAnimationDelay) { return ReflectFields->WeaponAnimationDelay; }
        SETTER(float, WeaponAnimationDelay) { ReflectFields->WeaponAnimationDelay = std::max(value, 0.0f); }
        PROPERTY(WeaponAnimationDelay)

        inline NoteType             GetNoteType() const { return ReflectFields->NoteType; }
        inline const std::string&   GetWeaponAnimationName() const { return ReflectFields->WeaponAnimationKey; }
        inline float                GetWeaponAnimationDelay() const { return ReflectFields->WeaponAnimationDelay; }
        inline NoteData             ToNoteData() const { return NoteData(ID, Time, GetWeaponAnimationName(), GetWeaponAnimationDelay()); }

    private:
        REFLECT_FIELDS_BEGIN(Timeline::EventContext)
        NoteType    NoteType             = NoteType::DEFAULT;
        std::string WeaponAnimationKey   = "";
        float       WeaponAnimationDelay = 0.0f; // 무기 애니메이션 재생 딜레이 (초)
        REFLECT_FIELDS_END(Note)
    };
}