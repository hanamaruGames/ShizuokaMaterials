#pragma once
#include "Object3D.h"
#include <map>
#include "FbxMeshAnim.h"

class Animator {
public:
	Animator(Object3D* obj);
	~Animator();
	void Update();
	void Load(int id, std::string filename, bool loop = true);
	void Play(int id, bool force = false);
	void SetPlaySpeed(float speed = 1.0f);
	int PlayingID();
	float CurrentFrame();
	bool Finished();
private:
	struct AnimationFrame {
		int framePoseNum;
		MATRIX4X4 framePose;

	};
	struct AnimationData {
		//ANIMATION構造体の中
		int startFrame;
		int endFrame;
		int rootBoneMesh;
		int rootBone;
		// BONE構造体の中
		MATRIX4X4 bindPose;
		int framePoseNum;
		std::vector<MATRIX4X4> framePose;
		// その他情報
		enum RootAnimType {
			eRootAnimNone = 0,
			eRootAnimXZ,
			eRootAnim,
		};
		RootAnimType type;
		int meshNum;
		bool loop;
		int boneNum;
	};
	Object3D* object;
	int playingID;
	float playSpeed;
	float frame;
	bool finished;
	std::map<int, AnimationData> animationList;
};