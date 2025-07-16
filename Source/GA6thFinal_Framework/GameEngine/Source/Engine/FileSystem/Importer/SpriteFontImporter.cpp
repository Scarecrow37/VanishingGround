#include "pch.h"
#include "SpriteFontImporter.h"

bool Impoerter::SpriteFontImporter::Initialize()
{
    UmFileSystem.RegisterFileEventSubscriber(this, {".ttf", ".otf"});
    return true;
}

void Impoerter::SpriteFontImporter::OnRequestedDragDrop(const File::Path& path) 
{
    File::Path extension = path.extension();
    if ("ttf" == extension)
    {
        EditorModule* editor = Global::editorModule;
        if (editor)
        {
        }
    }
}
