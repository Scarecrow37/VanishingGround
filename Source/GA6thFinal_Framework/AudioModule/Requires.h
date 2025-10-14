#pragma once


#include <exception>
#include <string>
#include <filesystem>
#include <fstream>
#include <unordered_map>
#include <unordered_set>
#include <functional>
#include <span>

#pragma comment(lib, "xaudio2.lib")
#pragma comment(lib, "xapobase.lib")
#include <xaudio2.h>
#include <xaudio2fx.h>
#include <xapobase.h>
#include <xapofx.h>

#include "Exceptions.h"
#include "Types.h"
#include "AudioHandle.h"
#include "GroupHandle.h"
#include "EffectHandle.h"
#include "ReverbHandle.h"
#include "FadeHandle.h"
#include "Source.h"