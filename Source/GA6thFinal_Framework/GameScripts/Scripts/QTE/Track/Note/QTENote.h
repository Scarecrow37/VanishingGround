#pragma once

namespace QTE
{
    class Note : public Timeline::EventContext
    {
    public:
        USING_PROPERTY(Note)
        Note()          = default;
        virtual ~Note() = default;

        GETTER(std::string_view, WeaponAnimation) { return ReflectFields->WeaponAnimationKey; }
        SETTER(std::string_view, WeaponAnimation) { ReflectFields->WeaponAnimationKey = value; }
        PROPERTY(WeaponAnimation)

    private:
        REFLECT_FIELDS_BEGIN(Timeline::EventContext)
        std::string WeaponAnimationKey;
        REFLECT_FIELDS_END(Note)
        // 무기 애니메이션 키
        // 
    };
}