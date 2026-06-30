#pragma once
#include<DxLib.h>
#include"../ActorBase.h"
#include"../CharactorBase.h"
#include"../Collider/ColliderCapsule.h"
#include"../../../Utility/AsoUtility.h"
#include <unordered_set>
class EnemyBase;
class ColliderCapsule;
class Camera;
class GameScene;
class Bullet;
class Player : public CharactorBase
{
public:
	struct HitCapsule
	{
		VECTOR start;
		VECTOR end;
		VECTOR pos;
		float radius;
	};
	struct Heal
	{
		int heal;

		VECTOR pos;

		float timer;

		float offsetY;

		float scale;

		int alpha;
	};
	//列挙
	//アニメーションの種類
	enum class ANIM_TYPE
	{
		IDLE,
		WALK,
		RUN,
		DAMAGE,
		JUMP,
		JUMP_END,
		ROLLING,
		ROLLING_INPLACE,
		DRAW_SWORD,
		OFF_SWORD,
		DRAW_GUN,
		OFF_GUN,
		GUN_WALK,
		GUN_IDLE,
		GUN_JUMP,
		GUN_SHOT,
		SWORD_WALK,
		SWORD_IDLE,
		SWORD_SLASH,
		SWORD_ATTACK,
		SWORD_ATTACK2,
		SWORD_JUMP,
		DEATH,
		SWORD_DEATH,
		HEAL,
		POWER_UP,
		ATTACK_SPEL,
		SWORD_CHARGE,
		MAX
	};

	//プレイヤーの状態
	enum class PlayerState
	{
		Idle,
		Move,
		Jump,
		DrawWeapon,
		OffWeapon,
		WeaponIdle,
		WeaponWalk,
		WeaponJump,
		Attack,
		Rolling,
		Death,
		Skill,
		Damage,
		Charge,
		MAX
	};
	//プレイヤーの武器種別
	enum class PLAYER_ATTACK_STATE
	{
		NONE,
		ATTACK_SWORD,
		ATTACK_GUN,
		MAX
	};
	//アニメーションの再生部位
	enum class PLAY_MODE
	{
		FULL_BODY,
		UPPER_BODY,
		LOWER_BODY,
		MAX,
	};

	enum class SKILL_TYPE
	{
		HEAL,       // 回復
		POWER_UP,  // 攻撃力アップ
		ATTAKC_SPEL, // 重攻撃
		MAX
	};
	const float skillCoolTimeMax_[static_cast<int>(SKILL_TYPE::MAX)]
	{
		5.0f,   // HEAL
		15.0f,  // POWER_UP
		60.0f   // ATTACK_SPELL
	};




	const float DAMAGE_REACTION_TIME = 0.4f;
	const float DAMAGE_FLASH_TIME = 0.15f;
	const float DAMAGE_REACTION_BORDER = 30.0f;

	//定数定義
	// プレイヤー
	const VECTOR INITPOS = VGet(-71.566f, -252.452f, -1381.522f);
	const float MOVE_SPEED = 1.2f;
	const float MOUSE_SENSITIVITY = 0.0005f;
	const float JUMP_POW = 0.5f;
	static constexpr int STAMINA_BAR_X = 1000;    // 画面右上のX座標
	static constexpr int STAMINA_BAR_Y = 20;     // 画面上部からのY座標
	static constexpr int STAMINA_BAR_WIDTH = 250; // ゲージの最大幅
	static constexpr int STAMINA_BAR_HEIGHT = 20; // ゲージの高さ
	static constexpr int STAMINA_BAR_THICKNESS = 2; // ゲージの枠の太さ
	static constexpr float OUT_STAGE = -57.0f;
	const float DAMAGE_INTERVAL_MAX = 0.3f;
	const float SKILL_COOL_TIME_MAX = 5.0f;
	const float POWER_UP_TIME = 30.0f;
	const float MAX_CHARGE_TIME = 2.0f;
	const float DEFAULT_CHARGE_POWER = 1.0f;
	
	//UI
	static constexpr float UI_TIME = 120.0f;
	const int SKILL_X = 1700;
	const int SKILL_Y = 120;
	const int SKILL_ICON_SIZE = 100;

	// ジャンプ力
	static constexpr float POW_JUMP_INIT = 2400.0f;
	// 持続ジャンプ力
	static constexpr float POW_JUMP_KEEP = 400.0f;
	// ジャンプの入力受付時間
	static constexpr float TIME_JUMP_INIT = 0.5f;

	//アニメーション

	static constexpr float ANIMATION_SPEED = 20.0f;

	static constexpr float ANIMATION_ATTACK_SPEED = 40.0f;

	static constexpr float JUMP_ANIMATION_SPEED = 60.0f;

	static constexpr float ATTACK_EXTENSION = 90.0f;//攻撃のキャンセル可能時間
	static constexpr float ATTACK_ANIM_TIME = 114.0f;//攻撃のアニメーション時間

	// 回復間隔
	const float MP_RECOVERY_INTERVAL_MAX = 0.2f;

	//コライダー
	// 衝突判定用線分開始
	static constexpr VECTOR COL_LINE_START_LOCAL_POS = { 0.0f, 65.0f, 0.0f };
	// 衝突判定用線分終了
	static constexpr VECTOR COL_LINE_END_LOCAL_POS = { 0.0f, -10.0f, 0.0f };

	// 衝突判定用線分開始(ジャンプ時)
	static constexpr VECTOR COL_LINE_JUMP_START_LOCAL_POS = { 0.0f, 100.0f, 0.0f };
	// 衝突判定用線分終了(ジャンプ時)
	static constexpr VECTOR COL_LINE_JUMP_END_LOCAL_POS = { 0.0f, 20.0f, 0.0f };

	// 衝突判定用カプセル上部球体
	static constexpr VECTOR COL_CAPSULE_TOP_LOCAL_POS = { 0.0f, 90.0f, 0.0f };
	// 衝突判定用カプセル下部球体
	static constexpr VECTOR COL_CAPSULE_DOWN_LOCAL_POS = { 0.0f, 10.0f, 0.0f };

	// 衝突判定用カプセル上部球体(ジャンプ時)
	static constexpr VECTOR COL_CAPSULE_TOP_JUMP_LOCAL_POS =
	{ 0.0f, 140.0f, 0.0f };
	// 衝突判定用カプセル下部球体(ジャンプ時)
	static constexpr VECTOR COL_CAPSULE_DOWN_JUMP_LOCAL_POS =
	{ 0.0f, 60.0f, 0.0f };

	// 衝突判定用カプセル球体半径
	static constexpr float COL_CAPSULE_RADIUS = 15.0f;

	// 色の定義 (DxLibのカラーコード)
	static constexpr unsigned int COLOR_GREEN = 0x00FF00;
	static constexpr unsigned int COLOR_YELLOW = 0x00FFFF00;
	static constexpr unsigned int COLOR_RED = 0xFF0000;
	static constexpr unsigned int COLOR_BLACK = 0x000000;
	static constexpr unsigned int COLOR_BLUE = 0x0000FF;
	static constexpr unsigned int COLOR_WHITE = 0xFFFFFF;

	//武器
	//オフセット座標
	VECTOR DEFAULT_SWORD_BACK_POS =
	{
	  -100.0f,
	  -40.0f,
	  40.0f
	};
	VECTOR DEFAULT_SWORD_HAND_POS =
	{
		30.0f,
		124.0f,
		5.00f
	};
	VECTOR DEFAULT_SWORD_ATTACK_POS =
	{
		110.0f,
		49.0f,
		30.0f
	};
	VECTOR DEFAULT_GUN_BACK_POS =
	{
		-23.0f,
		-16.0f,
		-9.0f
	};
	VECTOR DEFAULT_GUN_HAND_POS =
	{
		5.0f,
		10.0f,
		10.0f
	};
	VECTOR DEFAULT_GUN_ATTACK_POS =
	{
		5.0f,
		10.0f,
		0.0f
	};
	VECTOR DEFAULT_GUN_MOVE_POS =
	{
		7.0f,
		14.0f,
		2.0f
	};
	//オフセットアングル
	VECTOR DEFAULT_SWORD_BACK_ROT =
		VGet(AsoUtility::Deg2RadF(18.425f),
			AsoUtility::Deg2RadF(-127.692f),
			AsoUtility::Deg2RadF(-49.231f));

	VECTOR DEFAULT_SWORD_HAND_ROT =
		VGet(
			AsoUtility::Deg2RadF(7.692),
			AsoUtility::Deg2RadF(95.385),
			AsoUtility::Deg2RadF(-167.692));
	VECTOR DEFAULT_SWORD_ATTACK_ROT =
		VGet(
			AsoUtility::Deg2RadF(5.385),
			AsoUtility::Deg2RadF(162.306),
			AsoUtility::Deg2RadF(123.077))
		;
	VECTOR DEFAULT_GUN_BACK_ROT =
		VGet(
			AsoUtility::Deg2RadF(0.0f),
			AsoUtility::Deg2RadF(-94.961f),
			AsoUtility::Deg2RadF(0.0f))
		;
	VECTOR DEFAULT_GUN_HAND_ROT =
		VGet(
			AsoUtility::Deg2RadF(-8.462f),
			AsoUtility::Deg2RadF(76.923f),
			AsoUtility::Deg2RadF(-95.385f))
		;
	VECTOR DEFAULT_GUN_ATTACK_ROT =
		VGet(
			AsoUtility::Deg2RadF(32.307f),
			AsoUtility::Deg2RadF(-6.329f),
			AsoUtility::Deg2RadF(-67.559f))
		;
	VECTOR DEFAULT_GUN_MOVE_ROT =
		VGet(
			AsoUtility::Deg2RadF(8.461f),
			AsoUtility::Deg2RadF(28.462f),
			AsoUtility::Deg2RadF(-95.385f))
		;
	Player(Camera* camera);
	~Player()override;
	void Load();
	void LoadEnd();
	void Init();
	void Update()override;
	void Draw()override;
	void Release()override;
	void UpdateDebug();

	void OnGetItem();
	bool HasItem() const { return hasItem_; }

	void SetGameScene(GameScene* gameScene);


	//ゲッター
	VECTOR GetPos();
	VECTOR GetRot();
	bool GetClear();
	virtual ACTOR_TYPE GetType() const { return ACTOR_TYPE::PLAYER; }

	//プレイヤーの状態の遷移
	void ChangeState(PlayerState newState);

	//プライヤーの攻撃種類の遷移
	void ChangeAttackState(PLAYER_ATTACK_STATE newState);

	//セッター
	void SetPos(VECTOR pos);

	//void CheckSwordHit();

	const std::vector<HitCapsule>& GetHitCapsules() const;

	void TakeDamage(float damage);

	void ShotBullet();

	bool GetDeath();

	void PlaySlashEffect();
private:
	// リソースロード
	void InitLoad(void) override;

	// 大きさ、回転、座標の初期化
	void InitTransform(void) override;

	//アニメーション初期化
	void InitAnimation(void) override;

	// 初期化後の個別処理
	void InitPost(void) override;

	//衝突判定初期化
	void InitCollider(void) override;

	// 移動処理
	void Move()override;// 入力処理まとめ
	//武器用オフセット管理用関数
	void UpdateWeaponOffset();
	//抜刀処理
	void DrawSword();
	//銃を抜く処理
	void DrawGun();
	//ジャンプ処理
	void ProcessJump();
	//剣攻撃処理
	void ProcessAttack_Sword();
	//銃攻撃処理
	void ProcessAttack_Gun();

	virtual void UpdateProcces(void) override;
	virtual void UpdateProccesPost(void) override;

	//プレイヤーの状態の管理
	void PlayerStatement();

	//プレイヤーの攻撃状態の管理
	void PlayerAttackState();

	//プレイヤーのスキル管理
	void PlayerSkill();
	//スキルの名前取得
	const char* GetSkillName() const;
	//スキル使用
	void UseSkill();

	//プレイヤーの武器のモデル取得
	int GetWeapon();

	void CreateHitCapsule(int startFrame, int endFrame, float radius);

	void UpdateCollision();

	void Death();

	void DrawHPandMP();
	void DrawSkill();
	void HealingPop();
	void PlayHealEffect();
	void PlayPowerUpEffect();
	void PlayAttackSpellEffect();
	void PlayChargeEffect();
	void Healing();
	void PowerUp();
	void AttackSpell();
	void ProcessChargeAttack();
	void ChargeSlash();
	void DrawChargeBox();
	//VECTOR GetModelBoundingBox(int modelHandle);

private:
	
	// 状態管理
	

	PLAYER_ATTACK_STATE attackState_;
	PlayerState playerState_;

	bool isDead_ = false;
	bool isJumping_ = false;
	bool isAttacking_ = false;
	bool isKick_ = false;
	bool rolling_ = false;
	bool isDontMove_ = false;

	bool hasItem_ = false;

	GameScene* gameScene_;
	EnemyBase* enemy;

	std::unordered_set<EnemyBase*> hitEnemies_;

	
	// プレイヤーパラメータ
	

	float maxPlayerHP_ = 500.0f;
	float playerHP_ = maxPlayerHP_;

	float maxPlayerMP_ = 1000.0f;
	float playerMP_ = maxPlayerMP_;

	int playerAttackPower_ = 10;

	float damegeInterval_;

	bool isDamageReaction_ = false;
	float damageFlashTimer_ = 0.0f;
	float accumulatedDamage_ = 0.0f;

	// 移動
	

	VECTOR moveDir_;
	float moveSpeed_;
	float mouseSensitivity_;

	float verticalSpeed_ = 0.0f;
	const float gravity_ = 0.03f;

	
	// ダッシュ・スタミナ
	

	float currentStamina_;
	float maxStamina_;

	float staminaConsumptionRate_;
	float staminaRecoveryRate_;

	float dashSpeedMultiplier_;
	float baseMoveSpeed_;

	bool isDashing_;
	bool wasDashingLastFrame_ = false;

	float staminaCoolDownTimer_ = 0.0f;

	
	// ローリング
	

	float rollingTimer_ = 0.0f;
	float rollingDuration_ = 0.45f;

	VECTOR rollingDir_ = {};
	float rollingSpeed_ = 4.0f;

	
	// 息切れ
	

	bool isBreathless_ = false;
	float breathlessTimer_ = 0.0f;

	float breathlessAlpha_ = 0.0f;
	float breathlessPhase_ = 0.0f;

	
	// アニメーション


	AnimationController* animationController_;

	ANIM_TYPE animState_ = ANIM_TYPE::IDLE;

	float stepBlend_;
	float blendTime_;

	int attachIndex;

	
	// コンボ


	int comboStep_ = 0;
	float comboTimer_ = 0.0f;

	bool canNextCombo_ = false;

	
	// カメラ


	Camera* camera_;

	
	// 武器管理
	

	// モデル
	int swordModelId_;
	int gunRModelId_;
	int gunLModelId_;

	// フレーム
	int handR_;
	int handL_;
	int back_;
	int hips_;

	int rootFrame = 4;
	int tipFrame = 7;

	// 画像
	int weaponImg_;
	int gunHandle_;
	int swordHandle_;

	// 状態
	bool isDraw = false;
	bool drawNow = false;

	bool offWeapon = true;
	bool offNow = false;

	bool attackCollider_ = false;
	bool isDrawNow_ = false;

	
	// 剣


	VECTOR swordPos_;
	MATRIX swordRot_;
	VECTOR swordScale_ = { 1.0f,1.0f,1.0f };

	VECTOR swordOffsetBackPos_;
	VECTOR swordOffsetBackRot_;

	VECTOR swordOffsetHandPos_;
	VECTOR swordOffsetHandRot_;

	VECTOR swordOffsetAttackPos_;
	VECTOR swordOffsetAttackRot_;

	VECTOR swordCollPosStart_;
	VECTOR swordCollPosEnd_;

	Transform swordTransform_;

	
	ColliderCapsule* swordCollider_ = nullptr; 
	ColliderCapsule* GetSwordCollider();

	bool isCharging_ = false;

	float chargeTime_ = 0.0f;

	float chargePower_ = DEFAULT_CHARGE_POWER;

	bool isChargingAttack_ = false;

	
	// 銃
	

	VECTOR gunRPos_;
	MATRIX gunRRot_;
	VECTOR gunRScale_ = { 0.2f,0.2f,0.2f };

	VECTOR gunROffsetBackPos_;
	VECTOR gunROffsetBackRot_;

	VECTOR gunROffsetHandPos_;
	VECTOR gunROffsetHandRot_;

	VECTOR gunROffsetAttackPos_;
	VECTOR gunROffsetAttackRot_;

	VECTOR gunROffsetMovePos_;
	VECTOR gunROffsetMoveRot_;

	VECTOR gunRDrawOffsetPos_;
	VECTOR gunRDrawOffsetRot_;

	VECTOR bulletStart_;
	VECTOR bulletEnd_;

	
	// エフェクト


	int muzzleEffect_ = -1;
	int muzzleFrame_ = 2;
	VECTOR muzzlePos_ = VGet(0.0f, 0.0f, 0.0f);

	int slashEffect_ = -1;
	int slashFrame_ = 7;
	VECTOR slashPos_ = VGet(0.0f, 0.0f, 0.0f);

	int healingHandle_ = -1;
	int healingEffect_ = -1;

	int powerUpEffect_ = -1;

	VECTOR healingEffectPos_ = VGet(0.0f, 0.0f, 0.0f);
	VECTOR powerUpEffectPos_ = VGet(0.0f, 0.0f, 0.0f);

	int attackSpellEffect_ = -1;
	VECTOR attackSpellEffectPos_ = VGet(0.0f, 0.0f, 0.0f);

	
	// スキル
	
	float skillCoolTime_[static_cast<int>(SKILL_TYPE::MAX)];

	SKILL_TYPE currentSkill_ = SKILL_TYPE::HEAL;

	int skillImg_;
	int healHandle_;
	int attackSpellHandle_;
	int powerUpHandle_;

	bool isSkillUsing_ = false;
	bool healApplied_ = false;

	float healUseMP_ = 60.0f;
	float healAmount_ = 150.0f;

	bool isPowerUp_ = false;

	int powerUpUseMp_ = 100.0f;
	int powerUpAmount_ = 2;

	float powerUpTimer_ = 0.0f;

	float attackSpellUseMP_ = 700.0f;

	int skillFrame_ = 0;

	
	// MP関連

	float gunUseMP_ = 20.0f;
	float swordHitRecoveryMP_ = 25.0f;
	float mpRecoveryInterval_ = 0.0f;
	
	// プレイヤー当たり判定


	int headFrame_ = 6;

	int spineFrame_ = 2;
	int spine2Frame_ = 3;

	int rightArmFrame_ = 21;
	int rightForeArmFrame_ = 23;

	int leftArmFrame_ = 9;
	int leftForeArmFrame_ = 11;

	int rightUpLegFrame_ = 37;
	int rightLegFrame_ = 39;

	int leftUpLegFrame_ = 32;
	int leftLegFrame_ = 34;

	VECTOR headPos_ =
		MV1GetFramePosition(modelId_, headFrame_);

	VECTOR spinePos_ =
		MV1GetFramePosition(modelId_, spineFrame_);

	VECTOR rightArmPos_ =
		MV1GetFramePosition(modelId_, rightArmFrame_);

	VECTOR leftArmPos_ =
		MV1GetFramePosition(modelId_, leftArmFrame_);

	VECTOR rightLegPos_ =
		MV1GetFramePosition(modelId_, rightLegFrame_);

	VECTOR leftLegPos_ =
		MV1GetFramePosition(modelId_, leftLegFrame_);

	std::vector<HitCapsule> hitCapsules_;

	
	// UI


	bool announce;

	int scoreFontHandle_ = -1;

	int countDown_;

	float debugCounter_;

	float bulletDebugTimer_ = 0.0f;


	// 回復ポップアップ


	std::vector<Heal> healPos_;

	
	// その他

	int attachFrame;
	float damageReactionTimer_ = 0.0f;
	float distToEnemy_ = 0.0f;
	float attackRange_ = 0.0f;
	bool isWalkingSE_ = false;
	int index = 0;

	bool isDebug_ = false;
	//武器用初期化関数
	void InitWeapon(void);
};

