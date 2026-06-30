#pragma once
#include<map>
#include"Sound.h"

class SoundManager
{
public:
	//リソース名
	enum class SRC
	{
		WALK,
		DRAW_GUN,
		DRAW_SWORD,
		ENEMY_DEATH,
		MENU_CARSOL,
		MENU_OPEN,
		HEALING,
		POWER_UP,
		SPEL_ATTACK,
		SWORD_ATTACK,
		GUN_SHOT,
		SWORD_HIT,
		KICK,
		SWORD_ATTACK2,
		OFFGUN,
		OFSWORD,
		TITLE,
		BATTLE,
		GAME_OVER,
		CLEAR,
		MAX
	};

	// 明示的にインステンスを生成する
	static void CreateInstance(void);

	// 静的インスタンスの取得
	static SoundManager& GetInstance(void);

	// 初期化
	void Init(void);

	// 解放(シーン切替時に一旦解放)
	void Release(void);

	// リソースの完全破棄
	void Destroy(void);

	bool Play(SRC src, Sound::TIMES times, bool isForce = false);	//二次元音源用
	bool Play(SRC src, Sound::TIMES times, VECTOR pos, float radius);	//三次元音源用

	void Stop(SRC src);	//音源を停止する
	void AllStop(void);	//音源を停止する

	bool CheckMove(SRC src);

	void ChengeVolume(SRC src, float per);	//per 0.0～1.0でパーセントを設定する

	bool IsEnded(SRC src);
private:
	// 静的インスタンス
	static SoundManager* instance_;

	std::map<SRC, Sound> soundMap_;

	// デフォルトコンストラクタをprivateにして、
	// 外部から生成できない様にする
	SoundManager(void) = default;

	// デストラクタも同様
	~SoundManager(void) = default;
};
