#include "pchScripts.h"
#include "TurnQueueView.h"

#include "ViewModels/TurnQueue/TurnQueueViewModel.h"
#include "UI/Elements/Image/ImageElement.h"

TurnQueueView::TurnQueueView() = default;

TurnQueueView::~TurnQueueView()
{
    UmWatcher.Blind<TurnQueueViewModel>("Turn Queue");
}

void TurnQueueView::Awake()
{
    Component::Awake();
    InitializeFramesAndPortraits();
}

void TurnQueueView::Start()
{
    Component::Start();
    UmWatcher.Watch<TurnQueueViewModel, std::vector<TurnUIData>>(
        "Turn Queue", [this](const std::vector<TurnUIData>& value) {
            const size_t dataSize = value.size();
            for (size_t i = 0; i < 7; ++i)
            {
                if (i < dataSize)
                {
                    if (nullptr != _turnQueueFrames[i])
                    {
                        _turnQueueFrames[i]->Enable = true;
                        _turnQueueFrames[i]->SetImage(value[i].Frame);
                    }
                    if (nullptr != _turnQueuePortraits[i])
                    {
                        _turnQueuePortraits[i]->Enable = true;
                        _turnQueuePortraits[i]->SetImage(value[i].ActorPortrait);
                    }
                }
                else
                {
                    if (nullptr != _turnQueueFrames[i])
                    {
                        _turnQueueFrames[i]->Enable = false;
                    }
                    if (nullptr != _turnQueuePortraits[i])
                    {
                        _turnQueuePortraits[i]->Enable = false;
                    }
                }

            }
            if (dataSize == 0)
            {
                if (nullptr != _firstTurnQueueFrameLeftWing)
                    _firstTurnQueueFrameLeftWing->Enable = false;
                if (nullptr != _firstTurnQueueFrameRightWing)
                    _firstTurnQueueFrameRightWing->Enable = false;
            }
            else
            {
                if (nullptr != _firstTurnQueueFrameLeftWing)
                    _firstTurnQueueFrameLeftWing->Enable = true;
                if (nullptr != _firstTurnQueueFrameRightWing)
                    _firstTurnQueueFrameRightWing->Enable = true;
            }
        });

    // Disable
    for (size_t i = 0; i < 7; ++i)
    {
        if (nullptr != _turnQueueFrames[i])
            _turnQueueFrames[i]->Enable = false;
        if (nullptr != _turnQueuePortraits[i])
            _turnQueuePortraits[i]->Enable = false;
    }
    if (nullptr != _firstTurnQueueFrameLeftWing)
        _firstTurnQueueFrameLeftWing->Enable = false;
    if (nullptr != _firstTurnQueueFrameRightWing)
        _firstTurnQueueFrameRightWing->Enable = false;
}

enum class FindResult
{
    FIND,
    NOT_EXIST_IMAGE_ELEMENT,
    NOT_EXIST_GAME_OBJECT,
};

ImageElement* TurnQueueView::FindImageElementWithTag(const std::string& tag) const
{
    const GameObject& owner          = gameObject;
    Transform&        ownerTransform = owner.transform;
    FindResult        result         = FindResult::NOT_EXIST_GAME_OBJECT;
    ImageElement*     element        = nullptr;
    Transform::ForeachBFS(ownerTransform, [&tag, &element, &result](const Transform* transform) {
        if (result != FindResult::NOT_EXIST_GAME_OBJECT)
            return;
        if (GameObject& object = transform->gameObject; object.CompareTag(tag))
        {
            if (ImageElement* imageElement = object.GetComponent<ImageElement>(); nullptr != imageElement)
            {
                element = imageElement;
                result          = FindResult::FIND;
            }
            else
            {
                result = FindResult::NOT_EXIST_IMAGE_ELEMENT;
            }
        }
    });

    if (result == FindResult::NOT_EXIST_IMAGE_ELEMENT)
    {
        UmLogger.Log(LogLevel::LEVEL_WARNING,
                     tag + reinterpret_cast<const char*>(u8" GameObject에 ImageElement가 없습니다."));
    }
    else if (result == FindResult::NOT_EXIST_GAME_OBJECT)
    {
        UmLogger.Log(LogLevel::LEVEL_WARNING, tag + reinterpret_cast<const char*>(u8" GameObject를 찾을 수 없습니다."));
    }

    return element;
}

void TurnQueueView::FindFramesWithTag(const std::string& tag, const size_t index)
{
    _turnQueueFrames[index] = FindImageElementWithTag(tag + " Frame");
}

void TurnQueueView::FindPortraitsWithTag(const std::string& tag, const size_t index)
{
    _turnQueuePortraits[index] = FindImageElementWithTag(tag + " Portrait");
}

void TurnQueueView::InitializeFramesAndPortraits()
{
    FindFramesWithTag("1st Turn", 0);
    _firstTurnQueueFrameLeftWing = FindImageElementWithTag("1st Turn Frame Right");
    _firstTurnQueueFrameRightWing = FindImageElementWithTag("1st Turn Frame Left");
    FindPortraitsWithTag("1st Turn", 0);
    FindFramesWithTag("2nd Turn", 1);
    FindPortraitsWithTag("2nd Turn", 1);
    FindFramesWithTag("3rd Turn", 2);
    FindPortraitsWithTag("3rd Turn", 2);
    for (size_t i = 3; i < 7; ++i)
    {
        FindFramesWithTag(std::to_string(i + 1) + "th Turn", i);
        FindPortraitsWithTag(std::to_string(i + 1) + "th Turn", i);
    }
}