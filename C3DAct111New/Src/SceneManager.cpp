#include "sceneManager.h"
#include "sceneBase.h"
#include "SceneFactory.h"
#include "Time.h"
#include <Windows.h>

namespace {
	std::string currentName; // 現在のシーンの名称
	std::string nextName;    // 次のシーンの名称
	SceneBase* currentScene; // 今のシーンのインスタンスを保持
	SceneFactory* factory;   // シーン切り替え用のFactoryのポインター
};

void SceneManager::Start()
{
	Time::Init();

	nextName = "";
	currentName = "";

	factory = new SceneFactory();
	// 最初に動くシーンを、SceneFactoryに作ってもらう
	currentScene = factory->CreateFirst();
}

void SceneManager::Update()
{
	if (nextName != currentName) { // シーン切り替えの指定があったので
		if (currentScene != nullptr) { // 今までのシーンを解放
			delete currentScene;
			currentScene = nullptr;
		}
		currentName = nextName;
		currentScene = factory->Create(nextName); // 次のシーンを作成
	}
	if (currentScene != nullptr)
		currentScene->Update();
}

void SceneManager::Draw()
{
	Time::Refresh();

	if (currentScene != nullptr)
		currentScene->Draw();
}

void SceneManager::Release()
{
	if (currentScene != nullptr) {
		delete currentScene;
		currentScene = nullptr;
	}
	delete factory;
}

SceneBase* SceneManager::CurrentScene()
{
	return currentScene;
}

void SceneManager::SetCurrentScene(SceneBase* scene)
{
	currentScene = scene;
}

void SceneManager::ChangeScene(const std::string& sceneName)
{
	nextName = sceneName;
}

void SceneManager::Exit()
{
	PostQuitMessage(0);
}
