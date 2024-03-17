#pragma once
#include "Object3D.h"

class SkyDoom : public Object3D
{
public:
	SkyDoom();
	~SkyDoom();
	void Draw() override;
};