#pragma once

class EditorFiniteStateMachine : public EditorTool
{
public:
    EditorFiniteStateMachine();
    virtual ~EditorFiniteStateMachine();

private:
    virtual void OnTickGui() override;
    virtual void OnStartGui() override;
    virtual void OnEndGui() override;

     /* Begin 호출 전에 호출 */
    virtual void OnPreFrameBegin() override;
    /* Begin 호출 직후 호출 */
    virtual void OnPostFrameBegin() override;
    /* Begin 호출 후 클리핑 테스트를 통과한 후 호출 */
    virtual void OnFrameRender() override;
    /* End 호출 후에 호출 */
    virtual void OnFrameEnd() override;

private:
    EditorNodeGraph* _nodeGraph;

    REFLECT_FIELDS_BEGIN(EditorTool)
    std::string SerializeData;
    REFLECT_FIELDS_END(EditorFiniteStateMachine)
};
