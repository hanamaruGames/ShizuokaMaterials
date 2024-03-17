//=============================================================================
//		メッシュのアニメーション再生情報構造体
//　                                                  ver 1.0        2023.1.31
//
//
//																FbxMeshAnim.h
//=============================================================================
#pragma once


// アニメーション再生情報構造体
struct ANIMATION_STATUS {
	bool   playAnim;    // 再生するか
	int    animNum;     // 再生中のアニメーション番号
	int    animNumOld;  // 前のフレームのアニメーション番号
	int    endFrame;    // 再生中のアニメーションの最終フレーム
	int    animFrame;   // 現在のフレーム数
	bool   endFlag;     // 最終フレームに達したか
	bool   animLock;    // アニメーションをロックする
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
	// アニメーション番号の設定
	void SetNum(int num)
	{
		if (animLock) return;
		if (animNum != num) endFlag = false;
		animNum = num;
	}
	// アニメーションの最終フレームに達したか
	bool isEnd(int num)
	{
		if (animNum == num && endFlag) return true;
		else  return false;
	}
};

