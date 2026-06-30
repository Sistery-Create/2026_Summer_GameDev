#include "Enemy.h"
#include"../../../Utility/AsoUtility.h"
#include"../../Actor/Player/Player.h"
#include"../../Actor/Item/item.h"
#include"../../../Object/AnimationController.h"
#include"../../../Scene/GameScene.h"
#include"../../../Scene/GameClearScene.h"
#include"../../../Manager/SoundManager.h"
#include"../../../Application.h"
#include "EnemyBase.h"
Enemy::Enemy()
{
}

Enemy::~Enemy()
{
}

void Enemy::Init()
{
	// Transform初期化
	InitTransform();

	// アニメーションの初期化
	InitAnimation();

	// 初期化後の個別処理
	InitPost();	
}

void Enemy::Load()
{
	// リソースロード
	InitLoad();
}

void Enemy::LoadEnd()
{
	Init();
}

void Enemy::Update()
{

	//プレイヤーを捜査する関数
	Search();

	pos_.y = DEFAULT_POS.y;
	MV1SetPosition(modelId_, pos_);
	MV1SetRotationXYZ(modelId_, rot_);
	MV1SetScale(modelId_, scale_);
}

void Enemy::Draw()
{

}

void Enemy::Release()
{
	MV1DeleteModel(modelId_);
	// アニメーション制御の解放
	if (animationController_ != nullptr)
	{
		animationController_->Release();
		delete animationController_;
		animationController_ = nullptr;
	}
}

VECTOR Enemy::GetPos()
{
	return pos_;
}

void Enemy::InitLoad(void)
{
	modelId_ = MV1LoadModel("Data/Model/Enemy/Spider.mv1");
}

void Enemy::InitTransform(void)
{
	pos_ = DEFAULT_POS;
	rot_ = VGet(0.0f, AsoUtility::Deg2RadF(-90.0), 0.0f);
	scale_ = DEFAULT_SCALE;
	
	activeFlag_ = false;

	// 当たり判定用カプセルの始点・終点・半径の初期化
	startCapsulePos_ = CAPSULE_START_POS;
	endCapsulePos_ = CAPSULE_END_POS;
	capsuleRadius_ =CAPSULE_RADIUS;

	MV1SetPosition(modelId_, pos_);
	MV1SetRotationXYZ(modelId_, rot_);
	MV1SetScale(modelId_, scale_);

	isEnemySoundPlayed_ = false;
	isBGMSoundPlayed_ = false;
}

void Enemy::InitAnimation(void)
{
	// アニメーション制御の初期化
	animationController_ = new AnimationController(modelId_);
}

void Enemy::InitPost(void)
{
}

void Enemy::FollowPlayer()
{
	
}


void Enemy::Search()
{
	
}

void Enemy::SetPos(VECTOR pos)
{
	pos_ = pos;
	MV1SetPosition(modelId_, pos_);
}
 