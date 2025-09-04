#pragma once
class QTEEditor;

class QTESystem : public Component
{
    USING_PROPERTY(QTESystem)
    inline static QTESystem* _staticInstance = nullptr;
public:
    QTESystem();
    ~QTESystem() override;
    inline static QTESystem* GetInstance() { return _staticInstance; }

private:
    void SerializedReflectEvent() override;
    void DeserializedReflectEvent() override;
    void ImGuiDrawPropertysEvent() override;

public:

private:
    REFLECT_FIELDS_BEGIN(Component)
    float DelayFromQTEStart = 0.0f; // QTE 시작 대기 시간
    REFLECT_FIELDS_END(QTESystem)
    
    // QTE 편집기
    QTEEditor&  GetEditor();
    void        OpenEditor();
};
