#pragma once

#include "PrimitiveBatch.h"
#include "../DirectXColors.h"
#include "CommonStates.h"
#include "DirectXCollision.h"
#include "Effects.h"
#include "VertexTypes.h"

void XM_CALLCONV Draw(DirectX::PrimitiveBatch<DirectX::VertexPositionColor>* batch, const DirectX::BoundingSphere& sphere,
                      DirectX::FXMVECTOR color = DirectX::Colors::White);

void XM_CALLCONV Draw(DirectX::PrimitiveBatch<DirectX::VertexPositionColor>* batch, const DirectX::BoundingBox& box,
                      DirectX::FXMVECTOR color = DirectX::Colors::White);

void XM_CALLCONV Draw(DirectX::PrimitiveBatch<DirectX::VertexPositionColor>* batch,
                      const DirectX::BoundingOrientedBox& obb, DirectX::FXMVECTOR color = DirectX::Colors::White);

void XM_CALLCONV Draw(DirectX::PrimitiveBatch<DirectX::VertexPositionColor>* batch,
                      const DirectX::BoundingFrustum& frustum, DirectX::FXMVECTOR color = DirectX::Colors::White);

void XM_CALLCONV DrawGrid(DirectX::PrimitiveBatch<DirectX::VertexPositionColor>* batch, DirectX::FXMVECTOR xAxis,
                          DirectX::FXMVECTOR yAxis, DirectX::FXMVECTOR origin, size_t xdivs, size_t ydivs,
                          DirectX::GXMVECTOR color = DirectX::Colors::White);

void XM_CALLCONV DrawRing(DirectX::PrimitiveBatch<DirectX::VertexPositionColor>* batch, DirectX::FXMVECTOR origin,
                          DirectX::FXMVECTOR majorAxis, DirectX::FXMVECTOR minorAxis,
                          DirectX::GXMVECTOR color = DirectX::Colors::White);

void XM_CALLCONV DrawRay(DirectX::PrimitiveBatch<DirectX::VertexPositionColor>* batch, DirectX::FXMVECTOR origin,
                         DirectX::FXMVECTOR direction, bool normalize = true,
                         DirectX::FXMVECTOR color = DirectX::Colors::White);

void XM_CALLCONV DrawTriangle(DirectX::PrimitiveBatch<DirectX::VertexPositionColor>* batch, DirectX::FXMVECTOR pointA,
                              DirectX::FXMVECTOR pointB, DirectX::FXMVECTOR pointC,
                              DirectX::GXMVECTOR color = DirectX::Colors::White);

void XM_CALLCONV DrawQuad(DirectX::PrimitiveBatch<DirectX::VertexPositionColor>* batch, DirectX::FXMVECTOR pointA,
                          DirectX::FXMVECTOR pointB, DirectX::FXMVECTOR pointC, DirectX::GXMVECTOR pointD,
                          DirectX::HXMVECTOR color = DirectX::Colors::White);

void XM_CALLCONV DrawLine(DirectX::PrimitiveBatch<DirectX::VertexPositionColor>* batch, DirectX::FXMVECTOR pointA,
                          DirectX::FXMVECTOR pointB, DirectX::FXMVECTOR color = DirectX::Colors::White);

void XM_CALLCONV DrawSpotLight(DirectX::PrimitiveBatch<DirectX::VertexPositionColor>* batch,
                               DirectX::FXMVECTOR position, DirectX::FXMVECTOR direction, float range, float innerCone,
                               float outerCone, size_t numSegments = 24,
                               DirectX::GXMVECTOR color = DirectX::Colors::White);

void XM_CALLCONV DrawDebugGrid(DirectX::PrimitiveBatch<DirectX::VertexPositionColor>* batch,
                               DirectX::FXMVECTOR cameraPosition, float farZ, size_t linesPerSide = 40,
                               DirectX::GXMVECTOR color = DirectX::Colors::White);

void XM_CALLCONV DrawCircle(DirectX::PrimitiveBatch<DirectX::VertexPositionColor>* batch, DirectX::FXMVECTOR origin,
                            float radius, DirectX::FXMVECTOR color = DirectX::Colors::White);
