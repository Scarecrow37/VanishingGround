#pragma once
#include <Windows.h>

#ifdef UMREALSCRIPTS_EXPORT
#define UMREALSCRIPTS_DECLSPEC __declspec(dllexport)
//dllexport는 함수 정의로 대체.        
#define UMREALSCRIPT_NEWCOMPONENT(CLASS_NAME)   \
{                                               \
    return new CLASS_NAME();                    \
}  
#else
#define UMREALSCRIPTS_DECLSPEC __declspec(dllimport)    
//dllimport는 함수 선언으로 대체.
#define UMREALSCRIPT_NEWCOMPONENT(CLASS_NAME)   \
;
#endif

//컴포넌트 생성 함수 선언용 매크로
#define UMREAL_COMPONENT(CLASS_NAME)                                \
extern "C" UMREALSCRIPTS_DECLSPEC Component* NewTest##CLASS_NAME()  \
UMREALSCRIPT_NEWCOMPONENT(CLASS_NAME)

//이 아래는 컴포넌트 생성 함수들을 추가합니다.
//CreateScriptFile() 함수에서 직접 수정하기 때문에 주의해야 합니다.

#include "Scripts\Test\TestComponent.h"
UMREAL_COMPONENT(TestComponent)

#include "Scripts\Graphics\StaticMeshRenderer.h"
UMREAL_COMPONENT(StaticMeshRenderer)
