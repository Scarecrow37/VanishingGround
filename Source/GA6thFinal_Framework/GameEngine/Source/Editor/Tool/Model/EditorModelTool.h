#pragma once

class EditorDynamicCamera;
class EditorModelTool : public EditorTool
{
public:
    EditorModelTool();
    virtual ~EditorModelTool() = default;

private:
    virtual void OnTickGui() override;
    virtual void OnStartGui() override;
    virtual void OnEndGui() override;

private:
    /* Begin 호출 전에 호출 */
    virtual void OnPreFrame() override;

    /* Begin 호출 시 호출 */
    virtual void OnFrame() override;

    /* End 호출 후에 호출 */
    virtual void OnPostFrame() override;

    /* 프레임이 포커싱 될 때 호출 (Begin 후에 호출) */
    virtual void OnFocus() override;

    /* PopUp창 호출 성공 시 호출 (Begin 후에 호출) */
    virtual void OnPopup() override;

private:
    void ImportFBX(std::filesystem::path path);
    void ExportFBX();

private:
    std::unique_ptr<EditorDynamicCamera> _camera;
    //std::shared_ptr<StaticMeshRenderer> 
};
