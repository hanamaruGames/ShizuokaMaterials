#include "Camera.h"
#include "Player.h"
//#include "Map.h"
#include "Time.h"

namespace {
	// プレイヤーを基準とした、視点と注視点の位置
	const VECTOR3 ThirdEye = VECTOR3(0, 3, -5);
	const VECTOR3 ThirdLook = VECTOR3(0, 2, 0);
	const VECTOR3 FirstEye = VECTOR3(0, 1.5f, 0.5f);
	const VECTOR3 FirstLook = VECTOR3(0, 2, 10);
	const VECTOR3 BirdEye = VECTOR3(0, 15, -5);
	const VECTOR3 BirdLook = VECTOR3(0, 2, 0);

	const float RotationXSpeed = 30.0f; // １秒間の回転量
	const float RotationYSpeed = 120.0f; // １秒間の回転量
	const float RotationXMaxLimit = 40.0f; // X回転の限界
	const float RotationXMinLimit = -30.0f; // X回転の限界
	const float LeapTime = 0.5f; // カメラの移動時間
};


Camera::Camera()
{
	SetDrawOrder(1000); // カメラの描画が最優先
	eyeLeap = new Leap<VECTOR3>();
	lookLeap = new Leap<VECTOR3>();
	eyePosition = ThirdEye;
	lookPosition = ThirdLook;
	mode = eThirdPerson;
	look = ThirdLook;
	eye = ThirdEye;
}

Camera::~Camera()
{
	SAFE_DELETE(eyeLeap);
	SAFE_DELETE(lookLeap);
}

void Camera::Update()
{
	CDirectInput* input = GameDevice()->m_pDI;
	if (input->CheckKey(KD_TRG, DIK_Q)) {
		switch (mode) {
		case eThirdPerson: // 三人称を一人称に変える
			mode = eFirstPerson;
			eyeLeap->Start(&eyePosition, eyePosition, FirstEye, LeapTime);
			lookLeap->Start(&lookPosition, lookPosition, FirstLook, LeapTime);
			rotation.x = 0.0f;
			rotation.y = 0.0f;
			break;
		case eFirstPerson: // 一人称を俯瞰に変える
			mode = eBirdView;
			eyeLeap->Start(&eyePosition, eyePosition, BirdEye, LeapTime);
			lookLeap->Start(&lookPosition, lookPosition, BirdLook, LeapTime);
			break;
		case eBirdView: // 俯瞰を三人称に変える
			mode = eThirdPerson;
			eyeLeap->Start(&eyePosition, eyePosition, ThirdEye, LeapTime);
			lookLeap->Start(&lookPosition, lookPosition, ThirdLook, LeapTime);
			break;
		};
	};
	if (mode == eThirdPerson) { // 三人称の時は、視点を動かすことができる
		if (input->CheckKey(KD_DAT, DIK_RIGHT)) {
			rotation.y += RotationYSpeed * Time::DeltaTime();
		}
		if (input->CheckKey(KD_DAT, DIK_LEFT)) {
			rotation.y -= RotationYSpeed * Time::DeltaTime();
		}
		if (input->CheckMouse(KD_DAT, DIM_UP)) {
			rotation.x += RotationXSpeed * Time::DeltaTime();
			rotation.x = min(rotation.x, RotationXMaxLimit);
		}
		if (input->CheckMouse(KD_DAT, DIM_DOWN)) {
			rotation.x -= RotationXSpeed * Time::DeltaTime();
			rotation.x = max(rotation.x, RotationXMinLimit);
		}
	}

	eyeLeap->Update();
	lookLeap->Update();

	Player* pPlayer = ObjectManager::FindGameObject<Player>();
	if (pPlayer != nullptr) {
		MATRIX4X4 m = XMMatrixRotationY(pPlayer->Transform().rotation.y * Deg2Rad);
		VECTOR3 pPosition = pPlayer->Transform().position;
		if (mode != eFirstPerson) {
			pPosition.y = 0;
		}
		look = pPosition + lookPosition * m * XMMatrixRotationY(rotation.y * Deg2Rad);
		eye = pPosition + eyePosition * XMMatrixRotationX(rotation.x * Deg2Rad) * m * XMMatrixRotationY(rotation.y * Deg2Rad);
	}

	{ // 注視点と視点の間に障害物があれば、障害物の手前に視点を移動させる
//		VECTOR3 hit;
//		VECTOR3 norm;
//		CMapProc* pMap = ObjectManager::FindGameObject<CMapProc>();
//		if (pMap->Hitcheck(eye, look, &hit, &norm))   // 障害物との接触チェック
//		{
//			VECTOR3 diff = hit - look;
//			float length = diff.Length();
//			float len = length - 0.01f; // 障害物の1cm手前にする
//			eye = look + diff / length * len;
//		}
	}

	// 他のクラスで参照しているので、とりあえず残す
	GameDevice()->m_vEyePt = eye;
	GameDevice()->m_vLookatPt = look;
}

void Camera::Draw()
{
	GameDevice()->m_mView = XMMatrixLookAtLH(eye, look, VECTOR3(0,1,0));

}
