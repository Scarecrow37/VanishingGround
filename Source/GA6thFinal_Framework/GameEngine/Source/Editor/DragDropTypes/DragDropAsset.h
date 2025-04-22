#pragma once

class DragDropAsset
{
public:
    static constexpr const char* key = "Asset";
    struct Data
    {
        std::weak_ptr<File::FileContext> context;
    };
};