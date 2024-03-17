#pragma once
//=============================================================================
//		３Ｄゲームプログラム
//		Ｃ３ＤＡｃｔ１１１                             ver 4.0        2023.3.10
//
//		カメラ制御
//		(視点変更処理・障害物回避処理が入っている)
//
//																N.Hanai
//=============================================================================
#include "Object3D.h"
#include "Leap.h"

/// <summary>
/// カメラを制御するクラス
/// 三人称 → 一人称 → 俯瞰 を切り替えることができる
/// 三人称の場合は、カメラの移動と、障害物の手前に来るようにしている。
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
	EyeMode mode; // 視点モード

	VECTOR3 eyePosition; // 視点の相対位置
	VECTOR3 lookPosition; // 注視点の相対位置
	VECTOR3 rotation; // 三人称のカメラ回転

	Leap<VECTOR3>* eyeLeap; // 視点変更をなめらかにする
	Leap<VECTOR3>* lookLeap; // 注視点変更をなめらかにする

	VECTOR3 eye; // 最終視点
	VECTOR3 look; // 最終注視点
};

