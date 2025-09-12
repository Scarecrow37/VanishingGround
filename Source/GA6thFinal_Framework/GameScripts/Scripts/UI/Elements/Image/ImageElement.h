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
    /// <summary>
    /// 이미지 파일을 지정된 GUID 참조로 설정합니다.
    /// </summary>
    /// <param name="guidRef">이미지 파일을 식별하는 File::GuidRef 참조입니다.</param>
    void SetImage(const File::GuidRef& guidRef);
    void SetLinearFill(float fill);

    /// <summary>
    /// 스프라이트의 크기로 값을 초기화합니다.
    /// </summary>
    void ResetToSpriteSize();

protected:
    void  Reset() override;
    void  DeserializedReflectEvent() override;
    float GetZOrder() const override;
    void  SetViewOrder(int viewOrder) override;
    void  ImGuiDrawPropertysEvent() override;

    SIZE MeasureOverride(SIZE availableSize) override;
    SIZE ArrangeOverride(SIZE finalSize) override;


private:
    void LoadTexture(const File::GuidRef& guid) const;
    void UpdateWorldMatrix();
    void UpdateRendererSize(const SIZE size) const;
    void RequestResource();

protected:
    REFLECT_FIELDS_BEGIN(DrawUIComponent)
    std::string Guid;
    REFLECT_FIELDS_END(ImageElement)

private:
    std::unique_ptr<SpriteRenderer> _renderer;
    File::GuidRef                   _guidRef;
    Matrix                          _worldMatrix;
    SIZE                            _spriteOriginSize;

};