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

#include "Scripts\Test\Component\TestComponent.h"
UMREAL_COMPONENT(TestComponent)

#include "Scripts/Mesh/MeshComponent.h"

#include "Scripts/Mesh/StaticMeshRenderer.h"
UMREAL_COMPONENT(StaticMeshRenderer)

#include "Scripts/Mesh/SkeletalMeshRenderer.h"
UMREAL_COMPONENT(SkeletalMeshRenderer)

//#include "Scripts\Graphics/SkeletalMeshRenderer.h"
//UMREAL_COMPONENT(SkeletalMeshRenderer)

#include "Scripts/GameCore/FSM/FiniteStateMachine.h"
UMREAL_COMPONENT(FiniteStateMachine)

#include "Scripts/Light/Base/LightComponent.h"

#include "Scripts/Light/DirectionalLight.h"
UMREAL_COMPONENT(DirectionalLight)

#include "Scripts/Light/PointLight.h"
UMREAL_COMPONENT(PointLight)

#include "Scripts/Light/SpotLight.h"
UMREAL_COMPONENT(SpotLight)

#include "Scripts\TurnSystem/TurnMode/TurnMode.h"
UMREAL_COMPONENT(TurnMode)

#include "Scripts/TurnSystem/TurnActor/TurnActor.h"

#include "Scripts/TurnSystem/TurnActor/Character/CharacterBase.h"

#include "Scripts/TurnSystem/TurnActor/Character/Player/Player.h"
UMREAL_COMPONENT(Player)

#include "Scripts/TurnSystem/TurnActor/Character/Enemy/Enemy.h"
UMREAL_COMPONENT(Enemy)

#include "Scripts\Camera/CameraComponent.h"
UMREAL_COMPONENT(CameraComponent)

#include "Scripts/Stats/TurnActorStatsComponent.h"
#include "Scripts\Stats/Player/PlayerStatsComponent.h"
UMREAL_COMPONENT(PlayerStatsComponent)

#include "Scripts\Stats/Enemy/EnemyStatsComponent.h"
UMREAL_COMPONENT(EnemyStatsComponent)

#include "Scripts\Test/Input/InputTestComponent.h"
UMREAL_COMPONENT(InputTestComponent)

#include "Scripts\Timeline/AnimationEventListener.h"
UMREAL_COMPONENT(AnimationEventListener)

#include "Scripts\Stats/WeaponTable/WeaponTableComponent.h"
UMREAL_COMPONENT(WeaponTableComponent)

#include "Scripts\Test/DevTools/FPSCounter.h"
UMREAL_COMPONENT(FPSCounter)

#include "Scripts\Test/UI/TestUI.h"
UMREAL_COMPONENT(TestUI)

#include "Scripts/UI/UIRoot/UIRoot.h"
UMREAL_COMPONENT(UIRoot)

#include "Scripts/UI/Elements/Image/ImageElement.h"
UMREAL_COMPONENT(ImageElement)

#include "Scripts/UI/Elements/Text/TextElement.h"
UMREAL_COMPONENT(TextElement)