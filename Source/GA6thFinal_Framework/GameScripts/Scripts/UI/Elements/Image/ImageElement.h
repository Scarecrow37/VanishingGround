#pragma once
#include "UI/Base/DrawUIComponent/DrawUIComponent.h"

class ImageElement : public DrawUIComponent
{
    USING_PROPERTY(ImageElement)

public:
    ImageElement();
    ImageElement(const ImageElement&)            = delete;
    ImageElement& operator=(const ImageElement&) = delete;
    ImageElement(ImageElement&&)                 = delete;
    ImageElement& operator=(ImageElement&&)      = delete;
    ~ImageElement() override;

public:
    REFLECT_PROPERTY(FilePath)
    GETTER_ONLY(std::string, FilePath) { return _guidRef.ToPath().string(); }
    PROPERTY(FilePath)

public:
    void SetImage(const File::GuidRef& guidRef);

protected:
    void  Reset() override;
    void  DeserializedReflectEvent() override;
    void  OnPlacementChange() override;
    float GetZOrder() const override;
    void  SetViewOrder(int viewOrder) override;

private:
    void LoadTexture() const;
    void LoadTexture(const File::GuidRef& guid) const;
    void UpdateWorldMatrix();
    void RequestResource();

protected:
    REFLECT_FIELDS_BEGIN(DrawUIComponent)
    std::string Guid;
    REFLECT_FIELDS_END(ImageElement)

private:
    std::unique_ptr<SpriteRenderer> _renderer;
    File::GuidRef                   _guidRef;
    Matrix                          _worldMatrix;

};