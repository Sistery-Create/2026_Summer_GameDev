#include "NightDoom.h"

NightDoom::NightDoom()
{
}

NightDoom::~NightDoom()
{
}

void NightDoom::Load(void)
{
}

void NightDoom::Init(void)
{
	modelId_ = MV1LoadModel("Data/Model/Stage/Skydome.mv1");
	pos_ = { 0.0f,0.0f,0.0f };
	scl_ = { 1.0f,1.0f,1.0f };
	rot_ = { 0.0f,0.0f,0.0f };

	MV1SetPosition(modelId_, pos_);
	MV1SetScale(modelId_, scl_);
	MV1SetRotationXYZ(modelId_, rot_);
}

void NightDoom::Update(void)
{
	rot_.y += 0.0007f;

	MV1SetRotationXYZ(modelId_, rot_);
}

void NightDoom::Draw(void)
{
	SetUseLighting(FALSE);
	MV1DrawModel(modelId_);
	SetUseLighting(TRUE);
}

void NightDoom::Release(void)
{
}
