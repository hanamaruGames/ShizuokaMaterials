#pragma once
#include "Object3D.h"
#include "Animator.h"

class Player : public Object3D
{
public:
	Player();
	~Player();
	void Update() override;
//	void Draw() override;
private:
	Animator* animator;
	enum AnimationID {
		anmIdle = 0,
		anmRun,
		anmJumpIn,
		anmJumpOut,
		anmDamage,
		anmDead,
		anmAttack1,
		anmAttack2,
		anmAttack3,
	};

	VECTOR3 velocity; // XZ���ʂ̈ړ���
	bool jumping;
	float jumpVelocity;

	// �v���C���[�̏�ԑJ��
	enum State {
		stNormal = 0,
		stDamage,
		stDead,
		stAttack1,
		stAttack2,
		stAttack3,
	};
	State state;
	void updateNormal();
	void updateDamage();
	void updateDead();
	void updateAttack1();
	void updateAttack2();
	void updateAttack3();

//	CFbxMesh* mesh2; // ����̓����蔻��ʒu��\�����邽��
};