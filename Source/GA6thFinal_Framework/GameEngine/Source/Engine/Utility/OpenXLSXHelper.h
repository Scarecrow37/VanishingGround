#pragma once

#pragma warning(disable : 4244)
#pragma warning(disable : 4267)

#ifdef _DEBUG
#pragma comment(lib, "OpenXLSXd.lib")
#else
#pragma comment(lib, "OpenXLSX.lib")
#endif

#include "OpenXLSX/OpenXLSX.hpp"

#pragma warning(default : 4244)
#pragma warning(default : 4267)

namespace OpenXLSXHelper
{

}