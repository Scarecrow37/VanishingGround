#pragma once

namespace QTE
{
    class Note : public Timeline::EventContext
    {
    public:
        USING_PROPERTY(Note)
        Note()          = default;
        virtual ~Note() = default;

        REFLECT_PROPERTY(WeaponAnimation)
        GETTER(std::string_view, WeaponAnimation) { return ReflectFields->WeaponAnimationKey; }
        SETTER(std::string_view, WeaponAnimation) { ReflectFields->WeaponAnimationKey = value; }
        PROPERTY(WeaponAnimation)

    private:
        REFLECT_FIELDS_BEGIN(Timeline::EventContext)
        std::string WeaponAnimationKey;
        REFLECT_FIELDS_END(Note)
    };
}