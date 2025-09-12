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

            if (resolution.cx == prevMode.Width && resolution.cy == prevMode.Height)
            {
                resolution.cx  = newMode.Width;
                resolution.cy = newMode.Height;

                resource->ResizeResource(resolution);
            }
        }
    }
}