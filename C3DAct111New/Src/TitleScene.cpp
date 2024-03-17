#include "TitleScene.h"
#include "GameMain.h"

TitleScene::TitleScene()
{
}

TitleScene::~TitleScene()
{
}

void TitleScene::Update()
{
	if (GameDevice()->m_pDI->CheckKey(KD_TRG, DIK_P)) {
		SceneManager::ChangeScene("PlayScene");
	}
}

void TitleScene::Draw()
{
	GameDevice()->m_pFont->Draw(400, 15, _T("TITLE SCENE"), 16, RGB(255, 0, 0));

}