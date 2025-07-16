#include "pch.h"
#include "SpriteFontImporter.h"

bool Impoerter::SpriteFontImporter::Initialize()
{
    UmFileSystem.RegisterFileEventSubscriber(this, {".ttf", ".otf"});

    return true;
}
