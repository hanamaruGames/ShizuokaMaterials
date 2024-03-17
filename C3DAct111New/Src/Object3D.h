#pragma once
#include "GameObject.h"

const float Deg2Rad = XM_PI / 180.0f;
const float Rad2Deg = 180.0f / XM_PI;

class TransformData {
public:
	VECTOR3 position;
	VECTOR3 rotation;
	VECTOR3 scale;
	MATRIX4X4 localMatrix;
	TransformData() {
		position = VECTOR3(0, 0, 0);
		rotation = VECTOR3(0, 0, 0);
		scale = VECTOR3(1, 1, 1);
	};
	void CalcLocalMatrix();
};

class Object3D : public GameObject
{
public:
	Object3D() : mesh(nullptr) {}
	virtual ~Object3D() { SAFE_DELETE(mesh); }
	virtual void Update() override {}
	virtual void Draw() override;
	const TransformData& Transform() { return transform; }
	CFbxMesh* Mesh() { return mesh; }
protected:
	TransformData transform;
	CFbxMesh* mesh;
};