#pragma once
#include"../Actor/ActorBase.h"

class Player;
class Stage;
class ResourceManager;
class SceneManager;
class AnimationController;
class CharactorBase :public ActorBase
{
public:
	// ステージ関係の定義
#define STAGECOLLOBJ_MAXNUM			256		// 追加のコリジョンオブジェクトの最大数

	// 衝突判定種別
	enum class COLLIDER_TYPE
	{
		LINE,
		CAPSULE,
		MAX,
	};

	//コンストラクタ
	CharactorBase(void);
	//デストラクタ
	virtual ~CharactorBase(void)override;


	//更新
	virtual void Update(void)override;
	// 描画
	virtual void Draw(void) override;
	//解放
	virtual void Release(void)override;


protected:

	// 最大落下速度
	static constexpr float MAX_FALL_SPEED = -15.0f;

	// 衝突時の押し戻し試行回数
	static constexpr int CNT_TRY_COLLISION = 20;
	// 衝突時の押し戻し量
	static constexpr float COLLISION_BACK_DIS = 1.0f;
	static constexpr float STEP_HEIGHT = 7.0f;


	// 丸影画像
	int imgShadow_;
	// リソースロード
	virtual void InitLoad(void) override;
	//更新系
	virtual void UpdateProcces(void) = 0;
	virtual void UpdateProccesPost(void) = 0;




	//移動方向に応じた遅延回転
	void DelayRotate(void);
	AnimationController* animCon_;
	SceneManager* sceMng_;
	VECTOR movePow_;

	VECTOR moveDir_;
	// 移動前の座標
	VECTOR prevPos_;
	// ジャンプ量
	VECTOR jumpPow_;
	// ジャンプ判定
	bool isJump_;
	//ステップジャンプ
	float stepJump_;
	//重力計算
	void CalcGravityPow(void);

	bool isGrounded_ = false;

	// 衝突判定
	virtual void CollisionReserve(void) {}
	void Collision(void);
	void CollisionGravity(void);
	void CollisionCapsule(void);
	//階段の衝突チェック
	void CheckStepUp(void);
	// 丸影描画
	void DrawShadow(void);
};

