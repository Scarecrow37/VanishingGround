#pragma once

class MissingComponent : public Component
{
    friend class EComponentFactory;
    USING_PROPERTY(MissingComponent)
public:
    MissingComponent() = default;
    virtual ~MissingComponent() = default;

    GETTER_ONLY(std::string_view, Script)
    { 
        return ReflectFields->typeName;
    }
    PROPERTY(Script)

    REFLECT_PROPERTY(Script)

protected:
    REFLECT_FIELDS_BEGIN(Component)
    std::string typeName = "";
    std::string reflectData = "";
    REFLECT_FIELDS_END(MissingComponent)
};