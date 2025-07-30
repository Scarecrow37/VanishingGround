#include "pchScripts.h"
#include "TurnQueueView.h"

#include "ViewModels/TurnQueue/TurnQueueViewModel.h"
#include "UI/Elements/Image/ImageElement.h"

TurnQueueView::TurnQueueView() = default;

void TurnQueueView::Reset()
{
    Component::Reset();

    InitializeFramesAndPortraits();

    UmTime.Invoke(this, 0, [this] {
        UmWatcher.Watch<TurnQueueViewModel, std::vector<TurnUIData>>(
            "TestValue", [this](const std::vector<TurnUIData>& value) {
                const size_t dataSize = value.size();
                for (size_t i = 0; i < 7; ++i)
                {
                    if (i < dataSize)
                    {
                        _turnQueueFrames[i]->SetImage(value[i].Frame);
                        _turnQueuePortraits[i]->SetImage(value[i].ActorPortrait);
                    }
                    else
                    {
                        _turnQueueFrames[i]->SetImage(File::GuidRef());
                        _turnQueuePortraits[i]->SetImage(File::GuidRef());
                    }
                }
            });
    });
}

void TurnQueueView::FindImageElementWithTag(const std::string& tag, std::array<ImageElement*, 7>& elements,
                                            const size_t index)
{
    if (const std::weak_ptr<GameObject> taggedObject = GameObject::FindWithTag(tag); taggedObject.expired())
    {
        UmLogger.Log(LogLevel::LEVEL_WARNING, tag + " GameObject를 찾을 수 없습니다.");
    }
    else
    {
        const auto    object       = taggedObject.lock();
        ImageElement* imageElement = object->GetComponent<ImageElement>();
        if (nullptr == imageElement)
        {
            UmLogger.Log(LogLevel::LEVEL_WARNING, tag + " GameObject에 ImageElement가 없습니다.");
            return;
        }
        elements[index] = imageElement;
    }
}

void TurnQueueView::FindFramesWithTag(const std::string& tag, const size_t index)
{
    FindImageElementWithTag(tag + " Frame", _turnQueueFrames, index);
}

void TurnQueueView::FindPortraitsWithTag(const std::string& tag, const size_t index)
{
    FindImageElementWithTag(tag + " Portrait", _turnQueuePortraits, index);
}

void TurnQueueView::InitializeFramesAndPortraits()
{
    FindFramesWithTag("1st Turn", 0);
    FindFramesWithTag("2nd Turn", 1);
    FindFramesWithTag("3rd Turn", 2);
    for (size_t i = 3; i < 7; ++i)
    {
        FindFramesWithTag(std::to_string(i + 1) + "th Turn", i);
    }
}