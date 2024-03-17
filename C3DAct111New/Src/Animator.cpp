#include "Animator.h"
#include <fstream>

Animator::Animator(Object3D* obj)
{
	object = obj;
	playingID = -1;
	playSpeed = 1.0f;
	frame = 0.0f;
	finished = true;
}

Animator::~Animator()
{
	animationList.clear();
}

void Animator::Update()
{
	if (object==nullptr || object->Mesh()==nullptr)
		return;
	auto itr = animationList.find(playingID);
	if (itr == animationList.end())
		return;

	finished = false;
	frame += playSpeed;
	const AnimationData info = itr->second;
	if (frame >= info.endFrame) {
		if (info.loop) {
			frame -= info.endFrame + info.startFrame;
		}
		else {
			frame = info.endFrame;
			finished = true;
		}
	}
	AnimationData& anim = animationList[playingID];
	object->Mesh()->SetAnimation(anim.loadedID, frame);
}

void Animator::Load(int id, std::string filename, bool loop)
{
	if (object == nullptr || object->Mesh() == nullptr)
		return;
	CFbxMesh* pMesh = object->Mesh();
	AnimationData info;
	pMesh->LoadAnimation(filename.c_str());
	int n = pMesh->m_allAnimationCount-1;
	info.loadedID = n;
	info.startFrame = pMesh->m_Animation[n].startFrame;
	info.endFrame = pMesh->m_Animation[n].endFrame;
	info.loop = loop;
	animationList[id] = info;
}

void Animator::Play(int id, bool force)
{
	if (object == nullptr || object->Mesh() == nullptr)
		return;

	auto itr = animationList.find(id);
	if (itr == animationList.end())
		return;

	AnimationData info = itr->second;
	if (playingID == id && !Finished()) {
		if (force) {
			frame = info.startFrame;
		}
	}
	else {
		playingID = id;
		frame = info.startFrame;
	}
}

void Animator::SetPlaySpeed(float speed)
{
	playSpeed = speed;
}

int Animator::PlayingID()
{
	return playingID;
}

float Animator::CurrentFrame()
{
	return frame;
}

bool Animator::Finished()
{
	AnimationData info = animationList[playingID];
	return !info.loop&& frame >= info.endFrame;
}
