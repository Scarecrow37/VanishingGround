#pragma once

class SelectMagicBook;
class SelectPopup : public UISFXNavigationComponent
{
    USING_PROPERTY(SelectPopup)

public:
    SelectPopup();
    ~SelectPopup() override;

public:
    REFLECT_PROPERTY(NextScene)
    GETTER_ONLY(std::string, NextScene) { return File::Guid(ReflectFields->NextSceneGuid).ToPath().string(); }
    PROPERTY(NextScene)
        
public:
    void SetSelectMagicBook(const std::string& imagePath, const std::string& title, SelectMagicBook* magicBook);   

protected:
    REFLECT_FIELDS_BEGIN(UISFXNavigationComponent)
    std::string NextSceneGuid;
    REFLECT_FIELDS_END(SelectPopup)

private:
    SelectMagicBook* _lastSelectMagicBook = nullptr;
};