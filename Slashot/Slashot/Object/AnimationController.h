#pragma once
#include <string>
#include <map>

class AnimationController
{

public:

	// アニメーションデータ
	struct Animation
	{
		int model = -1;
		int attachNo = -1;
		int animIndex = 0;
		float speed = 0.0f;
		float totalTime = 0.0f;
		float step = 0.0f;
	};

	struct PartialBlend
	{
		int frameIndex;
		float playRate;
		bool recursive;
	};

	enum class PLAY_MODE
	{
		FULL_BODY,
		UPPER_BODY,
		LOWER_BODY,
		MAX
	};
	// コンストラクタ
	AnimationController(int modelId);

	// デストラクタ
	~AnimationController(void);

	// 外部FBXからアニメーション追加
	void Add(int type, float speed, const std::string path);

	// 同じFBX内のアニメーションを準備
	void AddInFbx(int type, float speed, int animIndex);

	// アニメーション再生
	void Play(int type, bool isLoop = true, PLAY_MODE mode = PLAY_MODE::FULL_BODY,bool forceRestart=false);

	
	void Update(void);
	void Release(void);

	// 再生中のアニメーション
	int GetPlayType(void) const;

	// 再生終了
	bool IsEnd(int type) const;

	//Addで得たアニメーションデータを取得
	Animation GetAnim(int type) const { return animations_.at(type); }

	//指定したアニメーションが再生中か
	bool IsPlaying(int type) const;

	float GetProgress() const;

	void SetPartialBlend(
		int attachNo,
		int frame,
		float rate,
		bool recursive);

	void TogglePause() { isPause_ = !isPause_; }
	void Setpause(bool pause) { isPause_ = pause; }
	bool IsPause() const { return isPause_; }
	

private:

	// アニメーションするモデルのハンドルID
	int modelId_;

	// 種類別のアニメーションデータ
	std::map<int, Animation> animations_;

	std::vector<PartialBlend> upperBodyBlend_;
	std::vector<PartialBlend> lowerBodyBlend_;

	PLAY_MODE playMode_ = PLAY_MODE::FULL_BODY;

	// 再生中のアニメーション
	int playType_;
	Animation playAnim_;

	// アニメーションをループするかしないか
	bool isLoop_;

	// アニメーション追加の共通処理
	void Add(int type, float speed, Animation& animation);

	Animation preAnim_;

	float blendTime_ = 10.0f;
	float blendStep_ = 0.0f;

	bool isBlending_ = false;

	int upperBodyFrame_ = 2;
	int leftLegFrame_ = MV1SearchFrame(
		modelId_,
		"mixamorig:LeftUpLeg");
	int rightLegFrame_ = MV1SearchFrame(
		modelId_,
		"mixamorig:RightUpLeg");

	Animation baseAnim_;
	Animation upperAnim_;

	bool isUpperPlay = false;
	bool forceRestart_ = false;

	//デバッグ用
	bool isPause_ = false;	
};
