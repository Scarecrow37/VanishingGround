#pragma once

/// <summary>
/// <para> 모듈들의 초기화 및 자원 정리를 자동화 하기 위한 인터페이스입니다.                  </para>
/// <para> 상속 받은뒤, 클라이언트의 Application Drived 클래스의 생성자에서 등록해주면 됩니다.</para>
/// <para> 각 모듈들이 등록된 순서대로 초기화 함수들이 호출됩니다. 해제 함수는 추가 순서의 역순으로 호출합니다.</para>
/// <para> 초기화 순서는 모든 모듈의 PreInit -> ModuleInitialize 순입니다. </para>
/// <para> 해제 순서는 모든 모듈의 PerUnInitialize -> ModuleUnInitialize  순입니다.  </para>
/// </summary>
struct IAppModule
{
    IAppModule() = default;
    virtual ~IAppModule() = default;

    virtual void PreInitialize() = 0;
    virtual void ModuleInitialize() = 0;

    virtual void PreUnInitialize() = 0;
    virtual void ModuleUnInitialize() = 0;
};