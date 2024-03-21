#include "MeshCollider.h"

MeshCollider::MeshCollider()
{
}

MeshCollider::MeshCollider(Object3D* object)
{
}

void MeshCollider::MakeFromMesh(CFbxMesh* mesh)
{
    // CFbxMeshからデータを取得する
    // 複数メッシュに別れているが、まとめて１つにする
    int vTop = 0;
    for (int m = 0; m < mesh->m_dwMeshNum; m++) {
        CFbxMeshArray* arr = &mesh->m_pMeshArray[m];
        int vNum = arr->m_dwVerticesNum;
        vertices.reserve(vertices.size() + vNum);
        for (int v = 0; v < vNum; v++) {
            if (mesh->m_nMeshType == 1)
                vertices.push_back(arr->m_vStaticVerticesNormal[v].Pos);
            else
                vertices.push_back(arr->m_vSkinVerticesNormal[v].Pos);
        }
        int iNum = arr->m_dwIndicesNum / 3;
        polygons.reserve(polygons.size() + iNum);
        for (int i = 0; i < iNum; i++) {
            PolygonInfo inf;
            inf.indices[0] = arr->m_nIndices[i * 3 + 0] + vTop;
            inf.indices[1] = arr->m_nIndices[i * 3 + 1] + vTop;
            inf.indices[2] = arr->m_nIndices[i * 3 + 2] + vTop;
            VECTOR3 v0 = vertices[inf.indices[0]];
            VECTOR3 v1 = vertices[inf.indices[1]];
            VECTOR3 v2 = vertices[inf.indices[2]];
            inf.normal = XMVector3Normalize(XMVector3Cross(v2 - v0, v1 - v0)); // meshデータは、反時計回りが表なので
            inf.mesh = m;
            polygons.push_back(inf);
        }
        vTop += vNum;
    }
    bBox.min = vertices[0];
    bBox.max = vertices[0];
    bBall.center = vertices[0];
    for (int v = 1; v < vertices.size(); v++) {
        if (bBox.min.x > vertices[v].x) bBox.min.x = vertices[v].x;
        if (bBox.max.x < vertices[v].x) bBox.max.x = vertices[v].x;
        if (bBox.min.y > vertices[v].y) bBox.min.y = vertices[v].y;
        if (bBox.max.y < vertices[v].y) bBox.max.y = vertices[v].y;
        if (bBox.min.z > vertices[v].z) bBox.min.z = vertices[v].z;
        if (bBox.max.z < vertices[v].z) bBox.max.z = vertices[v].z;
        bBall.center += vertices[v];
    }
    bBall.center /= vertices.size(); // ここが中心になる
    bBall.radius = 0.0f; // まずは距離の二乗の最大値を求める
    for (int v = 1; v < vertices.size(); v++) {
        float lenSq = (vertices[v] - bBall.center).LengthSquare();
        if (bBall.radius < lenSq)
            bBall.radius = lenSq;
    }
    bBall.radius = sqrtf(bBall.radius);
}

void MeshCollider::MakeFromFile(std::string fileName)
{
    CFbxMesh* mesh = new CFbxMesh();
    if (mesh->Load(fileName.c_str())) {
        MakeFromMesh(mesh);
    }
    SAFE_DELETE(mesh);
    return;
}

bool MeshCollider::CheckCollisionLine(const MATRIX4X4& trans, const VECTOR3& from, const VECTOR3& to, MeshCollider::CollInfo* hitOut)
{
    MATRIX4X4 invTrans = XMMatrixInverse(nullptr, trans);
    VECTOR3 invFrom = from * invTrans;
    VECTOR3 invTo = to * invTrans;

    // バウンディングボールで判定
    VECTOR3 center = (invTo - invFrom) / 2.0f;
    float radius = (invTo - invFrom).Length() / 2.0f;
    float radiusSq = (radius + bBall.radius) * (radius + bBall.radius);
    if ((center - bBall.center).LengthSquare() > radiusSq) {
        return false;
    }
    // バウンディングボックスで判定
    if (bBox.min.x > from.x && bBox.min.x > to.x) return false;
    if (bBox.max.x < from.x && bBox.max.x < to.x) return false;
    if (bBox.min.y > from.y && bBox.min.y > to.y) return false;
    if (bBox.max.y < from.y && bBox.max.y < to.y) return false;
    if (bBox.min.z > from.z && bBox.min.z > to.z) return false;
    if (bBox.max.z < from.z && bBox.max.z < to.z) return false;

    float maxLength = (to - from).Length();
    float minLength = maxLength;
    CollInfo minColl;
    for (const PolygonInfo& pol : polygons) {
        CollInfo coll;
        if (checkPolygonToLine(pol, invFrom, invTo, &coll)) {
            if (minLength > coll.length) {
                minColl = coll;
                minLength = coll.length;
            }
        }
    }
    if (minLength < maxLength) {
        if (hitOut != nullptr) {
            hitOut->length = minColl.length;
            hitOut->hitPosition = minColl.hitPosition;
            hitOut->triangle[0] = minColl.triangle[0] * trans;
            hitOut->triangle[1] = minColl.triangle[1] * trans;
            hitOut->triangle[2] = minColl.triangle[2] * trans;
            VECTOR4 n = VECTOR4(minColl.normal);
            n.w = 0.0f;
            hitOut->normal = XMVector4Transform(n, trans);
        }
        return true;
    }
    return false;
}

bool MeshCollider::CheckCollisionSphere(const MATRIX4X4& trans, const VECTOR3& center, float radius, CollInfo* hitOut)
{
    MATRIX4X4 invTrans = XMMatrixInverse(nullptr, trans);
    VECTOR3 invCenter = center * invTrans;

    // バウンディングボール
    if ((invCenter - bBall.center).LengthSquare() > (bBall.radius + radius) * (bBall.radius + radius)) return false;

    // バウンディングボックス
    if (bBox.min.x > center.x+radius) return false;
    if (bBox.max.x < center.x-radius) return false;
    if (bBox.min.y > center.y+radius) return false;
    if (bBox.max.y < center.y-radius) return false;
    if (bBox.min.z > center.z+radius) return false;
    if (bBox.max.z < center.z-radius) return false;

    // 球の中心から、平面に下した垂線を求める
    float minLength = radius;
    CollInfo minColl;
    bool found = false;
    for (const PolygonInfo& pol : polygons) {
        CollInfo coll;
        if (checkPolygonToSphere(pol, invCenter, radius, &coll)) {
            if (coll.length < minLength) {
                minColl = coll;
                found = true;
            }
        }
    }
    if (!found) return false;

    if (hitOut != nullptr) {
        hitOut->hitPosition = minColl.hitPosition;
        hitOut->triangle[0] = minColl.triangle[0] * trans;
        hitOut->triangle[1] = minColl.triangle[1] * trans;
        hitOut->triangle[2] = minColl.triangle[2] * trans;
        VECTOR4 n = VECTOR4(minColl.normal);
        n.w = 0.0f;
        hitOut->normal = XMVector4Transform(n, trans);
    }
    return true;
}

std::list<MeshCollider::CollInfo> MeshCollider::CheckCollisionSphereList(const MATRIX4X4& trans, const VECTOR3& center, float radius)
{
    MATRIX4X4 invTrans = XMMatrixInverse(nullptr, trans);
    VECTOR3 invCenter = center * invTrans;
    std::list<CollInfo> meshes;

    // バウンディングボール
    if ((invCenter - bBall.center).LengthSquare() > (bBall.radius + radius) * (bBall.radius + radius)) return meshes;

    // バウンディングボックス
    if (bBox.min.x > center.x + radius) return meshes;
    if (bBox.max.x < center.x - radius) return meshes;
    if (bBox.min.y > center.y + radius) return meshes;
    if (bBox.max.y < center.y - radius) return meshes;
    if (bBox.min.z > center.z + radius) return meshes;
    if (bBox.max.z < center.z - radius) return meshes;

    // 球の中心から、平面に下した垂線を求める
    for (const PolygonInfo& pol : polygons) {
        CollInfo coll;
        if (checkPolygonToSphere(pol, invCenter, radius, &coll)) {
            meshes.push_back(coll);
        }
    }
    return meshes;
}

//bool MeshCollider::CheckCollisionCapsule(const MATRIX4X4& trans, const VECTOR3& p1, const VECTOR3& p2, float radius, MeshCollider::CollInfo* info)
//{
//    return false;
//}

//std::list<MeshCollider::CollInfo> MeshCollider::CheckCollisionCapsuleList(const MATRIX4X4& trans, const VECTOR3& p1, const VECTOR3& p2, float radius)
//{
//    return std::list<MeshCollider::CollInfo>();
//}

bool MeshCollider::checkPolygonToLine(const PolygonInfo& info, const VECTOR3& from, const VECTOR3& to, CollInfo* hit)
{
    // 法線が0だと、線か点
    if (info.normal.LengthSquare() == 0.0f)
        return false;
    // ポリゴンを含む面との交差があるかを求める
    VECTOR3 v0 = vertices[info.indices[0]];
    VECTOR3 v1 = vertices[info.indices[1]];
    VECTOR3 v2 = vertices[info.indices[2]];
    float nFrom = Dot(from - v0, info.normal);
    float nTo = Dot(to - v0, info.normal);
    if (nFrom * nTo > 0.0f) // ベクトルが同じ向きなので交差していない
        return false;
    nFrom = fabsf(nFrom);
    nTo = fabsf(nTo);
    // 交点の座標を求める
    VECTOR3 pos = from + (to - from) * nFrom / (nFrom + nTo);
    // 三角形の中かを調べる（外積ベクトルが法線と同じ向き）
    float n = Dot(XMVector3Cross(pos - v0, v1 - v0), info.normal);
    if (n < 0.0f)
        return false;
    n = Dot(XMVector3Cross(pos - v1, v2 - v1), info.normal);
    if (n < 0.0f)
        return false;
    n = Dot(XMVector3Cross(pos - v2, v0 - v2), info.normal);
    if (n < 0.0f)
        return false;
    hit->length = (pos - from).Length();
    hit->hitPosition = pos;
    hit->normal = info.normal;
    hit->triangle[0] = v0;
    hit->triangle[1] = v1;
    hit->triangle[2] = v2;
    return true;
}

bool MeshCollider::checkPolygonToSphere(const PolygonInfo& info, const VECTOR3& center, float radius, CollInfo* hit)
{
#define EDGE_HIT 1 // ポリゴンの外側の球が当たる場所を計算する場合

    // 法線が0だと、線か点
    if (info.normal.LengthSquare() == 0.0f)
        return false;
    // 中心からポリゴンを含む平面への垂線の長さを求める
    const VECTOR3& v0 = vertices[info.indices[0]];
    const VECTOR3& v1 = vertices[info.indices[1]];
    const VECTOR3& v2 = vertices[info.indices[2]];
    float len = Dot(center - v0, info.normal);
    len = fabsf(len);
    if (len > radius) return false; // 垂線の長さが半径より大きい

    VECTOR3 pos = center - info.normal * len; // 垂線との交点（ここが交点）
    if (Dot(center - pos, info.normal) < 0) // 法線が後ろなら当たらない
        return false;

    // 交点がポリゴンの内側か調べる（外積の向きが法線と同じになるはず）
    float n = Dot(XMVector3Cross(pos - v0, v1 - v0), info.normal);
    bool adjusted = false;

    if (n < 0.0f) { // 辺の上に交点があると、外積が0なので、nも0になる
#if EDGE_HIT>0
        VECTOR3 vNorm = XMVector3Normalize(v1 - v0);
        float dist = Dot(pos - v0, vNorm);
        if (fabsf(dist) > radius)
            return false;
        VECTOR3 newPos = vNorm * dist + v0;
        pos = newPos;
        adjusted = true;
#else
        return false;
#endif
    }
    n = Dot(XMVector3Cross(pos - v1, v2 - v1), info.normal);
    if (n < 0.0f) {
#if EDGE_HIT>0
        VECTOR3 vNorm = XMVector3Normalize(v2 - v1);
        float dist = Dot(pos - v1, vNorm);
        if (fabsf(dist) > radius)
            return false;
        VECTOR3 newPos = vNorm * dist + v1;
        pos = newPos;
        adjusted = true;
#else
        return false;
#endif
    }
    n = Dot(XMVector3Cross(pos - v2, v0 - v2), info.normal);
    if (n < 0.0f) {
#if EDGE_HIT>0
        VECTOR3 vNorm = XMVector3Normalize(v0 - v2);
        float dist = Dot(pos - v2, vNorm);
        if (fabsf(dist) > radius)
            return false;
        VECTOR3 newPos = vNorm * dist + v2;
        pos = newPos;
        adjusted = true;
#else
        return false;
#endif
    }
#if EDGE_HIT>0
    if (adjusted) {
        len = (center - pos).Length();
        if (len > radius)
            return false;
    }
#endif
    hit->hitPosition = pos;
    hit->normal = info.normal;
    hit->triangle[0] = v0;
    hit->triangle[1] = v1;
    hit->triangle[2] = v2;
    hit->length = len;
    return true;
}
