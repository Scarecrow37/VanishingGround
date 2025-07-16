#pragma once

namespace Impoerter
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
    };
}
