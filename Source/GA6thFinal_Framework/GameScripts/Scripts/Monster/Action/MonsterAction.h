#pragma once

class CharacterBase;
class Enemy;

namespace Monster
{
    class Action
    {
    public:
        Action() = default;
        virtual ~Action() = default;

    public:
        virtual void OnActionEnter(Enemy* caller)   = 0;
        virtual void OnActionUpdate(Enemy* caller)  = 0;
        virtual void OnActionExit(Enemy* caller)    = 0;

    private:
        int            _id;
        std::string    _name;
        std::string    _type;
        CharacterBase* _target;
    };
}
