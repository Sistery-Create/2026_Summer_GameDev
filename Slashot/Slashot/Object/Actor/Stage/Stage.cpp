#include "Stage.h"
#include"../../../Application.h"
#include"../Collider/ColliderModel.h"
Stage::Stage()
{
}

Stage::~Stage()
{
}

void Stage::Init()
{
	InitLoad();
	InitTransform();
	InitAnimation();
	InitCollider();
	InitPost();

	transform_.Update();
}

void Stage::Update()
{
}

void Stage::Draw()
{
	MV1SetScale(transform_.modelId, transform_.scl);
	MV1SetPosition(transform_.modelId, transform_.pos);
	MV1DrawModel(transform_.modelId);
}

int Stage::GetModelId(void)
{
	return transform_.modelId;
}

void Stage::InitLoad()
{
	transform_.modelId = MV1LoadModel("Data/Model/Stage/You Noob.mv1");
}

void Stage::InitTransform()
{
	transform_.pos = { 0.0f,0.0f,0.0f };
	transform_.scl = { 0.1f,0.1f,0.1f };
	transform_.Update();
}

void Stage::InitCollider(void)
{
	// DxLib側の衝突情報セットアップ
	MV1SetupCollInfo(transform_.modelId);
	// モデルのコライダ
	ColliderModel* colModel =
		new ColliderModel(ColliderBase::TAG::STAGE, &transform_);

	for (const std::string& name : EXCLUDE_FRAME_NAMES)
	{
		colModel->AddExcludeFrameIds(name);
	}
	ownColliders_.emplace(static_cast<int>(COLLIDER_TYPE::MODEL), colModel);
}

void Stage::InitAnimation()
{
}

void Stage::InitPost()
{
}
