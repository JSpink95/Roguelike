#pragma once
#include <Core.h>
#include <Core\Window.h>
#include <Game\SceneManager.h>

enum class GameScene : u8
{
    SplashScreen,
    // ... menus etc
    GameHub,
    Max
};

void PopulateSceneManager(SceneManager::Ref sceneManager, AppWindow::Ref graphics);
