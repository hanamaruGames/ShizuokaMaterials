//=============================================================================
//		���b�V���̃A�j���[�V�����Đ����\����
//�@                                                  ver 1.0        2023.1.31
//
//
//																FbxMeshAnim.h
//=============================================================================
#pragma once


// �A�j���[�V�����Đ����\����
struct ANIMATION_STATUS {
	bool   playAnim;    // �Đ����邩
	int    animNum;     // �Đ����̃A�j���[�V�����ԍ�
	int    animNumOld;  // �O�̃t���[���̃A�j���[�V�����ԍ�
	int    endFrame;    // �Đ����̃A�j���[�V�����̍ŏI�t���[��
	int    animFrame;   // ���݂̃t���[����
	bool   endFlag;     // �ŏI�t���[���ɒB������
	bool   animLock;    // �A�j���[�V���������b�N����
	ANIMATION_STATUS()
	{
		playAnim = false;
		animNum = 0;
		animNumOld = 0;
		endFrame = 0;
		animFrame = 0;
		endFlag = false;
		animLock = false;
	}
	// �A�j���[�V�����ԍ��̐ݒ�
	void SetNum(int num)
	{
		if (animLock) return;
		if (animNum != num) endFlag = false;
		animNum = num;
	}
	// �A�j���[�V�����̍ŏI�t���[���ɒB������
	bool isEnd(int num)
	{
		if (animNum == num && endFlag) return true;
		else  return false;
	}
};

