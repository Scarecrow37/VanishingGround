#include "pchScripts.h"
#include "HoldingProgressImageElement.h"

UMREAL_COMPONENT(HoldingProgressImageElement)

HoldingProgressImageElement::HoldingProgressImageElement() = default;

void HoldingProgressImageElement::BindProgressComplete(const ProgressCompleteCallback& callback)
{
    _progressCompleteCallback = callback;
}

void HoldingProgressImageElement::BeginHold()
{
    _isHolding = true;
}

void HoldingProgressImageElement::EndHold()
{
    _isHolding = false;
}

void HoldingProgressImageElement::ResetProgress()
{
    _elapsedTime = 0.0f;
    _isCompleted = false;
    ResetType();
}

void HoldingProgressImageElement::Reset()
{
    ImageElement::Reset();

    ResetType();
}

void HoldingProgressImageElement::Start()
{
    ImageElement::Start();

    UpdateType();
}

void HoldingProgressImageElement::Update()
{
    ImageElement::Update();

    if (false == _isCompleted)
    {
        if (_isHolding)
        {
            _elapsedTime += UmTime.DeltaTime();
        }
        else if (IsLeak)
        {
            _elapsedTime -= UmTime.DeltaTime();
        }

        const float holdDuration = HoldDuration;
        assert(holdDuration > 0.0f);

        _elapsedTime = std::clamp(_elapsedTime, 0.0f, holdDuration);

        const float t = std::clamp(_elapsedTime / holdDuration, 0.0f, 1.0f);

        switch (Type)
        {
        case ProgressType::OPACITY:
            Alpha = t;
            break;
        case ProgressType::LINEAR:
            LinearFill = t;
            break;
        case ProgressType::RADIAL:
            RadialFill = t;
            break;
        default:
            assert(false);
            break;
        }

        if (_elapsedTime >= holdDuration)
        {
            _elapsedTime = holdDuration;
            if (_progressCompleteCallback)
            {
                _progressCompleteCallback();
            }
            _isCompleted = true;
        }
    }
}

void HoldingProgressImageElement::UpdateType()
{
    switch (Type)
    {
    case ProgressType::OPACITY:
        Alpha = 0.0f;
        LinearFill = 1.0f;
        RadialFill = 1.0f;
        break;
    case ProgressType::LINEAR:
        Alpha = 1.0f;
        LinearFill = 0.0f;
        RadialFill = 1.0f;
        break;
    case ProgressType::RADIAL:
        Alpha = 1.0f;
        LinearFill = 1.0f;
        RadialFill = 0.0f;
        break;
    default:
        assert(false);
        break;
    }
}

void HoldingProgressImageElement::ResetType()
{
    Alpha      = 1.0f;
    LinearFill = 1.0f;
    RadialFill = 1.0f;
}