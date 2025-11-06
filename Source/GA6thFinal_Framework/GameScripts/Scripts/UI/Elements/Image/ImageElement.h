#pragma once
#include "UI/Base/DrawUIComponent/DrawUIComponent.h"
#include "UI/Base/IOpacity/IOpacity.h"

class ISpriteRenderer;
class ImageElement : public DrawUIComponent, public IOpacity
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
    REFLECT_PROPERTY(FilePath, FileGuid, Alpha, Column, Row, ColumnIndex, RowIndex, LinearFill, RadialFill)

    GETTER_ONLY(std::string, FilePath) { return _Guid.ToPath().string(); }
    PROPERTY(FilePath)

    GETTER_ONLY(std::string, FileGuid) { return _Guid.string(); }
    PROPERTY(FileGuid)

    GETTER(float, Alpha) { return ReflectFields->Alpha; }
    SETTER(float, Alpha) { SetOpacity(value); }
    PROPERTY(Alpha)

    GETTER(int, Column) { return ReflectFields->Column; }
    SETTER(int, Column)
    {
        ReflectFields->Column = std::max(1, value);
        UpdateAtlas();
        const int columnIndex = ColumnIndex;
        ColumnIndex           = columnIndex; // to clamp
    }
    PROPERTY(Column)

    GETTER(int, Row) { return ReflectFields->Row; }
    SETTER(int, Row)
    {
        ReflectFields->Row = std::max(1, value);
        UpdateAtlas();
        const int rowIndex = RowIndex;
        RowIndex           = rowIndex; // to clamp
    }
    PROPERTY(Row)

    GETTER(int, ColumnIndex) { return ReflectFields->ColumnIndex; }
    SETTER(int, ColumnIndex)
    {
        ReflectFields->ColumnIndex = std::clamp(value, 0, ReflectFields->Column - 1);
        UpdateAtlasIndex();
    }
    PROPERTY(ColumnIndex)

    GETTER(int, RowIndex) { return ReflectFields->RowIndex; }
    SETTER(int, RowIndex)
    {
        ReflectFields->RowIndex = std::clamp(value, 0, ReflectFields->Row - 1);
        UpdateAtlasIndex();
    }
    PROPERTY(RowIndex)

    GETTER(float, LinearFill) { return ReflectFields->LinearFill; }
    SETTER(float, LinearFill)
    {
        const float clampedFill = std::clamp(value, 0.0f, 1.0f);
        ReflectFields->LinearFill = clampedFill;
        SetLinearFill(clampedFill);
    }
    PROPERTY(LinearFill)

    GETTER(float, RadialFill) { return ReflectFields->RadialFill; }
    SETTER(float, RadialFill)
    {
        const float clampedFill = std::clamp(value, 0.0f, 1.0f);
        ReflectFields->RadialFill = clampedFill;
        SetRadialFill(clampedFill);
    }
    PROPERTY(RadialFill)

public:
    /// <summary>
    /// 이미지 파일을 지정된 GUID 참조로 설정합니다.
    /// </summary>
    /// <param name="Guid">이미지 파일을 식별하는 File::Guid 참조입니다.</param>
    void SetImage(const File::Guid& Guid);

    /// <summary>
    /// 선형 채우기 값을 설정합니다. 현재 좌우 채우기 모드에서만 적용됩니다.
    /// </summary>
    /// <param name="fill">설정할 선형 채우기 값입니다. 0.0f에서 1.0f 사이의 값을 가집니다.</param>
    void SetLinearFill(float fill) const;

    /// <summary>
    /// 방사형 채움(라디얼 필) 값을 설정합니다.
    /// </summary>
    /// <param name="fill">채워질 비율을 나타내는 부동 소수점 값. 일반적으로 0.0(비어 있음)에서 1.0(완전 채움) 범위로 사용됩니다.</param>
    void SetRadialFill(float fill) const;

    /// <summary>
    /// 스프라이트의 크기로 값을 초기화합니다.
    /// </summary>
    void ResetToSpriteSize();

    void SetOpacity(float opacity) override;

protected:
    void  Reset() override;
    void  DeserializedReflectEvent() override;
    float GetZOrder() const override;
    void  SetViewOrder(int viewOrder) override;
    void  ImGuiDrawPropertysEvent() override;

    SIZE MeasureOverride(SIZE availableSize) override;
    SIZE ArrangeOverride(SIZE finalSize) override;

    void UpdateAtlas();
    void UpdateAtlasIndex();


private:
    void LoadTexture(const File::Guid& guid) const;
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
    float       LinearFill  = 1.0f;
    float       RadialFill  = 1.0f;
    REFLECT_FIELDS_END(ImageElement)

private:
    GraphicsPointer<ISpriteRenderer> _renderer;
    Matrix                           _worldMatrix;
    SIZE                             _spriteOriginSize;
    File::Guid                       _Guid;

};