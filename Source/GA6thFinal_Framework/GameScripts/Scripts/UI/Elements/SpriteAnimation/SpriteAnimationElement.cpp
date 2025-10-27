#include "pchScripts.h"
#include "SpriteAnimationElement.h"

UMREAL_COMPONENT(SpriteAnimationElement)

void SpriteAnimationElement::Start()
{
    ImageElement::Start();

    Setup();
}

void SpriteAnimationElement::Update()
{
    ImageElement::Update();

    if (_isPlaying) UpdateFrame();
}

void SpriteAnimationElement::Setup()
{
    ResetUV();
    _elapsedTime = 0.0f;
    const int frameCount = FrameCount;
    _durationPerFrame = ReflectFields->Duration / static_cast<float>(frameCount);
    _currentFrame        = 0;
    _isPlaying           = ReflectFields->StartAnimationOnPlay | _isPlaying;
}

void SpriteAnimationElement::ResetUV()
{
    ReflectFields->Basefields.get().ColumnIndex = 0;
    ReflectFields->Basefields.get().RowIndex = 0;
}

void SpriteAnimationElement::UpdateFrame()
{
    _elapsedTime += UmTime.DeltaTime();

    if (const float totalDuration = ReflectFields->Duration; _elapsedTime >= totalDuration)
    {
        if (const bool loop = ReflectFields->Loop; loop)
        {
            _elapsedTime = std::fmod(_elapsedTime, totalDuration);
        }
        else
        {
            _elapsedTime = totalDuration;
            _isPlaying   = false;
            if (_willSuicide)
                GameObject::Destroy(this->gameObject);
            return;
        }
    }

    _currentFrame = static_cast<int>(_elapsedTime / _durationPerFrame);

    ReflectFields->Basefields.get().RowIndex = _currentFrame / ReflectFields->Basefields.get().Column;
    ReflectFields->Basefields.get().ColumnIndex = _currentFrame % ReflectFields->Basefields.get().Column;

    UpdateAtlasIndex();
}

void SpriteAnimationElement::StartAnimation()
{
    _isPlaying = true;
}

void SpriteAnimationElement::StopAnimation()
{
    _isPlaying = false;
}