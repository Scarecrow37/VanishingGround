#include "pchScripts.h"
#include "FadeHelper.h"

float Fader::Fade()
{
    switch (_fadeMode)
    {
    case Fader::FADE_NONE:
        break;
    case Fader::FADE_IN: {
        if (IsFadeInEnd())
        {
            return 1.0f;
        }
        else
        {
            FadeIn();
            break;
        }
    }
    case Fader::FADE_OUT: {
        if (IsFadeOutEnd())
        {
            return 0.0f;
        }
        else
        {
            FadeOut();
            break;
        }
    }
    default:
        break;
    }
    return _fadeFactor;
}

float Fader::FadeIn()
{
    _timer += UmTime.DeltaTime();
    _timer = std::min(_timer, _duration);

    float factor = std::min(_timer / _duration, 1.0f);
    _fadeFactor  = Mathf::Ease(_fadeInEaseType, _fadeInFuncType, 0.5f, factor);
    if (IsFadeInEnd() && _onFadeInEndCallback)
    {
        _onFadeInEndCallback();
    }
    return _fadeFactor;
}

float Fader::FadeOut()
{
    _timer -= UmTime.DeltaTime();
    _timer = std::max(_timer, 0.0f);

    float factor = std::max(_timer / _duration, 0.0f);
    _fadeFactor  = Mathf::Ease(_fadeOutEaseType, _fadeOutFuncType, 0.5f, factor);
    if (IsFadeOutEnd() && _onFadeOutEndCallback)
    {
        _onFadeOutEndCallback();
    }
    return _fadeFactor;
}