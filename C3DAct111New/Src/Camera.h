#pragma once
//=============================================================================
//		�R�c�Q�[���v���O����
//		�b�R�c�`�����P�P�P                             ver 4.0        2023.3.10
//
//		�J��������
//		(���_�ύX�����E��Q����������������Ă���)
//
//																N.Hanai
//=============================================================================
#include "Object3D.h"
#include "Leap.h"

/// <summary>
/// �J�����𐧌䂷��N���X
/// �O�l�� �� ��l�� �� ���� ��؂�ւ��邱�Ƃ��ł���
/// �O�l�̂̏ꍇ�́A�J�����̈ړ��ƁA��Q���̎�O�ɗ���悤�ɂ��Ă���B
/// </summary>
class Camera : public Object3D
{
protected:
public:
	Camera();
	~Camera();
	void Update() override;
	void Draw() override;
private:
	enum EyeMode {
		eThirdPerson,
		eFirstPerson,
		eBirdView,
	};
	EyeMode mode; // ���_���[�h

	VECTOR3 eyePosition; // ���_�̑��Έʒu
	VECTOR3 lookPosition; // �����_�̑��Έʒu
	VECTOR3 rotation; // �O�l�̂̃J������]

	Leap<VECTOR3>* eyeLeap; // ���_�ύX���Ȃ߂炩�ɂ���
	Leap<VECTOR3>* lookLeap; // �����_�ύX���Ȃ߂炩�ɂ���

	VECTOR3 eye; // �ŏI���_
	VECTOR3 look; // �ŏI�����_
};

