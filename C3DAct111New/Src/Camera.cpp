#include "Camera.h"
#include "Player.h"
//#include "Map.h"
#include "Time.h"

namespace {
	// �v���C���[����Ƃ����A���_�ƒ����_�̈ʒu
	const VECTOR3 ThirdEye = VECTOR3(0, 3, -5);
	const VECTOR3 ThirdLook = VECTOR3(0, 2, 0);
	const VECTOR3 FirstEye = VECTOR3(0, 1.5f, 0.5f);
	const VECTOR3 FirstLook = VECTOR3(0, 2, 10);
	const VECTOR3 BirdEye = VECTOR3(0, 15, -5);
	const VECTOR3 BirdLook = VECTOR3(0, 2, 0);

	const float RotationXSpeed = 30.0f; // �P�b�Ԃ̉�]��
	const float RotationYSpeed = 120.0f; // �P�b�Ԃ̉�]��
	const float RotationXMaxLimit = 40.0f; // X��]�̌��E
	const float RotationXMinLimit = -30.0f; // X��]�̌��E
	const float LeapTime = 0.5f; // �J�����̈ړ�����
};


Camera::Camera()
{
	SetDrawOrder(1000); // �J�����̕`�悪�ŗD��
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
		case eThirdPerson: // �O�l�̂���l�̂ɕς���
			mode = eFirstPerson;
			eyeLeap->Start(&eyePosition, eyePosition, FirstEye, LeapTime);
			lookLeap->Start(&lookPosition, lookPosition, FirstLook, LeapTime);
			rotation.x = 0.0f;
			rotation.y = 0.0f;
			break;
		case eFirstPerson: // ��l�̂���Ղɕς���
			mode = eBirdView;
			eyeLeap->Start(&eyePosition, eyePosition, BirdEye, LeapTime);
			lookLeap->Start(&lookPosition, lookPosition, BirdLook, LeapTime);
			break;
		case eBirdView: // ���Ղ��O�l�̂ɕς���
			mode = eThirdPerson;
			eyeLeap->Start(&eyePosition, eyePosition, ThirdEye, LeapTime);
			lookLeap->Start(&lookPosition, lookPosition, ThirdLook, LeapTime);
			break;
		};
	};
	if (mode == eThirdPerson) { // �O�l�̂̎��́A���_�𓮂������Ƃ��ł���
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

	{ // �����_�Ǝ��_�̊Ԃɏ�Q��������΁A��Q���̎�O�Ɏ��_���ړ�������
//		VECTOR3 hit;
//		VECTOR3 norm;
//		CMapProc* pMap = ObjectManager::FindGameObject<CMapProc>();
//		if (pMap->Hitcheck(eye, look, &hit, &norm))   // ��Q���Ƃ̐ڐG�`�F�b�N
//		{
//			VECTOR3 diff = hit - look;
//			float length = diff.Length();
//			float len = length - 0.01f; // ��Q����1cm��O�ɂ���
//			eye = look + diff / length * len;
//		}
	}

	// ���̃N���X�ŎQ�Ƃ��Ă���̂ŁA�Ƃ肠�����c��
	GameDevice()->m_vEyePt = eye;
	GameDevice()->m_vLookatPt = look;
}

void Camera::Draw()
{
	GameDevice()->m_mView = XMMatrixLookAtLH(eye, look, VECTOR3(0,1,0));

}
