#include "Ground.h"

Ground::Ground()
{
	mesh = new CFbxMesh();
	mesh->Load("Data/Map/map50Field1.mesh");

	meshCol = new MeshCollider();
	meshCol->MakeFromMesh(mesh);
}

Ground::~Ground()
{
	SAFE_DELETE(meshCol);
	SAFE_DELETE(mesh);
}

bool Ground::HitCheckSphere(const VECTOR3& center, float radius, VECTOR3* hit)
{
	std::list<MeshCollider::CollInfo> meshes = meshCol->CheckCollisionSphereList(XMMatrixIdentity(), center, radius);
	if (meshes.size() == 0)
		return false;
	if (hit != nullptr) { // 座標が必要なのであれば
		VECTOR3 pushVec = VECTOR3(0, 0, 0); // 最終的に押すベクトル
		for (const MeshCollider::CollInfo& m : meshes) { // 当たっているすべてのポリゴンで
			float pushLength = radius - Dot(center-m.hitPosition, m.normal);
			//			float pushLength = radius - m.length; MeshColliderのEDGE_HITが0ならば、これでもOK
			VECTOR3 push = m.normal * pushLength; // このポリゴンで押し戻すべきベクトル
			// 今のpushVecと合成する
			float dot = Dot(push, XMVector3Normalize(pushVec));
			dot -= pushVec.Length(); // 今のpushVecに含まれる成分は消す
			if (dot < 0)
				dot = 0;
			pushVec += push - pushVec * dot; // 残った分を加算
		}
		*hit = center + pushVec;
	}
	return true;
}

bool Ground::HitCheckLine(const VECTOR3& from, const VECTOR3& to, VECTOR3* hit)
{
	MeshCollider::CollInfo coll;
	bool ret = meshCol->CheckCollisionLine(XMMatrixIdentity(), from, to, &coll);
	if (ret) {
		if (hit != nullptr) {
			*hit = coll.hitPosition;
		}
		return true;
	}
	return false;
}
