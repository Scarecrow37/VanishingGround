#include "pch.h"
#include "GraphicsCore.h"
#include "Box.h"
#include "Cylinder.h"
#include "GeoSphere.h"
#include "Grid.h"
#include "Model.h"
#include "Quad.h"
#include "Sphere.h"

void GraphicsCore::Initialize(HWND hwnd, UINT width, UINT height, FEATURE_LEVEL feature)
{
    Device.SetUpDevice(hwnd, width, height, feature);
    ViewManager.Initialize();
    Device.Initialize();
    Device.ResetCommands();
    Renderer.Initialize();

    auto commandList = Device.GetCommandList().Get();
    commandList->Close();
    Device.RegisterCommand(commandList);
    Device.ExecuteCommand();
    Device.GPUSync();

    //MainCamera.SetupPerspective(45.f, static_cast<float>(width) / height, 0.01f, 10000.f);
    //MainCamera.SetPosition({0.f, 0.f, -5.f});

    //InitializeDefaultGeometry();
    UmDevice.ResetCommands();
}

void GraphicsCore::UpdateAnimation(const float deltaTime) {}

void GraphicsCore::Update()
{
    // 임시 카메라
    // MainCamera.Update();
    Renderer.Update();
}

void GraphicsCore::Render()
{
    Renderer.Render();
}

void GraphicsCore::Flip()
{
    Renderer.Flip();
}


void GraphicsCore::InitializeDefaultGeometry()
{
    std::unique_ptr<Box> box = std::make_unique<Box>();
    box->Initialize(1.f, 1.f, 1.f);

    std::unique_ptr<Sphere> sphere = std::make_unique<Sphere>();
    sphere                         = std::make_unique<Sphere>();
    sphere->Initialize(1.f, 20, 20);

    std::unique_ptr<GeoSphere> geoSphere = std::make_unique<GeoSphere>();
    geoSphere                            = std::make_unique<GeoSphere>();
    geoSphere->Initialize(1.f, 5);

    std::unique_ptr<Cylinder> cylinder = std::make_unique<Cylinder>();
    cylinder                           = std::make_unique<Cylinder>();
    cylinder->Initialize(0.5f, 0.3f, 2.f, 20, 20);

    std::unique_ptr<Grid> grid = std::make_unique<Grid>();
    grid                       = std::make_unique<Grid>();
    grid->Initialize(20.f, 30.f, 4, 4);

    std::unique_ptr<Quad> quad = std::make_unique<Quad>();
    quad                       = std::make_unique<Quad>();
    quad->Initialize(-1.0f, 1.0f, 2.0f, 2.0f, 0.0f);

    std::shared_ptr<Model>    geometry;
    std::unique_ptr<BaseMesh> baseMesh;

    baseMesh = std::move(box);
    geometry = std::make_shared<Model>();
    geometry->AddMesh(std::move(baseMesh));
    _geometries.push_back(geometry);
    UmResourceManager.AddResource(L"Box", geometry);

    baseMesh = std::move(cylinder);
    geometry = std::make_shared<Model>();
    geometry->AddMesh(std::move(baseMesh));
    _geometries.push_back(geometry);
    UmResourceManager.AddResource(L"Cylinder", geometry);

    baseMesh = std::move(sphere);
    geometry = std::make_shared<Model>();
    geometry->AddMesh(std::move(baseMesh));
    _geometries.push_back(geometry);
    UmResourceManager.AddResource(L"Sphere", geometry);

    baseMesh = std::move(geoSphere);
    geometry = std::make_shared<Model>();
    geometry->AddMesh(std::move(baseMesh));
    _geometries.push_back(geometry);
    UmResourceManager.AddResource(L"GeoSphere", geometry);

    baseMesh = std::move(grid);
    geometry = std::make_shared<Model>();
    geometry->AddMesh(std::move(baseMesh));
    _geometries.push_back(geometry);
    UmResourceManager.AddResource(L"Grid", geometry);

    baseMesh = std::move(quad);
    geometry = std::make_shared<Model>();
    geometry->AddMesh(std::move(baseMesh));
    _geometries.push_back(geometry);
    UmResourceManager.AddResource(L"Quad", geometry);
}