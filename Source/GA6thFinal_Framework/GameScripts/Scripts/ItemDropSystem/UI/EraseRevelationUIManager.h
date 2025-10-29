#pragma once
#include "Utility/SingletonHelper.h"
#include "ItemDropSystem/Interface/IDropItem.h"

class DescriptionPanel;
class TextElement;
class ImageElement;
class EraseRevelationNavi;
class InputOkCancelComponent;
class EraseRevelationUIManager : public Component, public InputReceiver
{
    USING_PROPERTY(EraseRevelationUIManager)

public:
    inline static constexpr const char* TAG = "Erase Revelation UI Manager";

    EraseRevelationUIManager();
    ~EraseRevelationUIManager() override;

public:
    void OpenUI(const size_t artifactObtainIndex);
    void CloseUI();
    void EraseRevelation(int slot);
    void SetRevelationInfoUI(const DropItemInfo& info);

public:
    REFLECT_PROPERTY()

    GETTER_ONLY(InputOkCancelComponent*, InputOkCancel) { return _inputOkCancelComponent.lock().get(); }
    PROPERTY(InputOkCancel)

protected:
    REFLECT_FIELDS_BEGIN(Component)
    REFLECT_FIELDS_END(EraseRevelationUIManager)

    void Added() override;
    void Awake() override;
    void Start() override;
    void Update() override;
    void OnButtonDownB(const Input::Controller&);

private:
    SingletonComponent<EraseRevelationUIManager> _singletonComponent;

    std::vector<EraseRevelationNavi*> _focusNaviElements;
    std::vector<ImageElement*>        _revelationImages;

    struct RevelationInfoUI
    {
        TextElement*      Name        = nullptr;
        ImageElement*     Icon        = nullptr;
        DescriptionPanel* Description = nullptr;
        DescriptionPanel* Flavor      = nullptr;
        DescriptionPanel* Keyword     = nullptr;
    };
    RevelationInfoUI _revelation;

private:
    void FindElements();

    size_t _artifactObtainIndex = std::numeric_limits<size_t>::max();
    bool   _closeFlag           = false;

    std::weak_ptr<InputOkCancelComponent> _inputOkCancelComponent;
};

