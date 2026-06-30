#pragma once

#include <vector>

#include "../Scene/SceneBase.h"
#include<Dxlib.h>
class ColliderBase;
class NightDoom;
class Door;
class Item;
class Stage;
class Player;
class EnemyBase;
class Camera;
class ActorBase;
class EnemyManager;
class Bullet;

class GameScene : public SceneBase
{
public:
    struct ScorePopup
    {
        int score;

        VECTOR pos;

        float timer;

        float offsetY;

        float scale;

        int alpha;
    };
    
    //定数定義
    static constexpr int ENEMY_STATCK_TIME = 30;
    // 表示時間
    static constexpr int CONTROLS_SHOW_TIME = 10000;
    const int INFO_X = 40;
    const int INFO_Y = 140;
    GameScene();
    ~GameScene() override;

    void Load(void) override;
    void Init(void) override;
    void Update(void) override;
    void Draw(void) override;

    Camera* GetCamera();

    void Release(void) override;

    Item* GetItem();

    //ゲームクリア判定
    bool CheckGameClear();


    Player* GetPlayer();

    std::vector<EnemyBase*>& GetEnemies();
   
    void SpawnEnemy();

    void CreateScorePopup(
        int score,
        VECTOR pos
    );

    void AddScore(int score);

	void AddBullet(Bullet* bullet);

    void AddKillEnemy();

    int GetKillEnemyCount()const;

    int GetScore();

private:
   

    Stage* stage_ = nullptr;
    Player* player_ = nullptr;
    Camera* camera_ = nullptr;
    Item* item_ = nullptr;
	Door* door_ = nullptr;
    NightDoom* nDoom = nullptr;

    int startTime_;
    int killEnemyCount_ = 0;
    //デバッグ用当たり判定
	bool OkCollision_ = true;

    bool enemyStack_ = false;

    bool clear_;

    //カウントダウン
    int countDown_;

    //すべてのアクター
    std::vector<ActorBase*>allActor_;

    //複数エネミー
	std::vector<EnemyBase*> enemys_;

 
    //敵のスタック回避用関数
    void EnemyCollision(ActorBase* actor);

    //敵のスタック検知用
    void EnemyStack(ActorBase* actor);

    void UpdateDebug(void);


    // 敵のスタック解除関連のフラグとカウンタ
    bool isEnemyStuck_ = false; // 敵が壁にスタックしているかを検知するフラグ
    int stuckTimer_ = 0;        // スタック開始からの時間
    int noCollisionTimer_ = 0;  // 当たり判定無効期間のタイマー
    bool isNoCollisionMode_ = false; // 当たり判定無効モードかどうかのフラグ

    bool isPaused_ = false;        // ゲームポーズ状態 (メニュー表示) フラグ
    int menuSelection_ = 0;        // メニュー選択肢 (0: 続ける, 1: 終了)

    void DrawMenu();               // メニュー画面の描画処理

    //エネミーのランダムスポーン
    std::vector<VECTOR> enemySpawnPoints_;
    VECTOR enemySpawnPos_;

    float enemySpawnTimer_ = 0.0f;      // 現在のタイマー
    float enemySpawnInterval_ = 1.0f;   // 何秒ごとに生成するか

    int maxEnemyNum_ = 30;              // フィールド上の最大数
    int spawnEnemyNum_ = 120;            // 合計で何体生成するか
	int targetKillCount_ = 80;           // クリアに必要な撃破数
    int spawnedEnemyCount_ = 0;         // 今まで生成した数
    int randomIndex_ = -1;
    float gameClearCountDown_ = 3.0f;
	float timeLimit_ = 200.0
        ; // 制限時間（秒）

	float gameOverCountdown_ = 2.5f;

    std::vector<ScorePopup> scorePopups_;
    int scoreFontHandle_ = -1;

    int score_ = 0;

    int drawScore_ = 0;

    bool isShowControls_ = true;

    int controlsImage_ = -1;

    // 表示開始時間
    int controlsStartTime_ = 0;


  
    //弾管理
    std::vector<Bullet*> bullets_;
};
