#include "EnemyManager.h"
#include"../../Actor/Player/Player.h"
#include"EnemyBase.h"

EnemyManager::EnemyManager(Player* player)
{
}

EnemyManager::~EnemyManager(void)
{
}

void EnemyManager::Init(void)
{
	enemys_.emplace_back(new EnemyBase());
	for (EnemyBase* enemy : enemys_)
	{
		enemy->Load();
	}
}

void EnemyManager::Update(void)
{
	for (EnemyBase* enemy : enemys_)
	{
		enemy->Update();
	}
}

void EnemyManager::Draw(void)
{
	/*for (EnemyBase* enemy : enemys_)
	{
		enemy->Draw();
	}*/
}

void EnemyManager::Release(void)
{
	for (EnemyBase* enemy : enemys_)
	{
		enemy->Release();
		delete enemy;
	}
	enemys_.clear();
}

