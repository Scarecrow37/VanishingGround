#pragma once

class ExcelDataBase;
class SelectMagicBook : public UISFXNavigationComponent
{
    USING_PROPERTY(SelectMagicBook)

public:
    SelectMagicBook();
    ~SelectMagicBook() override;

public:
    void Awake() override;

public:
    void FocusIn(FocusCallType callType) override;
    void FocusOut(FocusCallType callType) override;
    void Submit() override;

public:
    REFLECT_PROPERTY(FocusImagePath, UnFocusImagePath)
    GETTER_ONLY(std::string, FocusImagePath) { return File::Guid(ReflectFields->FocusImageGuid).ToPath().string(); }
    PROPERTY(FocusImagePath)

    GETTER_ONLY(std::string, UnFocusImagePath) { return File::Guid(ReflectFields->UnFocusImageGuid).ToPath().string(); }
    PROPERTY(UnFocusImagePath)

    const std::string& GetBookID() { return ReflectFields->TitleID; }

protected:
    REFLECT_FIELDS_BEGIN(UISFXNavigationComponent)
    std::string TitleID;
    std::string FocusImageGuid;
    std::string UnFocusImageGuid;
    std::string FocusDescriptionID;
    std::string UnFocusDescriptionID;
    REFLECT_FIELDS_END(SelectMagicBook)

    void ImGuiDrawPropertysEvent() override;

private:
    void ChangeBookImage(GameObject& targetGameObject, const File::Guid& imgaeGuid);
    void ChangeTitle(GameObject& targetGameObject, const float alpha);
    void ChangeDescription(GameObject& targetGameObject, ExcelDataBase* dataBase, const std::string& ID);
    void ChangeHPHUD(GameObject& targetGameObject, const float alpha);
};