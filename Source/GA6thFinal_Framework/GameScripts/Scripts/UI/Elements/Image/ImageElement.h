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
    REFLECT_PROPERTY(FilePath, Alpha, Column, Row, ColumnIndex, RowIndex)

    GETTER_ONLY(std::string, FilePath) { return _guidRef.ToPath().string(); }
    PROPERTY(FilePath)

    GETTER(float, Alpha) { return ReflectFields->Alpha; }
    SETTER(float, Alpha)
    {
        const float clampedAlpha = std::clamp(value, 0.0f, 1.0f);
        ReflectFields->Alpha     = clampedAlpha;
        UpdateRendererAlpha(clampedAlpha);
    }
    PROPERTY(Alpha)

    GETTER(int, Column) { return ReflectFields->Column; }
    SETTER(int, Column)
    {
        ReflectFields->Column = std::max(1, ReflectFields->Column);
        if (_renderer)
            _renderer->SetAtlas(ReflectFields->Column, ReflectFields->Row);
    }
    PROPERTY(Column)

    GETTER(int, Row) { return ReflectFields->Row; }
    SETTER(int, Row)
    {
        ReflectFields->Row = std::max(1, ReflectFields->Row);
        if (_renderer)
            _renderer->SetAtlas(ReflectFields->Column, ReflectFields->Row);
    }
    PROPERTY(Row)

    GETTER(int, ColumnIndex) { return ReflectFields->ColumnIndex; }
    SETTER(int, ColumnIndex)
    {
        ReflectFields->ColumnIndex = std::clamp(value, 0, ReflectFields->Column - 1);
        if (_renderer) _renderer->SetAtlasIndex(ReflectFields->ColumnIndex, ReflectFields->RowIndex);
    }
    PROPERTY(ColumnIndex)

    GETTER(int, RowIndex) { return ReflectFields->RowIndex; }
    SETTER(int, RowIndex)
    {
        ReflectFields->RowIndex = std::clamp(value, 0, ReflectFields->Row - 1);
        if (_renderer) _renderer->SetAtlasIndex(ReflectFields->ColumnIndex, ReflectFields->RowIndex);
    }
    PROPERTY(RowIndex)

public:
    /// <summary>
    /// 이미지 파일을 지정된 GUID 참조로 설정합니다.
    /// </summary>
    /// <param name="guidRef">이미지 파일을 식별하는 File::GuidRef 참조입니다.</param>
    void SetImage(const File::Guid& guidRef);

    /// <summary>
    /// 선형 채우기 값을 설정합니다. 현재 좌우 채우기 모드에서만 적용됩니다.
    /// </summary>
    /// <param name="fill">설정할 선형 채우기 값입니다. 0.0f에서 1.0f 사이의 값을 가집니다.</param>
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
    void UpdateRendererSize(SIZE size) const;
    void UpdateRendererAlpha(float alpha) const;
    void RequestResource();

protected:
    REFLECT_FIELDS_BEGIN(DrawUIComponent)
    std::string Guid;
    float       Alpha = 1.0f;
    int         Column = 1;
    int         Row    = 1;
    int         ColumnIndex = 0;
    int         RowIndex    = 0;
    REFLECT_FIELDS_END(ImageElement)

private:
    std::unique_ptr<SpriteRenderer> _renderer;
    File::GuidRef                   _guidRef;
    Matrix                          _worldMatrix;
    SIZE                            _spriteOriginSize;

};