#pragma once

namespace Importer
{
    class SpriteFontImporter : public File::FileEventSubscriber
    {
        typedef int (*MakeSpriteFont)(const std::vector<std::wstring>& options);

        static constexpr char INITIAL_DEFAULT_CHARACTER = 0;
        static constexpr int  INITIAL_FONT_SIZE         = 23;
        static constexpr int  INITIAL_LINE_SPACING      = 0;
        static constexpr int  INITIAL_CHARACTER_SPACING = 0;

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

        void Reset();

    private:
        HMODULE                   _moduleHandle;
        MakeSpriteFont            _makeSpriteFont;
        std::vector<std::wstring> _options;

        File::Path _sourcePath;
        File::Path _importPath; // 임포트할 폰트의 대상 경로

        char _defaultCharacter[2] = {INITIAL_DEFAULT_CHARACTER, 0}; // 기본 문자 (문자가 없을 경우 대신 출력될 문자)
        int  _fontSize            = INITIAL_FONT_SIZE;              // 폰트 크기
        bool _isBold              = false;                          // 볼드체 여부
        bool _isItalic            = false;                          // 이탤릭체 여부
        bool _isUnderline         = false;                          // 밑줄 여부
        bool _isStrikeout         = false;                          // 취소선 여부
        int  _lineSpacing         = INITIAL_LINE_SPACING;           // 줄 간격
        int  _characterSpacing    = INITIAL_CHARACTER_SPACING;      // 문자 간격
        bool _isSharp             = false; // 샤프 모드 여부 (True: 샤프 모드, False: 일반 모드)
    };
} // namespace Importer
