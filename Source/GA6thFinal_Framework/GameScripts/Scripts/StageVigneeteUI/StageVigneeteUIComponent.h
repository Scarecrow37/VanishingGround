#pragma once
class ImageElement;

class StageVigneeteUIComponent : public Component
{
    USING_PROPERTY(StageVigneeteUIComponent)

public:
    StageVigneeteUIComponent();
    ~StageVigneeteUIComponent() override;

public:
    void SetActiveVigneeteImages(bool value);

    REFLECT_PROPERTY()

protected:
    REFLECT_FIELDS_BEGIN(Component)
    REFLECT_FIELDS_END(StageVigneeteUIComponent)

private:
    void Awake() override;
    void Start() override;
    std::vector<ImageElement*> _vigneeteImages;

    void FindImages();
    void UpdateVigneete();
};

