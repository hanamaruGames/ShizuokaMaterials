#pragma once
#include "Object3D.h"

class SkyDoom : public Object3D
{
public:
	void Draw() override;
	SkyDoom();
	~SkyDoom();
};