#include "../../Application.h"
#include "../../Utility/AsoUtility.h"
#include "../../Utility/MatrixUtility.h"
#include"Collider/ColliderBase.h"
#include "../Transform.h"
#include"../../Manager/SceneManager.h"
#include "ActorBase.h"

ActorBase::ActorBase(void)
	:
	scnMng_(SceneManager::GetInstance()),
	transform_()
{
	animationController_ = nullptr;

	animType_ = 0;

	modelId_ = -1;
	pos_ = { 0.0f,0.0f,0.0f };
	angle_ = { 0.0f,0.0f,0.0f };
	localAngle_ = { 0.0f,0.0f,0.0f };
	scale_ = { 0.0f,0.0f,0.0f };

	startCapsulePos_ = { 0.0f,0.0f,0.0f };
	endCapsulePos_ = { 0.0f,0.0f,0.0f };
	capsuleRadius_ = 0.0f;

	preInputDir_ = { 0.0f,0.0f,0.0f };

	moveDir_ = { 0.0f,0.0f,0.0f };
	jumpPow_ = 0.0f;
	isCollision_ = false;
}

ActorBase::~ActorBase(void)
{
}

void ActorBase::Init(void)
{
	// Transform初期化
	InitTransform();

	// アニメーションの初期化
	InitAnimation();

	// 初期化後の個別処理
	InitPost();
}

void ActorBase::Load(void)
{
	// リソースロード
	InitLoad();
}

void ActorBase::LoadEnd(void)
{
	// 初期化
	Init();
}

void ActorBase::Update(void)
{
	// プレイヤーの遅延回転処理
	DelayRotate();

	// 行列の合成(子, 親と指定すると親⇒子の順に適用される)
	MATRIX mat = MatrixUtility::Multiplication(localAngle_, transform_.rot);

	// 回転行列をモデルに反映
	MV1SetRotationMatrix(transform_.modelId, mat);

	// プレイヤーの移動処理
	Move();

}

void ActorBase::Draw(void)
{
	/*DrawFormatString(0, 80, 0xFFFFFF,
		"Actor Draw ModelId : %d",
		transform_.modelId);*/

	if (transform_.modelId != -1)
	{
		MV1DrawModel(transform_.modelId);
	}

#ifdef _DEBUG
	/*for (const auto& own : ownColliders_)
	{
		own.second->Draw();
	}*/
#endif
}

void ActorBase::Release(void)
{
	MV1DeleteModel(modelId_);
	delete animationController_;
}

void ActorBase::CollisionStage(const VECTOR& pos)
{
	// 衝突判定に指定座標に押し戻す
	pos_ = pos;
	jumpPow_ = 0.0f;
}

const Transform& ActorBase::GetTransform(void) const
{
	return transform_;
}

void ActorBase::Move(void)
{
}

void ActorBase::DelayRotate(void)
{
	if (AsoUtility::EqualsVZero(moveDir_))
		return;

	// 移動方向から角度に変換する
	float goal = atan2f(moveDir_.x, moveDir_.z);

	// 常に最短経路で補間
	angle_.y = AsoUtility::LerpAngle(angle_.y, goal, 0.2f);
}

const ColliderBase* ActorBase::GetOwnCollider(int key) const
{
	if (ownColliders_.count(key) == 0)
	{
		return nullptr;
	}
	return ownColliders_.at(key);
}

void ActorBase::AddHitCollider(const ColliderBase* hitCollider)
{
	for (const auto& c : hitColliders_)
	{
		if (c == hitCollider)
		{
			return;
		}
	}
	hitColliders_.emplace_back(hitCollider);
}

void ActorBase::ClearHitCollider(void)
{
	hitColliders_.clear();
}

