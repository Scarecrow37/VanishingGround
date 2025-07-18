#pragma once

namespace Importer
{
    class SpriteFontImporter 
        : public File::FileEventSubscriber
    {
        typedef int (*MakeSpriteFont)(const std::vector<std::wstring>& options);
    public:
        SpriteFontImporter() = default;
        ~SpriteFontImporter() override;

    public:
        void Initialize();

    private:
        /// <summary>
        /// 파일 드래그 드롭 이벤트를 처리하는 함수입니다.
        /// </summary>
        /// <param name="path">드롭된 파일 경로</param>
        void OnRequestedDragDrop(const File::Path& path) override;

        /// <summary>
        /// ImGui를 사용하여 스프라이트 폰트 임포트 설정 창을 출력합니다.
        /// </summary>
        void DrawImGuiImportSetting();


        /// <summary>
        /// 폰트를 가져옵니다.
        /// </summary>
        void ImportFont();

    private:
        HMODULE _moduleHandle;
        MakeSpriteFont _makeSpriteFont;
        std::vector<std::wstring> _options;

        File::Path _sourcePath;
        File::Path _importPath; // 임포트할 폰트의 대상 경로

        char _defaultCharacter = 0;
        int _fontSize = 23; // 폰트 크기
        bool _isBold   = false; // 볼드체 여부
        bool _isItalic = false; // 이탤릭체 여부
    };
}
