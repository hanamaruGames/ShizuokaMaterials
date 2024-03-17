#include "sceneManager.h"
#include "sceneBase.h"
#include "SceneFactory.h"
#include "Time.h"
#include <Windows.h>

namespace {
	std::string currentName; // ���݂̃V�[���̖���
	std::string nextName;    // ���̃V�[���̖���
	SceneBase* currentScene; // ���̃V�[���̃C���X�^���X��ێ�
	SceneFactory* factory;   // �V�[���؂�ւ��p��Factory�̃|�C���^�[
};

void SceneManager::Start()
{
	Time::Init();

	nextName = "";
	currentName = "";

	factory = new SceneFactory();
	// �ŏ��ɓ����V�[�����ASceneFactory�ɍ���Ă��炤
	currentScene = factory->CreateFirst();
}

void SceneManager::Update()
{
	if (nextName != currentName) { // �V�[���؂�ւ��̎w�肪�������̂�
		if (currentScene != nullptr) { // ���܂ł̃V�[�������
			delete currentScene;
			currentScene = nullptr;
		}
		currentName = nextName;
		currentScene = factory->Create(nextName); // ���̃V�[�����쐬
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
