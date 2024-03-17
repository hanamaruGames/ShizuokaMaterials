#include "PlayScene.h"
#include "Player.h"
#include "Camera.h"
#include "Ground.h"
#include "SkyDoom.h"
#include "ResourceLoader.h"

PlayScene::PlayScene()
{
	Instantiate<Player>();
	Instantiate<Camera>();
	Instantiate<Ground>();
	Instantiate<SkyDoom>();
}

PlayScene::~PlayScene()
{
}

void PlayScene::Update()
{
	if (GameDevice()->m_pDI->CheckKey(KD_TRG, DIK_T)) {
		SceneManager::ChangeScene("TitleScene");
		ResourceLoader::Reset();
	}
}

void PlayScene::Draw()
{
	GameDevice()->m_pFont->Draw(400, 15, _T("PLAY SCENE"), 16, RGB(255, 0, 0));
}