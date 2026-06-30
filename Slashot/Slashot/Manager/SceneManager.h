#pragma once
#include <chrono>
class Player;
class SceneBase;
class Fader;
class Camera;
class MiniCamera;

class SceneManager
{

public:

	// 背景色
	static constexpr int BACKGROUND_COLOR_R = 0;
	static constexpr int BACKGROUND_COLOR_G = 139;
	static constexpr int BACKGROUND_COLOR_B = 139;


	// シーン管理用
	enum class SCENE_ID
	{
		NONE,
		TITLE,
		LOAD,
		GAME,
		SCARE,
		CLEAR,
		OVER,
		FINAL,
		CAUTION
	};
	enum class GAME_OVER_TYPE
	{
		DEATH,
		MISSION_FAIL
	};
	// インスタンスの生成
	static void CreateInstance(void);

	// インスタンスの取得
	static SceneManager& GetInstance(void);

	// 初期化
	void Init(void);

	// 3Dの初期化
	void Init3D(void);

	// 更新
	void Update(void);

	// 描画
	void Draw(void);

	// リソースの破棄
	void Destroy(void);

	// 状態遷移
	void ChangeScene(SCENE_ID nextId);

	// シーンIDの取得
	SCENE_ID GetSceneID(void);

	// デルタタイムの取得
	float GetDeltaTime(void) const;

	bool GetGameClearState();

	void SetGameClearState(bool clear);

	bool GetSpecialGameOver();

	void SetGameOverState(bool over);

	bool GetNowGame();
	
	void SetGameOverType(GAME_OVER_TYPE type);
	

	GAME_OVER_TYPE GetGameOverType() const
	{
		return gameOverType_;
	}

	void SetClearScore(int score)
	{
		score_ = score;
	}

	int GetClearScore() const
	{
		return score_;
	}

private:

	// 静的インスタンス
	static SceneManager* instance_;

	SCENE_ID sceneId_;
	SCENE_ID waitSceneId_;

	// フェード
	Fader* fader_;

	// 各種シーン
	SceneBase* scene_;

	//プレイヤー
	Player* player_;

	GAME_OVER_TYPE gameOverType_;

	// シーン遷移中判定
	bool isSceneChanging_;

	//クリア判定
	bool isClear_;

	//死んだ判定
	bool over_;

	//ゲーム判定用
	bool gameNow_;

	//スコア
	int score_;

	// デルタタイム
	std::chrono::system_clock::time_point preTime_;
	float deltaTime_;

	// デフォルトコンストラクタをprivateにして、
	// 外部から生成できない様にする
	SceneManager(void);

	// コピーコンストラクタも同様
	SceneManager(const SceneManager& instance) = default;

	// デストラクタも同様
	~SceneManager(void) = default;

	// デルタタイムをリセットする
	void ResetDeltaTime(void);

	// シーン遷移
	void DoChangeScene(SCENE_ID sceneId);

	// フェード
	void Fade(void);

};