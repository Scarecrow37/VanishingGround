#pragma once
#include "UI/Base/EditablePlacementUIComponent/EditablePlacementUIComponent.h"

class ImageElement : public EditablePlacementUIComponent
{
    USING_PROPERTY(ImageElement)

public:
    ImageElement();
    ImageElement(const ImageElement&) = delete;
    ImageElement& operator=(const ImageElement&) = delete;
    ImageElement(ImageElement&&)                 = delete;
    ImageElement& operator=(ImageElement&&)      = delete;
    ~ImageElement() override;

public:
    REFLECT_PROPERTY(FilePath)
    GETTER_ONLY(std::string, FilePath) { return _guidRef.ToPath().string(); }
    PROPERTY(FilePath)

protected:
    void Reset() override;
    void DeserializedReflectEvent() override;
    void OnPlacementChange() override;
    float GetZOrder() const override;

private:
    void OnSetViewOrder() override;
    void LoadTexture() const;
    void UpdateWorldMatrix();

protected:
    REFLECT_FIELDS_BEGIN(EditablePlacementUIComponent)
    std::string Guid;
    REFLECT_FIELDS_END(ImageElement)


    std::unique_ptr<SpriteRenderer> _renderer;
    File::GuidRef                   _guidRef;
    Matrix                          _worldMatrix;
};