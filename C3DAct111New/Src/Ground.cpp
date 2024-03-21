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
	if (hit != nullptr) { // ���W���K�v�Ȃ̂ł����
		VECTOR3 pushVec = VECTOR3(0, 0, 0); // �ŏI�I�ɉ����x�N�g��
		for (const MeshCollider::CollInfo& m : meshes) { // �������Ă��邷�ׂẴ|���S����
			float pushLength = radius - Dot(center-m.hitPosition, m.normal);
			//			float pushLength = radius - m.length; MeshCollider��EDGE_HIT��0�Ȃ�΁A����ł�OK
			VECTOR3 push = m.normal * pushLength; // ���̃|���S���ŉ����߂��ׂ��x�N�g��
			// ����pushVec�ƍ�������
			float dot = Dot(push, XMVector3Normalize(pushVec));
			dot -= pushVec.Length(); // ����pushVec�Ɋ܂܂�鐬���͏���
			if (dot < 0)
				dot = 0;
			pushVec += push - pushVec * dot; // �c�����������Z
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
