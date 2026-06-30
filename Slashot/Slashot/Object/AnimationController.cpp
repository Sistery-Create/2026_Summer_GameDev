#include<algorithm>
#include <DxLib.h>
#include "../Manager/SceneManager.h"
#include "AnimationController.h"

AnimationController::AnimationController(int modelId)
{
	modelId_ = modelId;

	playType_ = -1;

	blendTime_ = 10.0f;
	blendStep_ = 0.0f;

	isBlending_ = false;
	isLoop_ = false;

	playAnim_ = {};
	preAnim_ = {};
}

AnimationController::~AnimationController(void)
{
}

void AnimationController::Add(int type, float speed, const std::string path)
{
	Animation animation{};

	animation.model =
		MV1LoadModel(path.c_str());

	animation.animIndex = 0;
	animation.speed = speed;
	animation.step = 0.0f;

	// attach
	animation.attachNo =
		MV1AttachAnim(
			modelId_,
			0,
			animation.model);

	animation.totalTime =
		MV1GetAttachAnimTotalTime(
			modelId_,
			animation.attachNo);

	// 最初は再生率0
	MV1SetAttachAnimBlendRate(
		modelId_,
		animation.attachNo,
		0.0f);

	animations_.emplace(type, animation);
}

void AnimationController::AddInFbx(int type, float speed, int animIndex)
{
	Animation animation{};

	animation.model = -1;
	animation.animIndex = animIndex;
	animation.speed = speed;
	animation.step = 0.0f;

	
	animation.attachNo =
		MV1AttachAnim(
			modelId_,
			animIndex,
			-1);

	animation.totalTime =
		MV1GetAttachAnimTotalTime(
			modelId_,
			animation.attachNo);

	MV1SetAttachAnimBlendRate(
		modelId_,
		animation.attachNo,
		0.0f);

	animations_.emplace(type, animation);
}

void AnimationController::Play(int type, bool isLoop, PLAY_MODE mode, bool forceRestart)
{
	if (playType_ == type && !forceRestart)
	{
		return;
	}

	preAnim_ = playAnim_;

	playType_ = type;
	playAnim_ = animations_[type];

	playAnim_.step = 0.0f;

	blendStep_ = 0.0f;

	isBlending_ = true;

	isLoop_ = isLoop;

	playMode_ = mode;

	//printfDx("Play : %d\n", type);
}



void AnimationController::Update(void)
{

	float deltaTime = SceneManager::GetInstance().GetDeltaTime();

	if (!isPause_)
	{
		playAnim_.step += deltaTime * playAnim_.speed;

		if (playAnim_.step > playAnim_.totalTime)
		{
			playAnim_.step = isLoop_ ? 0.0f : playAnim_.totalTime;
		}

		preAnim_.step += deltaTime * preAnim_.speed;

		if (preAnim_.step > preAnim_.totalTime)
		{
			preAnim_.step = 0.0f;
		}
	}

	MV1SetAttachAnimTime(modelId_, playAnim_.attachNo, playAnim_.step);
	MV1SetAttachAnimTime(modelId_, preAnim_.attachNo, preAnim_.step);

	// ブレンド率計算
	float rate = 1.0f;
	if (isBlending_) {
		blendStep_ += 1.0f;
		rate = std::clamp(blendStep_ / blendTime_, 0.0f, 1.0f);
		if (rate >= 1.0f) isBlending_ = false;
	}

	// 全アニメーションを一旦リセット
	for (auto& pair : animations_) {
		MV1SetAttachAnimBlendRate(modelId_, pair.second.attachNo, 0.0f);
	}

	if (playMode_ == PLAY_MODE::FULL_BODY) {
		MV1SetAttachAnimBlendRate(modelId_, playAnim_.attachNo, 1.0f);
	}
	else if (playMode_ == PLAY_MODE::UPPER_BODY) {
		int hipsFrame = 1;
		MV1SetAttachAnimBlendRate(modelId_, preAnim_.attachNo, 1.0f);

		MV1SetAttachAnimBlendRateToFrame(modelId_, preAnim_.attachNo, upperBodyFrame_, 1.0f - rate, true);

		MV1SetAttachAnimBlendRate(modelId_, playAnim_.attachNo, 1.0f);

		MV1SetAttachAnimBlendRateToFrame(modelId_, playAnim_.attachNo, 0, 0.0f, true);

		MV1SetAttachAnimBlendRateToFrame(modelId_, playAnim_.attachNo, upperBodyFrame_, rate, true);

		MV1SetAttachAnimBlendRateToFrame(modelId_, playAnim_.attachNo, hipsFrame, 0.0f, false);
	}

	animations_[playType_].step = playAnim_.step;

}

void AnimationController::Release(void)
{

	// 外部FBXのモデル(アニメーション)解放
	for (const std::pair<int, Animation>& pair : animations_)
	{
		if (pair.second.model != -1)
		{
			MV1DeleteModel(pair.second.model);
		}
	}

	// 可変長配列をクリアする
	animations_.clear();

}

int AnimationController::GetPlayType(void) const
{
	return playType_;
}

bool AnimationController::IsEnd(int type) const
{

	// 再生中のアニメーションと違うなら終了してない
	if (playType_ != type)
	{
		return false;
	}

	// ループしているなら終了しない
	if (isLoop_)
	{
		return false;
	}

	// 再生終了判定
	if (playAnim_.step >= playAnim_.totalTime)
	{
		return true;
	}

	return false;

}

// 指定したアニメーションが再生中か
bool AnimationController::IsPlaying(int type) const
{
	//アニメーションタイプ不一致
	if (playType_ != type) return false;

	// ループ中なら再生中
	if (isLoop_) return true;

	// ループなしなら再生時間が合計時間に達していなければ再生中
	return playAnim_.step < playAnim_.totalTime;
}

float AnimationController::GetProgress() const
{
	// アタッチされていない場合は0
	if (playAnim_.attachNo == -1) return 0.0f;

	// DXライブラリの関数を呼ぶ必要はありません。
	// Updateで計算している playAnim_.step を直接使いましょう。
	if (playAnim_.totalTime <= 0.0f) return 0.0f;

	float progress = playAnim_.step / playAnim_.totalTime;

	// 1.0を超えないようにクランプ
	return std::clamp(progress, 0.0f, 1.0f);
}

void AnimationController::SetPartialBlend(int attachNo, int frame, float rate, bool recursive)
{
	MV1SetAttachAnimBlendRateToFrame(
		modelId_,
		attachNo,
		frame,
		rate,
		recursive);
}

void AnimationController::Add(int type, float speed, Animation& animation)
{
	animation.speed = speed;

	if (animations_.count(type) == 0)
	{
		// 追加
		animations_.emplace(type, animation);
	}
}
