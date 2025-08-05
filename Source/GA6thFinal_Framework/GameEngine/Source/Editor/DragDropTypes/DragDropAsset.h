#pragma once

class DragDropAsset
{
public:
    static constexpr const char* KEY = "Asset";
    struct Data
    {
        std::weak_ptr<File::Context>* pContext = nullptr;
        File::Path* pPath = nullptr;
        File::Guid* pGuid = nullptr;
        const File::Path& GetPath() const;
        const File::Guid& GetGuid() const;
    };
};