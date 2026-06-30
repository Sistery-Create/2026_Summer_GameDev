#pragma once
#include "SceneBase.h"
#include <DxLib.h>
#include <vector>
#include<string>
class SceneManager;
class TitleScene : public SceneBase
{
public:
	
	struct Particle
	{
		VECTOR pos;
		float speed;
		int alpha;
	};
	enum class MENU
	{
		START,
		EXIT,

		MAX
	};

	MENU menu_ = MENU::START;
	static constexpr unsigned int COLOR_GREEN = 0x00FF00;
	static constexpr unsigned int COLOR_YELLOW = 0x00FFFF00;
	static constexpr unsigned int COLOR_RED = 0xFF0000;
	static constexpr unsigned int COLOR_BLACK = 0x000000;
	static constexpr unsigned int COLOR_BLUE = 0x0000FF;
	static constexpr unsigned int COLOR_WHITE = 0xFFFFFF;
	// コンストラクタ
	TitleScene(void);
	// デストラクタ
	~TitleScene(void)override;

	void Load(void)override;
	void Init(void)override;
	void Update(void)override;
	void Draw(void)override;
	void Release(void)override;
private:
	// シーンの状態
	enum class STATE
	{
		WAIT,   // 入力待ち
		SHAKE,  // 揺れ演出中
		END     // 終了（次のシーンへ遷移）
	};
	std::vector<Particle> particles_;
	STATE state_;
	int timer_;

	int image_;
	int kizu_;
	int logoFont_;
	int enterFont_;
	bool waitSound_;
	bool playBGM_;
};

