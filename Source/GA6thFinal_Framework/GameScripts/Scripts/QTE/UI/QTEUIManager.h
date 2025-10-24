#pragma once
#include <QTE/Result/QTEResult.h>
#include <Utility/SingletonHelper.h>
#include <Utility/FadeHelper.h>

#include "QTE/UI/QTEBackgroundUI.h"
#include "QTE/UI/QTEFieldUI.h"
#include "QTE/UI/QTEGuideUI.h"
#include "QTE/UI/QTENoteUI.h"

class SpriteAnimationElement;

namespace QTE
{
    class Track;
}
class QTESystem;
class OverlayPanel;
class ImageElement;
class CameraComponent;


class QTEUIManager : public Component
{
    friend class QTESystem;
    USING_PROPERTY(QTEUIManager)

public:
    QTEUIManager();
    ~QTEUIManager() override;

public:
    REFLECT_PROPERTY(FilePath, PoolSize)
    GETTER_ONLY(std::string, FilePath) { return File::Guid(ReflectFields->NotePrefabGuid).ToPath().string(); }
    PROPERTY(FilePath)

    SETTER(int, PoolSize) { ReflectFields->PoolSize = value; }
    GETTER(int, PoolSize) { return ReflectFields->PoolSize; }
    PROPERTY(PoolSize)

private:
    void OnQTEEnter();
    void OnQTEButtonPressed();
    void OnQTENotePressed(UINT noteID, QTE::ResultType result);
    void OnQTEPlay();
    void OnQTEExit();

public:
    /// <summary>노트 프리팹의 GUID를 설정합니다. 해당 프리팹을 통해 QTE 노트 UI가 생성됩니다.</summary>
    /// <param name="guid">설정할 File::Guid 객체입니다.</param>
    void SetNotePrefabGuid(const File::Guid& guid);

    // TODO: 나중에 블러로 변경 가능성 있음
    /// <summary>QTE 백그라운드 UI의 알파 값을 설정합니다.</summary>
    /// <param name="factor">설정할 UI의 알파 값입니다. 0.0f에서 1.0f 사이의 값을 가집니다.</param>
    void SetUIAlpha(float factor);

private:
    void Reset() override;
    void Awake() override;
    void Start() override;
    void Update() override;

    void SerializedReflectEvent() override;
    void DeserializedReflectEvent() override;
    void ImGuiDrawPropertysEvent() override;

private:
    void ResetUI();
    void InitializeNotePool();

    QTE::NoteUI* GetNoteUIFromID(UINT id);

    void FindUIComponents();

private:
    SingletonComponent<QTEUIManager>    _singletoneComponent{this};

    OverlayPanel*                       _overlayPanel = nullptr;
    QTE::BackgroundUI                   _backGroundUI;
    QTE::FieldUI                        _fieldUI;
    QTE::GuideUI                        _guideUI;

    std::vector<QTE::NoteUI>            _notePool;
    std::unordered_map<UINT, size_t>    _activedNote;

    Fader _mainFader;

    REFLECT_FIELDS_BEGIN(Component)
    std::string NotePrefabGuid; // QTE 노트 프리팹 GUID
    int         PoolSize = 15;
    REFLECT_FIELDS_END(QTEUIManager)
    
};