#pragma once
#include<DxLib.h>
#include"../ActorBase.h"
#include"../CharactorBase.h"
#include"../Collider/ColliderCapsule.h"
#include"../../../Utility/AsoUtility.h"

class ColliderCapsule;
class Player;
class GameScene;
class EnemyBase : public CharactorBase
{
public:
	struct HitCapsule
	{
		VECTOR start;
		VECTOR end;
		VECTOR pos;
		float radius;
	};
	struct DamageText
	{
		int damage;

		VECTOR pos;

		float timer;

		float offsetY;
	};
	enum class ANIM_TYPE
	{
		IDLE,
		WALK,
		ATTACK,
		JUMP,
		DEATH,
		DAMAGE,
		MAX
	};
	enum class EnemyState
	{
		IDLE,
		WANDER,
		CHASE,
		ATTACK,
		DEATH,
		DAMAGE,
		MAX
	};
	//定数定義
	//アニメーションの再生速度
	static constexpr float ANIMATION_SPEED = 30.0f;
	//デフォルト座標
	static constexpr VECTOR DEFAULT_POS = { -36.300f,-66.205f,868.141f };
	// 衝突判定用線分開始
	static constexpr VECTOR COL_LINE_START_LOCAL_POS = { 0.0f, 30.0f, 0.0f };
	// 衝突判定用線分終了
	static constexpr VECTOR COL_LINE_END_LOCAL_POS = { 0.0f, -10.0f, 0.0f };

	// 衝突判定用線分開始(ジャンプ時)
	static constexpr VECTOR COL_LINE_JUMP_START_LOCAL_POS = { 0.0f, 100.0f, 0.0f };
	// 衝突判定用線分終了(ジャンプ時)
	static constexpr VECTOR COL_LINE_JUMP_END_LOCAL_POS = { 0.0f, 20.0f, 0.0f };

	// 衝突判定用カプセル上部球体
	static constexpr VECTOR COL_CAPSULE_TOP_LOCAL_POS = { 0.0f, 25.0f, 0.0f };
	// 衝突判定用カプセル下部球体
	static constexpr VECTOR COL_CAPSULE_DOWN_LOCAL_POS = { 0.0f, 10.0f, 0.0f };

	// 衝突判定用カプセル上部球体(ジャンプ時)
	static constexpr VECTOR COL_CAPSULE_TOP_JUMP_LOCAL_POS =
	{ 0.0f, 140.0f, 0.0f };
	// 衝突判定用カプセル下部球体(ジャンプ時)
	static constexpr VECTOR COL_CAPSULE_DOWN_JUMP_LOCAL_POS =
	{ 0.0f, 60.0f, 0.0f };

	// 衝突判定用カプセル球体半径
	static constexpr float COL_CAPSULE_RADIUS = 45.0f;

	static constexpr float SEARCH_RANGE = 800.0f;

	static constexpr float DEFAULT_MOVE_SPEED = 1.0;

	const float ATTACK_COOL_MAX_ = 2.0f;


	EnemyBase();
	virtual ~EnemyBase();
	void Load();
	void LoadEnd();
	void Init();
    void Update(void) override;
	void Draw(void) override;
	void Release(void) override;

	void SetGameScene(GameScene* gameScene);

	//プレイヤーの状態の遷移
	void ChangeState(EnemyState newState);

	//セッター
	void SetPos(const VECTOR& pos) { transform_.pos = pos; }

	//スポーン関数
	void SetSpawnPos(const VECTOR& spawnPos);

	ColliderCapsule* GetCollider();

	void TakeDamage(int damage);

	void Death();

	bool IsDead() const { return isDead_; }

	int GetScore() const { return score_; }

	int GetKillCount() const { return killCount_; }


protected:
	void InitLoad(void) override ;
	void InitTransform(void) override;
	void InitAnimation(void) override;
	void InitPost(void) override;
	void InitCollider(void) override;
	void UpdateProcces(void) override;
	void UpdateProccesPost(void) override;
	void UpdateIdle();
	void UpdateWander();
	void UpdateChase();
	void UpdateAttack();
	void ColliderUpdate();
	void CreateHitCapsule(int startFrame, int endFrame, float radius);

private:
	GameScene* gameScene_;
	Player* player_;
	ColliderCapsule* colCapsule_ = nullptr;
	ColliderCapsule* colAttack_ = nullptr;
	std::vector<HitCapsule> hitCapsules_;
	std::vector<DamageText> damageTexts_;
    void UpdateDebug(void);
  
	void FollowPlayer(void);

	void SearchPlayer(void);

	void EnemyStatement();

	void PlayDeathEffect();

	int modelId_;
	float dictanceToPlayer_ = 0.0f;
	float searchRange_ = SEARCH_RANGE;
	ANIM_TYPE animType_;
	bool isFoundPlayer_ = false;
	float moveSpeed_ = DEFAULT_MOVE_SPEED;
	EnemyState enemyState_;
	VECTOR playerPos_;
	bool isDamaged_ = false;
	bool isAttacking_ = false;
	bool isCountedKill_ = false;

	int hp_ = 100;
	int scoreValue_ = 300;
	int score_ = 0;
	float damageInterval_ = 0.0f;
	const float DAMAGE_INTERVAL_MAX = 0.3f;
	bool isDead_ = false;
	bool isHitPlayer_ = false;
	bool isCreateScorePopup_ = false;
	bool isInvincible = false;

	// ランダム移動先
	VECTOR wanderTarget_;

	// 次の行動までの時間
	float wanderTimer_ = 0.0f;

	// 移動速度
	float wanderSpeed_ = 1.5f;

	// 行動範囲
	float wanderRange_ = 30.0f;

	float distance_ = 0.0f;
	float attackRange_ = 50.0f;
	float attackInterval_ = 0.0f;

	float attackRadius_ = 20.0f;
	int rightLegFrame_ = 42;
	int rightLegUpFrame_ = 44;
	int effectFrame_ = 0;

	int leftLegFrame_ = 26;
	int leftLegUpFrame_ = 28;

	bool isDebug_ = false;

	float stopDistance_ = 0.0f;
	int damageFontHandle_ = -1;
	int drawScore_ = 0;
	int killCount_ = 0;
	//エフェクト
	int deathEffect_ = -1;
	VECTOR effectOffset_ = { 0.0f, 0.0f, 0.0f };
};

