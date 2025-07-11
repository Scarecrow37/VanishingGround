#pragma once
#include "UI/Base/AreaUIComponent/AreaUIComponent.h"

class ImageElement : public AreaUIComponent
{
    USING_PROPERTY(ImageElement)

public:
    ImageElement();
    ImageElement(const ImageElement&) = delete;
    ImageElement& operator=(const ImageElement&) = delete;
    ImageElement(ImageElement&&)                 = delete;
    ImageElement& operator=(ImageElement&&)      = delete;
    ~ImageElement() override;

    REFLECT_PROPERTY(FilePath)
    GETTER_ONLY(std::string, FilePath) { return _guidRef.ToPath().string(); }
    PROPERTY(FilePath)

protected:
    REFLECT_FIELDS_BEGIN(AreaUIComponent)
    std::string Guid;
    REFLECT_FIELDS_END(ImageElement)

    void Reset() override;
    void DeserializedReflectEvent() override;

private:
    void LoadTexture();

    std::unique_ptr<SpriteRenderer> _renderer;
    File::GuidRef                   _guidRef;
};