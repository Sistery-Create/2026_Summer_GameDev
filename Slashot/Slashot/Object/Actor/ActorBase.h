#pragma once
#include <map>
#include<vector>
#include <string>
#include <DxLib.h>
#include"../Transform.h"

class ColliderBase;
class AnimationController;
class SceneManager;

class ActorBase
{
public:
	enum class ACTOR_TYPE
	{
		PLAYER,
		ENEMY,
		MAX,
	};

	ActorBase(void);			// コンストラクタ
	virtual ~ActorBase(void);	// デストラクタ

	void Init(void);			// 初期化
	void Load(void);			// 読み込み
	void LoadEnd(void);			// 読み込み後
	virtual void Update(void);	// 更新
	virtual void Draw(void);	// 描画
	virtual void Release(void);	// 解放

	// 座標を取得
	const VECTOR& GetPos(void) const { return transform_.pos; }
	const VECTOR& GetRot(void) const { return angle_; }

	virtual ACTOR_TYPE GetType() const { return ACTOR_TYPE::MAX; }

	// カプセルの当たり判定を取得
	const VECTOR& GetStartCapsulePos(void) const { return startCapsulePos_; }
	const VECTOR& GetEndCapsulePos(void) const { return endCapsulePos_; }
	const float& GetCapsuleRadius(void) const { return capsuleRadius_; }

	const bool& GetisCollision(void) const { return isCollision_; }

	// ステージと衝突
	void CollisionStage(const VECTOR& pos);

	void SetGrounded(bool grounded) { isGrounded_ = grounded; }
	bool IsGrounded() const { return isGrounded_; }

	// 大きさ、回転、座標等の取得
	const Transform& GetTransform(void) const;

	// 自身の衝突情報取得
	const std::map<int, ColliderBase*>& GetOwnColliders(void) const
	{
		return ownColliders_;
	}
	// 特定の自身の衝突情報取得
	const ColliderBase* GetOwnCollider(int key) const;

	// 衝突対象となるコライダを登録
	void AddHitCollider(const ColliderBase* hitCollider);

	// 衝突対象となるコライダをクリア
	void ClearHitCollider(void);

protected:

	// アニメーション制御
	AnimationController* animationController_;

	//シングルトン参照
	SceneManager& scnMng_;

	// モデル制御の基本情報
	Transform transform_;

	// 自身の衝突情報
	std::map<int, ColliderBase*> ownColliders_;


	// 衝突相手の情報
	std::vector<const ColliderBase*> hitColliders_;


	// アニメーション種別
	int animType_;

	// モデル情報
	int modelId_;
	VECTOR pos_;
	VECTOR angle_;
	VECTOR localAngle_;
	VECTOR scale_;
	VECTOR drawPos_;

	// カプセルの当たり判定座標
	VECTOR startCapsulePos_;
	VECTOR endCapsulePos_;
	float capsuleRadius_;

	// 当たり判定を取るか
	bool isCollision_;

	// 地面に接地しているか
	bool isGrounded_ = false;

	// パッド入力値の補間
	VECTOR preInputDir_;

	// 移動方向
	VECTOR moveDir_;

	// ジャンプ力
	float jumpPow_;

	// リソースロード
	virtual void InitLoad(void) = 0;

	// 大きさ、回転、座標の初期化
	virtual void InitTransform(void) = 0;

	// アニメーションの初期化
	virtual void InitAnimation(void) = 0;

	// 初期化後の個別処理
	virtual void InitPost(void) = 0;

	// 衝突判定初期化
	virtual void InitCollider(void) = 0;

	// 移動処理
	virtual void Move(void);

	// 遅延回転処理
	void DelayRotate(void);

};
