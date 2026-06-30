#pragma once
#include <vector>
class EnemyBase;
class Player;
class EnemyManager
{
public:
	// 敵の出現間隔
	static constexpr int SPAWN_INTERVAL = 60;
	// コンストラクタ
	EnemyManager(Player* player);
	// デストラクタ
	~EnemyManager(void);
	void Init(void);
	void Update(void);
	void Draw(void);
	void Release(void);
private:
	// エネミー
	std::vector<EnemyBase*> enemys_;
	// プレイヤーのポインタ
	Player* player_;
	// 出現間隔
	int cntSpawn_;
};