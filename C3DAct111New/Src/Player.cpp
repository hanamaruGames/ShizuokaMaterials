#include "Player.h"
#include "GameMain.h"
#include "Time.h"
#include "Ground.h"

namespace {
	const float MoveSpeed = 10.0f;
	const float RotationSpeed = 30.0f;
	const float Gravity = 9.8f / 8.0f;
	const float JumpHeight = 2.0f;
	const float CharaRadius = 0.5f;
};

Player::Player()
{
	mesh = new CFbxMesh();
	mesh->Load("Data/Player/PlayerChara.mesh");
	animator = new Animator(this);
	animator->Load(AnimationID::anmIdle, "Data/Player/Idle.anmx");
	animator->Load(AnimationID::anmRun, "Data/Player/Run.anmx");
	animator->Load(AnimationID::anmJumpIn, "Data/Player/JumpUp.anmx", false);
	animator->Load(AnimationID::anmJumpOut, "Data/Player/JumpDown.anmx", false);
	animator->Load(AnimationID::anmDamage, "Data/Player/Damage.anmx", false);
	animator->Load(AnimationID::anmDead, "Data/Player/Dead.anmx", false);
	animator->Load(AnimationID::anmAttack1, "Data/Player/Attack1.anmx", false);
	animator->Load(AnimationID::anmAttack2, "Data/Player/Attack2.anmx", false);
	animator->Load(AnimationID::anmAttack3, "Data/Player/Attack3.anmx", false);
	animator->Play(AnimationID::anmIdle);
	state = State::stNormal;
	jumpVelocity = 0.0f;
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
	MATRIX4X4 oldMatrix = transform.localMatrix;	// 旧ワールドマトリックスを保存   // -- 2024.3.13

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
	Ground* pGround = ObjectManager::FindGameObject<Ground>();
	VECTOR3 hit;
	transform.position += velocity;
	if (pGround->HitCheckSphere(transform.position + VECTOR3(0, 1.1f, 0), CharaRadius, &hit)) {
		transform.position = hit + VECTOR3(0,-1.1f, 0);
	}


	jumpVelocity -= Gravity * Time::DeltaTime();
	transform.position.y += jumpVelocity;
	if (jumpVelocity <= 0.0f) {
		if (pGround->HitCheckLine(transform.position + VECTOR3(0, 1, 0), transform.position + VECTOR3(0, -1, 0), &hit)) {
			transform.position = hit;
			jumping = false;
			jumpVelocity = 0.0f;
			transform.position = hit;
			if (animator->PlayingID()==AnimationID::anmJumpIn)
				animator->Play(AnimationID::anmIdle);
		}
		else {
			jumping = true;
		}
	}
	transform.CalcLocalMatrix();		// ワールドマトリックスを計算  	   // -- 2024.3.13

	viewAnimation();
	animator->Update();

	ImGui::Begin("Player Position");
	ImGui::InputFloat("X", &transform.position.x);
	ImGui::InputFloat("Y", &transform.position.y);
	ImGui::InputFloat("Z", &transform.position.z);
	ImGui::End();
}

void Player::viewAnimation()
{
	CDirectInput* input = GameDevice()->m_pDI;
	if (input->CheckKey(KD_TRG, DIK_1))
		animator->Play(1);
	if (input->CheckKey(KD_TRG, DIK_2))
		animator->Play(2);
	if (input->CheckKey(KD_TRG, DIK_3))
		animator->Play(3);
	if (input->CheckKey(KD_TRG, DIK_4))
		animator->Play(4);
	if (input->CheckKey(KD_TRG, DIK_5))
		animator->Play(5);
	if (input->CheckKey(KD_TRG, DIK_6))
		animator->Play(6);
	if (input->CheckKey(KD_TRG, DIK_7))
		animator->Play(7);
	if (input->CheckKey(KD_TRG, DIK_8))
		animator->Play(8);
	if (input->CheckKey(KD_TRG, DIK_9))
		animator->Play(9);
	if (input->CheckKey(KD_TRG, DIK_0))
		animator->Play(0);
}

// 通常時の更新処理
void Player::updateNormal()
{
	CDirectInput* input = GameDevice()->m_pDI;
	if (input->CheckKey(KD_DAT, DIK_W)) {
		VECTOR3 forward = VECTOR3(0, 0, 1) * XMMatrixRotationY(transform.rotation.y * DegToRad);
		velocity = forward * MoveSpeed * Time::DeltaTime();
		if (animator->PlayingID()==AnimationID::anmIdle || animator->Finished())
			animator->Play(AnimationID::anmRun);
	}else if (input->CheckKey(KD_DAT, DIK_S)) {											 // -- 2024.3.13
		VECTOR3 forward = VECTOR3(0, 0, -1) * XMMatrixRotationY(transform.rotation.y * DegToRad);
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
	if (!jumping) {
		if (input->CheckKey(KD_TRG, DIK_N)) {
			jumping = true;
			jumpVelocity = sqrtf(JumpHeight * Gravity * 2.0f *Time::DeltaTime());
			animator->Play(AnimationID::anmJumpIn);
			velocity.y = jumpVelocity;
		}
		if (input->CheckKey(KD_TRG, DIK_M)) {
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
		if (input->CheckKey(KD_TRG, DIK_M)) {
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
		if (input->CheckKey(KD_TRG, DIK_M)) {
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
	// 再生速度を調整
	if (animator->CurrentFrame() < 40.0f)
		animator->SetPlaySpeed(5.0f);
	else
		animator->SetPlaySpeed(1.2f);
	if (animator->Finished()) {
		state = State::stNormal;
	}
	if (animator->Finished()) {
		state = State::stNormal;
	}
}
