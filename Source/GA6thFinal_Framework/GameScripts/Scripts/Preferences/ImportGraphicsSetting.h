#pragma once

class ImportGraphicsSetting : public Component
{
    USING_PROPERTY(ImportGraphicsSetting)

public:
    ImportGraphicsSetting();
    ~ImportGraphicsSetting() override;

public:
    REFLECT_PROPERTY(FilePath)
    GETTER_ONLY(std::string, FilePath)   
    {
        return ReflectFields->Guid;
    }
    PROPERTY(FilePath)

protected:
    void Reset() override;

protected:
    REFLECT_FIELDS_BEGIN(Component)
    std::string Guid;
    REFLECT_FIELDS_END(ImportGraphicsSetting)
};

