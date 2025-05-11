#pragma once
#include "UmFramework.h"

class GameApplication : public Application
{
public:
    GameApplication();
    virtual ~GameApplication() override;

protected:
    /*모듈 초기화 끝난 후 호출되는 함수*/
    virtual void OnStartupComplete();
    /*모듈 초기화 해제 끝난 후 호출되는 함수*/
    virtual void OnShutdownComplete();

private:
    void BuildRootDock();
    void BuildSceneDock();
    void BuildAssetDock();

    EditorModule*     _editorModule = nullptr;
    EditorDockWindow* _rootDock     = nullptr;
    EditorDockWindow* _sceneDock    = nullptr;
    EditorDockWindow* _assetDock    = nullptr;
};