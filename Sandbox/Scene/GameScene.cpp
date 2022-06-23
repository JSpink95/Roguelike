#include "Scene\GameScene.h"

#include "Scene\SplashScreenScene.h"
#include "Scene\GameHubScene.h"

void PopulateSceneManager(SceneManager::Ref sceneManager, AppWindow::Ref window)
{
    sceneManager->PushPrebuiltScene(SplashScreenScene::Create(window, sceneManager));
    sceneManager->PushPrebuiltScene(GameHubScene::Create(window, sceneManager));
}