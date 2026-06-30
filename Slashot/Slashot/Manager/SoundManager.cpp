#include <DxLib.h>
#include "../Application.h"
#include "SoundManager.h"
#include"../Object/Actor/Enemey/Enemy.h"
SoundManager* SoundManager::instance_ = nullptr;

void SoundManager::CreateInstance(void)
{
	if (instance_ == nullptr)
	{
		instance_ = new SoundManager();
	}
	instance_->Init();
}

SoundManager& SoundManager::GetInstance(void)
{
	return *instance_;
}

void SoundManager::Init(void)
{

	Sound res;

	
	res = Sound(Sound::TYPE::SOUND_2D, "Data/Music/SE/Walk.mp3");
	res.ChengeMaxVolume(2.0);
	soundMap_.emplace(SRC::WALK, res);

	res = Sound(Sound::TYPE::SOUND_2D, "Data/Music/SE/DrawGun.mp3");
	res.ChengeMaxVolume(2.0);
	soundMap_.emplace(SRC::DRAW_GUN, res);

	res = Sound(Sound::TYPE::SOUND_2D, "Data/Music/SE/DrawSword.mp3");
	res.ChengeMaxVolume(2.0);
	soundMap_.emplace(SRC::DRAW_SWORD, res);

	res = Sound(Sound::TYPE::SOUND_2D, "Data/Music/SE/EnemyDeath.mp3");
	res.ChengeMaxVolume(2.0);
	soundMap_.emplace(SRC::ENEMY_DEATH, res);

	res = Sound(Sound::TYPE::SOUND_2D, "Data/Music/SE/MenuCarsol.mp3");
	res.ChengeMaxVolume(2.0);
	soundMap_.emplace(SRC::MENU_CARSOL, res);

	res = Sound(Sound::TYPE::SOUND_2D, "Data/Music/SE/MenuOpend.mp3");
	res.ChengeMaxVolume(2.0);
	soundMap_.emplace(SRC::MENU_OPEN, res);

	res = Sound(Sound::TYPE::SOUND_2D, "Data/Music/SE/Healing.mp3");
	res.ChengeMaxVolume(0.8);
	soundMap_.emplace(SRC::HEALING, res);

	res = Sound(Sound::TYPE::SOUND_2D, "Data/Music/SE/PowerUp.mp3");
	res.ChengeMaxVolume(0.8);
	soundMap_.emplace(SRC::POWER_UP, res);

	res = Sound(Sound::TYPE::SOUND_2D, "Data/Music/SE/SpelAttack.mp3");
	res.ChengeMaxVolume(2.0);
	soundMap_.emplace(SRC::SPEL_ATTACK, res);

	res = Sound(Sound::TYPE::SOUND_2D, "Data/Music/SE/SwordAttack.mp3");
	res.ChengeMaxVolume(0.8);
	soundMap_.emplace(SRC::SWORD_ATTACK, res);

	res = Sound(Sound::TYPE::SOUND_2D, "Data/Music/SE/SwordHit.mp3");
	res.ChengeMaxVolume(0.8);
	soundMap_.emplace(SRC::SWORD_HIT, res);

	res = Sound(Sound::TYPE::SOUND_2D, "Data/Music/SE/Gun_Attack.mp3");
	res.ChengeMaxVolume(0.8);
	soundMap_.emplace(SRC::GUN_SHOT, res);

	res = Sound(Sound::TYPE::SOUND_2D, "Data/Music/SE/Kick.mp3");
	res.ChengeMaxVolume(1.5);
	soundMap_.emplace(SRC::KICK, res);

	res = Sound(Sound::TYPE::SOUND_2D, "Data/Music/SE/SwordAttack2.mp3");
	res.ChengeMaxVolume(1.5);
	soundMap_.emplace(SRC::SWORD_ATTACK2, res);

	res = Sound(Sound::TYPE::SOUND_2D, "Data/Music/SE/offGun.mp3");
	res.ChengeMaxVolume(1.5);
	soundMap_.emplace(SRC::OFFGUN, res);

	res = Sound(Sound::TYPE::SOUND_2D, "Data/Music/SE/offSword.mp3");
	res.ChengeMaxVolume(1.5);
	soundMap_.emplace(SRC::OFSWORD, res);

	res = Sound(Sound::TYPE::BGM, "Data/Music/BGM/Title.mp3");
	res.ChengeMaxVolume(1.5);
	soundMap_.emplace(SRC::TITLE, res);

	res = Sound(Sound::TYPE::BGM, "Data/Music/BGM/Battle.mp3");
	res.ChengeMaxVolume(1.5);
	soundMap_.emplace(SRC::BATTLE, res);

	res = Sound(Sound::TYPE::BGM, "Data/Music/BGM/GameOver.mp3");
	res.ChengeMaxVolume(1.5);
	soundMap_.emplace(SRC::GAME_OVER, res);

	res = Sound(Sound::TYPE::BGM, "Data/Music/BGM/Victory.mp3");
	res.ChengeMaxVolume(1.5);
	soundMap_.emplace(SRC::CLEAR, res);

}

void SoundManager::Release(void)
{
	for (auto& p : soundMap_)
	{
		p.second.Release();  // サウンドのリソースを解放

		// p.second が動的に確保されたオブジェクトであれば、以下のようにメモリ解放
		// delete p.second; // 必要に応じて、メモリを解放する

	}

	soundMap_.clear();  // マップをクリア
}

void SoundManager::Destroy(void)
{
	printfDx(
		"Destroy SoundManager = %p\n",
		instance_
	);
	Release();
	soundMap_.clear();
	delete instance_;
}

//音の再生
bool SoundManager::Play(SRC src, Sound::TIMES times, bool isForce)
{
	const auto& lPair = soundMap_.find(src);
	if (lPair != soundMap_.end())
	{
		if (!lPair->second.CheckLoad())
		{
			lPair->second.Load();
		}
		return lPair->second.Play(times, isForce);
	}
	return false;
}
bool SoundManager::Play(SRC src, Sound::TIMES times, VECTOR pos, float radius)
{
	const auto& lPair = soundMap_.find(src);
	if (lPair != soundMap_.end())
	{
		if (!lPair->second.CheckLoad())
		{
			lPair->second.Load();
		}
		return lPair->second.Play(pos, radius, times);
	}
	return false;
}

void SoundManager::Stop(SRC src)
{
	const auto& lPair = soundMap_.find(src);
	if (lPair != soundMap_.end())//指定のものが存在するか？
	{
		return lPair->second.Stop();
	}
}
void SoundManager::AllStop(void)
{
	// soundMap_ のすべての要素に対して Stop() を呼び出す
	for (auto& lPair : soundMap_)
	{
		lPair.second.Stop();
	}
}
//再生中か?
bool SoundManager::CheckMove(SRC src)
{
	const auto& lPair = soundMap_.find(src);
	if (lPair != soundMap_.end())
	{
		return lPair->second.CheckMove();
	}
	return false;
}
//音量
void SoundManager::ChengeVolume(SRC src, float per)
{
	const auto& lPair = soundMap_.find(src);
	if (lPair != soundMap_.end())
	{
		return lPair->second.ChengeVolume(per);
	}
}

bool SoundManager::IsEnded(SRC src)
{
	const auto& lPair = soundMap_.find(src);
	if (lPair != soundMap_.end())
	{
		return lPair->second.IsEnded();
	}
	return true;  // 存在しなければ再生終了
}
