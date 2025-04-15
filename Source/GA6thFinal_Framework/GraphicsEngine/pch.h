// pch.h: 미리 컴파일된 헤더 파일입니다.
// 아래 나열된 파일은 한 번만 컴파일되었으며, 향후 빌드에 대한 빌드 성능을 향상합니다.
// 코드 컴파일 및 여러 코드 검색 기능을 포함하여 IntelliSense 성능에도 영향을 미칩니다.
// 그러나 여기에 나열된 파일은 빌드 간 업데이트되는 경우 모두 다시 컴파일됩니다.
// 여기에 자주 업데이트할 파일을 추가하지 마세요. 그러면 성능이 저하됩니다.

#ifndef PCH_H
#define PCH_H
#define DIRECTX_TOOLKIT_IMPORT

// 여기에 미리 컴파일하려는 헤더 추가
#include <dwrite.h>
#include <d3d11.h>
#include <directxtk/SimpleMath.h>
#include <comdef.h>
#include <d3dcompiler.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

using namespace DirectX;
using namespace DirectX::SimpleMath;

#include <fstream>
#include <typeindex>
#include <vector>
#include <unordered_map>
#include <queue>
#include <string>
#include <filesystem>
#include <memory>
#include <thread>
#include <condition_variable>
#include <ranges>
#include <algorithm>

#include "framework.h"

#include "Utility.h"

#include "Extern.h"
#include "Struct.h"
#include "Enum.h"
#include "includes/Matrix4x4.h"

// Global
#include "GraphicDevice.h"
#include "ResourceManager.h"
#include "ConstantBuffer.h"
#include "RenderGroup.h"
#include "PostProcessSystem.h"
#include "ViewManagement.h"
#include "StateManagement.h"
#include "Quad.h"
#include "Sprite.h"

#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dcompiler.lib")
#pragma comment (lib, "dwrite.lib")

#endif //PCH_H