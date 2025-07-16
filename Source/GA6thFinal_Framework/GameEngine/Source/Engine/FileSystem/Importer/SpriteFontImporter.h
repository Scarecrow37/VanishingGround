#pragma once

namespace Importer
{
    class SpriteFontImporter 
        : public File::FileEventSubscriber
    {
    public:
        SpriteFontImporter()  = default;
        ~SpriteFontImporter() = default;

    public:
        bool Initialize();

    private:
        void OnRequestedDragDrop(const File::Path& path) override;

        /// <summary>
        /// ImGui를 사용하여 스프라이트 폰트 임포트 설정 창을 출력합니다.
        /// </summary>
        void DrawImGuiImportSetting();

    private:
        File::Path _importPath; // 임포트할 폰트의 대상 경로

        int _fontSize = 32; // 폰트 크기
        ImColor _fontColor = ImColor(255, 255, 255, 255); 
    };
}
