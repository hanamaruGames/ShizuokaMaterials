#include "Object3D.h"

void TransformData::CalcLocalMatrix()
{
	localMatrix = XMMatrixScalingFromVector(scale);
	localMatrix = localMatrix * XMMatrixRotationRollPitchYawFromVector(rotation*Deg2Rad);
	localMatrix = localMatrix * XMMatrixTranslationFromVector(position);
}

void Object3D::Draw()
{
	transform.CalcLocalMatrix();
	mesh->Render(transform.localMatrix);
}
