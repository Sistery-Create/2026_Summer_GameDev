#pragma once
#include <vector>
#include <string>
#include <DxLib.h>

class Sound
{
public:

	enum class TYPE
	{
		NONE,
		SOUND_2D,
		SOUND_3D,
		BGM
	};

	enum class TIMES
	{
		ONCE,
		LOOP,
	};

	Sound(void);
	Sound(TYPE type, const std::string& path);
	~Sound(void);

	void Update(VECTOR pos);

	void Load(void);
	void Release(void);

	bool Play(TIMES times, bool isForce = false);
	bool Play(VECTOR pos, float radius, TIMES times);

	void Stop(void);
	bool CheckMove(void);
	bool CheckLoad(void);
	bool IsEnded();

	void ChengeVolume(float per);
	void ChengeMaxVolume(float per);

private:
	TYPE soundType_;
	std::string path_;

	int baseHandleId_;              // 元ハンドル
	std::vector<int> dupHandles_;   // 複製ハンドル群

	int maxVolume_;

	// 3D用
	VECTOR pos_;
	float radius_;
};