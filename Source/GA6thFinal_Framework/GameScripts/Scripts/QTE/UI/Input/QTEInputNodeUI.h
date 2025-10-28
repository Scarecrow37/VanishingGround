#pragma once

class GameObject;
class OverlayPanel;
class ImageElement;

namespace QTE
{
    class InputNodeUI
    {
    public:
        InputNodeUI(const File::Guid& prefab, Transform* parent);
        ~InputNodeUI();

    public:
        // 상위 오브젝트의 활성화 여부를 조작합니다.
        void Active(bool active);

        // UI 알파를 적용합니다.
        void Alpha(float alpha);

        // UI 상태를 초기화합니다.
        void Reset();

        void SetParent(Transform* parent);

        void SetImage(const File::Guid& guid);

    private:
        void SpawnObject(const File::Guid& prefab, Transform* parent);

    public:
        OverlayPanel* Overlay       = nullptr;
        ImageElement* ButtonImage   = nullptr;

    private:
        static constexpr const char* BUTTON_IMAGE_TAG = "QTE Button Image";

    };
}
