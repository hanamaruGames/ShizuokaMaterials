#pragma once
#include "Object3D.h"
#include <vector>

class MeshCollider
{
public:
	struct CollInfo {
		float length; // 始点または中心点から、当たった点までの距離
		VECTOR3 hitPosition; // 当たった位置
		VECTOR3 normal; // 法線
		VECTOR3 triangle[3]; // 頂点
	};

	MeshCollider();
	MeshCollider(Object3D* object);

	/// <summary>
	/// FbxMeshデータから、判定データを作成する
	/// </summary>
	/// <param name="mesh">メッシュデータ</param>
	void MakeFromMesh(CFbxMesh* mesh);

	/// <summary>
	/// meshファイルから、判定データを作成する
	/// 当たり判定用に軽量化モデルを使う場合はこちらを使ってください
	/// </summary>
	/// <param name="fileName">モデルデータのパス</param>
	void MakeFromFile(std::string fileName);

	/// <summary>
	/// 線分との当たり判定を行う
	/// 始点から一番近いポリゴンの当たり判定情報を返します
	/// ポリゴンの表面のみ判定し、裏面は判定しません
	/// </summary>
	/// <param name="trans">このコライダーの位置情報</param>
	/// <param name="from">線分の始点</param>
	/// <param name="to">線分の終点</param>
	/// <param name="info">当たったポリゴンの情報を入れる場所</param>
	/// <returns>当たっていればtrue</returns>
	bool CheckCollisionLine(const MATRIX4X4& trans, const VECTOR3& from, const VECTOR3& to, CollInfo* hitOut = nullptr);

	/// <summary>
	/// 球体との当たり判定を行う
	/// ポリゴンの表面のみ判定し、裏面は判定しません
	/// 
	/// 球の中心から一番近いポリゴンの当たり判定情報を返します
	/// infoがnullptrの場合は、当たり情報を返さず、当たったかのみ返します
	/// 
	/// 複数のポリゴンが当たる可能性があるので、CheckCollisionSphereList関数を使用することを推奨します
	/// </summary>
	/// <param name="trans">このコライダーの位置情報</param>
	/// <param name="center">球の中心座標</param>
	/// <param name="radius">球の半径</param>
	/// <param name="info">当たったポリゴンの情報を入れる場所</param>
	/// <returns>当たっていればtrue</returns>
	bool CheckCollisionSphere(const MATRIX4X4& trans, const VECTOR3& center, float radius, CollInfo* hitOut = nullptr);

	/// <summary>
	/// 球体との当たり判定を行う
	/// ポリゴンの表面のみ判定し、裏面は判定しません
	/// 
	/// 当たっているポリゴンすべての情報を返します
	/// 当たっているものがなければ、returnのsizeが0になります
	/// </summary>
	/// <param name="trans">このコライダーの位置情報</param>
	/// <param name="center">球の中心座標</param>
	/// <param name="radius">球の半径</param>
	/// <returns>当たったポリゴン全ての位置情報</returns>
	std::list<MeshCollider::CollInfo> CheckCollisionSphereList(const MATRIX4X4& trans, const VECTOR3& center, float radius);

	/// <summary>
	/// カプセルとの当たり判定を行う
	/// ポリゴンの表面のみ判定し、裏面は判定しません
	/// 
	/// カプセルのp1の点から一番近いポリゴンの当たり判定情報を返します
	/// infoがnullptrの場合は、当たり情報を返さず、当たったかのみ返します
	/// 
	/// 複数のポリゴンが当たる可能性があるので、CheckCollisionCapsuleList関数を使用することを推奨します
	/// </summary>
	/// <param name="trans">このコライダーの位置情報</param>
	/// <param name="p1">カプセルの点１</param>
	/// <param name="p2">カプセルの点２</param>
	/// <param name="radius">球の半径</param>
	/// <param name="info">当たったポリゴンの情報を入れる場所</param>
	/// <returns>当たっていればtrue</returns>
	//bool CheckCollisionCapsule(const MATRIX4X4& trans, const VECTOR3& p1, const VECTOR3& p2, float radius, CollInfo* hitOut = nullptr);

	/// <summary>
	/// カプセルとの当たり判定を行う
	/// 当たっているポリゴンすべての情報を返します
	/// 
	/// ポリゴンの表面のみ判定し、裏面は判定しません
	/// </summary>
	/// <param name="trans">このコライダーの位置情報</param>
	/// <param name="p1">カプセルの点１</param>
	/// <param name="p2">カプセルの点２</param>
	/// <param name="radius">球の半径</param>
	/// <returns>当たったポリゴン全ての位置情報</returns>
	//std::list<MeshCollider::CollInfo> CheckCollisionCapsuleList(const MATRIX4X4& trans, const VECTOR3& p1, const VECTOR3& p2, float radius);
private:
	struct BoundingBox {
		VECTOR3 min;
		VECTOR3 max;
	};
	struct BoundingBall {
		VECTOR3 center;
		float radius;
	};
	struct PolygonInfo {
		int indices[3]; // 頂点番号
		int mesh;
		VECTOR3 normal; // 面の法線
	};
	std::vector<PolygonInfo> polygons;
	std::vector<VECTOR3> vertices;
	BoundingBox bBox;
	BoundingBall bBall;

	bool checkPolygonToLine(const PolygonInfo& info, const VECTOR3& from, const VECTOR3& to, CollInfo* hit = nullptr);
	bool checkPolygonToSphere(const PolygonInfo& info, const VECTOR3& center, float radius, CollInfo* hit = nullptr);
};