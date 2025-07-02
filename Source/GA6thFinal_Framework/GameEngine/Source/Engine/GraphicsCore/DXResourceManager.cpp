#include "pch.h"
#include "DXResourceManager.h"
#include "ResourceBase.h"

DXResourceManager::DXResourceManager() {}

DXResourceManager::~DXResourceManager() {}

void DXResourceManager::ResizeResource(DXGI_MODE_DESC prevMode)
{
    const auto& newMode = UmDevice.GetMode();

    for (auto& [typeID, resources] : _resources)
    {
        for (auto& resource : resources)
        {
            auto mode = resource->GetMode();

            if (mode.Width == prevMode.Width && mode.Height == prevMode.Height)
            {
                mode.Width  = newMode.Width;
                mode.Height = newMode.Height;

                resource->ResizeResource(mode);
            }
        }
    }
}