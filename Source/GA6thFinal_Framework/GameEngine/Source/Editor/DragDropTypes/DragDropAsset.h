#pragma once

class DragDropAsset
{
public:
    static constexpr const char* KEY = "Asset";
    struct Data
    {
        std::weak_ptr<File::Context>* context;
    };
};