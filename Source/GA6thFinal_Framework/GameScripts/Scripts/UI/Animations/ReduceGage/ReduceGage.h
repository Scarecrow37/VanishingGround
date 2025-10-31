#pragma once

class ImageElement;
class ReduceGage : public Component
{
    USING_PROPERTY(ReduceGage)

public:
    ReduceGage();
    ~ReduceGage() override;

public:
    REFLECT_PROPERTY()

public:
    void StartReduceGage(ImageElement* imageElement, const float targetRate);

protected:
    REFLECT_FIELDS_BEGIN(Component)
    REFLECT_FIELDS_END(ReduceGage)

    void Update() override;

private:
    void ResetReduce();

private:
    ImageElement* _imageElement = nullptr;
    float         _reduceTimer  = 0.f;
    float         _targetRate   = 0.f;
    float         _currentRate  = 1.f;
    float         _tempRate     = 0.f;
    float         _reduceFactor = 0.f;
    bool          _isReducing   = false;
};