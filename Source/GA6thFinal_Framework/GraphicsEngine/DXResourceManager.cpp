#include "pch.h"
#include "DXResourceManager.h"
#include "ResourceBase.h"

DXResourceManager::DXResourceManager() {}

DXResourceManager::~DXResourceManager() {}

void DXResourceManager::ResizeResource(DXGI_MODE_DESC prevMode)
{
    const auto& newMode = Global::device->GetMode();

    for (auto& [typeID, resources] : _resources)
    {
        for (auto& resource : resources)
        {
            auto resolution = resource->GetResolution();

            if (resolution.Width == prevMode.Width && resolution.Height == prevMode.Height)
            {
                resolution.Width  = newMode.Width;
                resolution.Height = newMode.Height;

                resource->ResizeResource(resolution);
            }
        }
    }
}