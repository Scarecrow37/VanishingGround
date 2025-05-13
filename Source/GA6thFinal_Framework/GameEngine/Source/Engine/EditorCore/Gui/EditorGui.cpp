#include "pch.h"

bool EditorGui::SerializeFromData(EditorToolSerializeData* data)
{
    if (nullptr == data)
        return false;

    data->Class     = typeid(*this).name(); 
    data->IsVisible = IsVisible();

    return true;
}

bool EditorGui::DeSerializeFromData(EditorToolSerializeData* data)
{
    if (nullptr == data)
        return false;

    SetVisible(data->IsVisible);

    return true;
}
