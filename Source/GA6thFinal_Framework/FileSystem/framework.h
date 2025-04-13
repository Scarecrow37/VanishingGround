#pragma once

#define WIN32_LEAN_AND_MEAN // 거의 사용되지 않는 내용을 Windows 헤더에서
                            // 제외합니다.

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <thread>

#include <system_error>

#include "Type/FileDataType.h"
#include "FileHelper.h"
#include "Extra/FileEventProcesser.h"
#include "Extra/FileEventNotifier.h"
#include "Core/FileObserver.h"
#include "Extra/FileContext.h"
#include "Core/FileSystem.h"