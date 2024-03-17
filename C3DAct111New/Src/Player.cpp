#include "Player.h"
#include "GameMain.h"
#include "Time.h"
namespace {
	const float MoveSpeed = 10.0f;
	const float RotationSpeed = 30.0f;
	const float Gravity = 9.8f / 8.0f;
	const float JumpHeight = 2.0f;
};

Player::Player()
{
	mesh = new CFbxMesh();
	mesh->Load("Data/Player/PlayerChara.mesh");
	animator = new Animator(this);
	animator->Load(AnimationID::anmIdle, "Data/Player/Idle.anmx");
	animator->Load(AnimationID::anmJumpIn, "Data/Player/JumpUp.anmx", false);
	animator->Load(AnimationID::anmJumpOut, "Data/Player/JumpDown.anmx", false);
	animator->Load(AnimationID::anmDamage, "Data/Player/Damage.anmx", false);
	animator->Load(AnimationID::anmDead, "Data/Player/Dead.anmx", false);
	animator->Load(AnimationID::anmAttack1, "Data/Player/Attack1.anmx", false);
	animator->Load(AnimationID::anmAttack2, "Data/Player/Attack2.anmx", false);
	animator->Load(AnimationID::anmAttack3, "Data/Player/Attack3.anmx", false);
	animator->Load(AnimationID::anmRun, "Data/Player/Run.anmx");
	animator->Play(AnimationID::anmIdle);
	state = State::stNormal;
	jumpVelocity = 0.0f;

//	mesh2 = new CFbxMesh();
//	mesh2->Load("Data/Player/Laser2.mesh");
}

Player::~Player()
{
	SAFE_DELETE(animator);
	SAFE_DELETE(mesh);
}

void Player::Update()
{
	CDirectInput* input = GameDevice()->m_pDI;
	velocity = VECTOR3(0, 0, 0);
	switch (state) {
	case State::stNormal:
		updateNormal();
		break;
	case State::stAttack1:
		updateAttack1();
		break;
	case State::stAttack2:
		updateAttack2();
		break;
	case State::stAttack3:
		updateAttack3();
		break;
	}
	transform.position += velocity;

	animator->Update();
}

//void Player::Draw()
//{
//	static const VECTOR3 offset = VECTOR3(-33, 79, 0);

//	Object3D::Draw();
//	MATRIX4X4 bone = mesh->GetFrameMatrices(34);
//	bone = XMMatrixRotationRollPitchYawFromVector(offset*DegToRad)* bone* transform.localMatrix;
//	mesh2->Render(bone);
//}

// 通常時の更新処理
void Player::updateNormal()
{
	CDirectInput* input = GameDevice()->m_pDI;
	if (input->CheckKey(KD_DAT, DIK_W)) {
		VECTOR3 forward = VECTOR3(0, 0, 1) * XMMatrixRotationY(transform.rotation.y * DegToRad);
		velocity = forward * MoveSpeed * Time::DeltaTime();
		if (animator->PlayingID()==AnimationID::anmIdle || animator->Finished())
			animator->Play(AnimationID::anmRun);
	}
	else {
		if (animator->PlayingID() == AnimationID::anmRun || animator->Finished())
			animator->Play(AnimationID::anmIdle);
	}
	if (input->CheckKey(KD_DAT, DIK_D)) {
		transform.rotation.y += RotationSpeed * Time::DeltaTime();
	}
	if (input->CheckKey(KD_DAT, DIK_A)) {
		transform.rotation.y -= RotationSpeed * Time::DeltaTime();
	}
	if (jumping) {
		jumpVelocity -= Gravity * Time::DeltaTime();
		if (transform.position.y + jumpVelocity <= 0.0f) {
			jumping = false;
			jumpVelocity = 0.0f;
			transform.position.y = 0.0f;
			animator->Play(AnimationID::anmIdle);
		}
		velocity.y = jumpVelocity;
	}
	else {
		if (input->CheckKey(KD_DAT, DIK_N)) {
			jumping = true;
			jumpVelocity = sqrtf(JumpHeight * Gravity * 2.0f *Time::DeltaTime());
			animator->Play(AnimationID::anmJumpIn);
			velocity.y = jumpVelocity;
		}
		if (input->CheckKey(KD_DAT, DIK_M)) {
			animator->Play(AnimationID::anmAttack1);
			state = State::stAttack1;
		}
	}
}

void Player::updateDamage()
{
}

void Player::updateDead()
{
}

// 攻撃中の更新処理
void Player::updateAttack1()
{
	// 再生速度を調整
	if (animator->CurrentFrame() < 50.0f)
		animator->SetPlaySpeed(5.0f);
	else
		animator->SetPlaySpeed(1.2f);

	// モーションキャンセル
	if (animator->CurrentFrame() > 90.0f) {
		CDirectInput* input = GameDevice()->m_pDI;
		if (input->CheckKey(KD_DAT, DIK_M)) {
			animator->Play(AnimationID::anmAttack2);
			state = State::stAttack2;
		}
	}
	if (animator->Finished()) {
		state = State::stNormal;
	}
}

void Player::updateAttack2()
{
	// 再生速度を調整
	if (animator->CurrentFrame() < 40.0f)
		animator->SetPlaySpeed(5.0f);
	else
		animator->SetPlaySpeed(1.2f);

	// モーションキャンセル
	if (animator->CurrentFrame() > 90.0f) {
		CDirectInput* input = GameDevice()->m_pDI;
		if (input->CheckKey(KD_DAT, DIK_M)) {
			animator->Play(AnimationID::anmAttack3);
			state = State::stAttack3;
		}
	}
	if (animator->Finished()) {
		state = State::stNormal;
	}
}

void Player::updateAttack3()
{
	if (animator->Finished()) {
		state = State::stNormal;
	}
}
