#pragma once

/*
스크립트 빌드
*/
class EditorBuildSettingMenu : public EditorMenu
{
public:
    EditorBuildSettingMenu();
    virtual ~EditorBuildSettingMenu() = default;

public:
    virtual void OnTickGui() override;
    virtual void OnMenu() override;

private:
    bool isPopup;
    bool isShow;
    static void BuildSettingPopup();
};