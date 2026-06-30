#pragma once
#include<DxLib.h>
#include"../CharactorBase.h"

class GameClearScene;
class GameScene;
class Player;
class Item;
class Enemy : public CharactorBase
{
public:
	//列挙
	enum class ANIM_TYPE
	{
		IDLE,
		RUN,
		WALK,
		DEATH,
		ATTACK,
		MAX
	};



	//定数定義
	//デフォルト座標
	static constexpr VECTOR DEFAULT_POS = { -38.0f,-66.2f,-508.504f };
	//デフォルトスケール
	static constexpr VECTOR DEFAULT_SCALE = { 0.025f,0.025f,0.025f };

	//エネミーのステータス
	static constexpr float DEFAULT_MOVE_SPEED = 0.25f;
	static constexpr float SEARCH_RANGE = 30.0f;

	//当たり判定用カプセルの始点
	static constexpr VECTOR CAPSULE_START_POS = { 0.0f,3.0f,0.0f };
	//当たり判定用カプセルの終点
	static constexpr VECTOR CAPSULE_END_POS = { 0.0f,0.5f,0.0f };
	//当たり判定用カプセルの半径
	static constexpr float CAPSULE_RADIUS = 0.5f;

	//ゲームオーバー判定距離
	static constexpr float GAMEOVER_DISTANCE = 2.0f;

	//スロースピード
	static constexpr float SLOW_SPEED = 0.2f;

	//音の判定用
	static constexpr float DISTANCE_RANGE = 10.0f;
	static constexpr float VOLUME_LOW_RIMIT = 0.5f;
	static constexpr float DEFAULT_VOLUME = 3.0f;

	//UI用
	static constexpr int UI_POS_X = 800;
	static constexpr int UI_POS_Y = 50;

	//アニメーションスピード
	static constexpr float ANIMATION_SPEED = 25.0f;

	Enemy();
	~Enemy() override;
	void Init();
	void Load();
	void LoadEnd();
	void Update() override;
	void Draw() override;
	void Release() override;

	VECTOR GetPos();

	bool GetGameOver();
	virtual ACTOR_TYPE GetType() const { return ACTOR_TYPE::ENEMY; }

	void SetGameScene(GameScene* scene) { gameScene_ = scene; }

	void SetGameClearScene(GameClearScene* scene) { clearScene_ = scene; }

	//セッター
	void SetPos(VECTOR pos);

private:
	// リソースロード
	void InitLoad(void) override;

	// 大きさ、回転、座標の初期化
	void InitTransform(void) override;

	//アニメーション初期化
	void InitAnimation(void) override;

	// 初期化後の個別処理
	void InitPost(void) override;


	//プレイヤーを追従する
	void FollowPlayer();

	void SlowFollowPlayer();

	//Playerをみつける
	void Search();




private:
	int modelId_;
	bool gameOver_ = false;
	VECTOR rot_;
	VECTOR scale_;
	bool activeFlag_;

	bool isFoundPlayer_ = true; 

	float moveSpeed_ = DEFAULT_MOVE_SPEED;    
	float searchRange_ = SEARCH_RANGE; 
	float dictanceToPlayer_ = 0.0f;
	float dictanceSE = 0.0f;
	float volume_ = DEFAULT_VOLUME;

	//アニメーション制御
	AnimationController* animationController_;
	//ゲームシーン
	GameScene* gameScene_;
	//クリアシーン
	GameClearScene* clearScene_;

	bool isEnemySoundPlayed_; // SRC::ENEMY の再生開始フラグ
	bool isBGMSoundPlayed_;   // SRC::BGM の再生開始フラグ

};
