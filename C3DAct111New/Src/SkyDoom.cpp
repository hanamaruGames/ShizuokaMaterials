#include "SkyDoom.h"

SkyDoom::SkyDoom()
{
	mesh = new CFbxMesh();
	mesh->Load("Data/Map/sky2.mesh");
	transform.scale = VECTOR3(2, 2, 2);
}
void SkyDoom::Draw()
{
	VECTOR3 lightBack = GameDevice()->m_vLightDir;
	GameDevice()->m_vLightDir = VECTOR3(0, 0, 0);
	Object3D::Draw();
	GameDevice()->m_vLightDir = lightBack;
}

SkyDoom::~SkyDoom()
{
	SAFE_DELETE(mesh);
}
