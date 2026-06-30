#pragma once
#include "SceneBase.h"
#include<string>
class SceneManager;
class GameClearScene : public SceneBase
{
public:

	static constexpr int MENU_CONTINUE = 0;
	static constexpr int MENU_TITLE = 1;
	// コンストラクタ
	GameClearScene(void);
	// デストラクタ
	~GameClearScene();
	void Load(void)override;
	void Init(void)override;
	void Update(void)override;
	void Draw(void)override;
	void Release(void)override;

	void SetGameScore(int score)
	{
		gameScore_ = score;
	}
private:
	// シーンの状態
	enum class STATE
	{
		WAIT,   // 入力待ち
		SHAKE,  // 揺れ演出中
		END     // 終了（次のシーンへ遷移）
	};

	STATE state_;
	int timer_;

	int image_;
	int kizu_;

	int gameScore_;
	bool waitSound_;

	std::string clearMessage_;
	int messageTimer_;
	int messageSpeed_;
	int menuSelection_ = 0;
};

