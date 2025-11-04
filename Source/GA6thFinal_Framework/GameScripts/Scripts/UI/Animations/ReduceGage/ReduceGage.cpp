#include "pchScripts.h"
#include "ReduceGage.h"
#include "UI/Elements/Image/ImageElement.h"

UMREAL_COMPONENT(ReduceGage)

ReduceGage::ReduceGage() = default;

ReduceGage::~ReduceGage() = default;

void ReduceGage::StartReduceGage(ImageElement* imageElement, const float targetRate)
{
    ResetReduce();

    _imageElement = imageElement;
    _targetRate   = targetRate;
}

void ReduceGage::Update()
{
    if (_imageElement)
    {
        // 피가 회복되는 경우 처리
        if (_targetRate > _currentRate)
        {
            _currentRate = _targetRate;
            _imageElement->SetLinearFill(_currentRate);
            return;
        }
        else if (!_isReducing && _currentRate > _targetRate)
        {
            _reduceTimer += UmTime.DeltaTime();

            if (_reduceTimer >= 1.f)
            {
                _isReducing   = true;
                _reduceTimer  = 0.0f;
                _reduceFactor = 0.f;
                _tempRate     = _currentRate;
            }
        }        

        if (_isReducing)
        {
            _reduceTimer += UmTime.DeltaTime();

            if (_reduceTimer >= 1.f)
            {                
                _currentRate = _targetRate;
                ResetReduce();
            }
            else
            {
                _reduceFactor = Mathf::Ease(Mathf::EASE_OUT, Mathf::EXPO, 0.5f, _reduceTimer);
                _currentRate  = std::lerp(_tempRate, _targetRate, _reduceFactor);
            }
            
            _imageElement->SetLinearFill(_currentRate);
        }
    }
}

void ReduceGage::ResetReduce()
{
    _reduceTimer  = 0.0f;
    _tempRate     = 0.f;
    _reduceFactor = 0.f;
    _isReducing   = false;
}