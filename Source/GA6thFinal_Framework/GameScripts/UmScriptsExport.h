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

#include "Scripts\WeaponSystem/WeaponTable/WeaponTableComponent.h"
UMREAL_COMPONENT(WeaponTableComponent)

#include "Scripts\Test/DevTools/FPSCounter.h"
UMREAL_COMPONENT(FPSCounter)

#include "Scripts\Particle/ParticleComponent.h"
UMREAL_COMPONENT(ParticleComponent)

#include "Scripts/UI/UIRoot/UIRoot.h"
UMREAL_COMPONENT(UIRoot)

#include "Scripts/UI/Elements/Dummy/DummyElement.h"
UMREAL_COMPONENT(DummyElement)

#include "Scripts/UI/Elements/Image/ImageElement.h"
UMREAL_COMPONENT(ImageElement)

#include "Scripts/UI/Elements/Text/TextElement.h"
UMREAL_COMPONENT(TextElement)

#include "Scripts/UI/Wrappers/Dummy/DummyWrapper.h"
UMREAL_COMPONENT(DummyWrapper)

#include "Scripts/UI/Wrappers/Ratio/RatioWrapper.h"
UMREAL_COMPONENT(RatioWrapper)

#include "Scripts/UI/Wrappers/Scrolling/ScrollingWrapper.h"
UMREAL_COMPONENT(ScrollingWrapper)

#include "Scripts/UI/Panels/Grid/GridPanel.h"
UMREAL_COMPONENT(GridPanel)
UMREAL_COMPONENT(GridPanelSlot)

#include "Scripts/UI/Panels/Overlay/OverlayPanel.h"
UMREAL_COMPONENT(OverlayPanel)

#include "Scripts/UI/Panels/Horizontal/HorizontalPanel.h"
UMREAL_COMPONENT(HorizontalPanel)
UMREAL_COMPONENT(HorizontalPanelSlot)

#include "Scripts/UI/Panels/Description/DescriptionPanel.h"
UMREAL_COMPONENT(DescriptionPanel)

#include "Scripts/UI/Views/TurnQueue/TurnQueueView.h"
UMREAL_COMPONENT(TurnQueueView)

#include "Scripts/UI/Views/Weapon/WeaponView.h"
UMREAL_COMPONENT(WeaponView)

#include "Scripts/UI/Views/MonsterHp/MonsterHpView.h"
UMREAL_COMPONENT(MonsterHpView)

#include "Scripts/UI/Views/RevelationsView/RevelationsView.h"
UMREAL_COMPONENT(RevelationsView)

#include "Scripts\WeaponSystem/WeaponSystem.h"
UMREAL_COMPONENT(WeaponSystem)

#include "Scripts\RevelationSystem/RevelationSystem.h"
UMREAL_COMPONENT(RevelationSystem)

#include "Scripts/Token/TokenSystem.h"
UMREAL_COMPONENT(TokenSystem)

#include "Scripts/Test/TurnAction/TestActionComponent.h"
UMREAL_COMPONENT(TestActionComponent)

//#include "Scripts/Test/Model/ModelTest.h"
//UMREAL_COMPONENT(ModelTest)
//UMREAL_COMPONENT(ViewTest)

#include "Scripts/Audio/AudioComponent.h"
UMREAL_COMPONENT(AudioComponent)

#include "Scripts/Audio/Table/AudioTableComponent.h"
UMREAL_COMPONENT(AudioTableComponent)

#include "Scripts/Animation/AnimationComponent.h"
UMREAL_COMPONENT(AnimationComponent)

#include "Scripts/Test/DevTools/LogOutput.h"
UMREAL_COMPONENT(LogOutput)

#include "Scripts/EnemyAction/System/EnemyActionSystem.h"
UMREAL_COMPONENT(EnemyActionSystem)

#include "Scripts\Audio/BGMManager.h"
UMREAL_COMPONENT(BGMManager)

#include "Scripts\ItemDropSystem/UI/ItemDropUIRootManager.h"
UMREAL_COMPONENT(ItemDropUIRootManager)

#include "Scripts\ItemDropSystem/UI/ArtifactUIManager.h"
UMREAL_COMPONENT(ArtifactUIManager)