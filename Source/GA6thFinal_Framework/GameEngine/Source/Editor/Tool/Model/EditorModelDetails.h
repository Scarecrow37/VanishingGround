#pragma once

class MeshRenderer;
class FBXConverter;
class EditorModelDetails : public EditorTool
{
    friend class EditorModelTool;
    friend class EditorModelMenu;
public:
    EditorModelDetails();
    virtual ~EditorModelDetails() = default;

private:
    virtual void OnTickGui() override;
    virtual void OnStartGui() override;
    virtual void OnEndGui() override;

private:
    /* Begin 호출 전에 호출 */
    virtual void OnPreFrameBegin() override;

    /* Begin 호출 직후 호출 */
    virtual void OnPostFrameBegin() override;

    /* Begin 호출 후 클리핑 테스트를 통과한 후 호출 */
    virtual void OnFrameRender() override;
    virtual void OnFrameClipped() override;

    /* End 호출 후에 호출 */
    virtual void OnFrameEnd() override;

    /* 프레임이 포커싱 될 때 호출 (OnPostFrameBegin 후에 호출) */
    virtual void OnFrameFocusEnter() override;
    virtual void OnFrameFocusStay() override;
    virtual void OnFrameFocusExit() override;

    /* Popup창 호출 성공 시 호출 (OnPreFrameBegin 전에 호출) */
    virtual void OnFramePopupOpened() override;

    private:
    static FBXConverter& GetFBXConverter();

private:
    void ImportModel();
    void ExportModel();
    void SaveModel();

private:
    std::unique_ptr<MeshRenderer>        _meshRenderer;
    Matrix                               _worldMatrix;
    std::filesystem::path                _filePath;
};
