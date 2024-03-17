#include "Ground.h"

Ground::Ground()
{
	mesh = new CFbxMesh();
	mesh->Load("Data/Map/map50Field1.mesh");
}

Ground::~Ground()
{
	SAFE_DELETE(mesh);
}
