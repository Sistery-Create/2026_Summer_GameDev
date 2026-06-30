#include "Sound.h"

Sound::Sound(void)
{
	soundType_ = TYPE::NONE;
	path_ = "";
	baseHandleId_ = -1;
	pos_ = { 0.0f, 0.0f, 0.0f };
	radius_ = 0.0f;
	maxVolume_ = 255;
}

Sound::Sound(TYPE type, const std::string& path)
{
	soundType_ = type;
	path_ = path;
	baseHandleId_ = -1;
	pos_ = { 0.0f, 0.0f, 0.0f };
	radius_ = 0.0f;
	maxVolume_ = 255;


}

Sound::~Sound(void)
{
	Release();
}

void Sound::Update(VECTOR pos)
{
	if (soundType_ != TYPE::SOUND_3D)
		return;

	// 3Dサウンドが再生中の複製ハンドル全てに位置をセット
	for (int handle : dupHandles_)
	{
		if (CheckSoundMem(handle) == 1)
		{
			Set3DPositionSoundMem(pos, handle);
		}
	}
	pos_ = pos;
}

void Sound::Load(void)
{
	if (soundType_ == TYPE::NONE)
		return;

	SetCreate3DSoundFlag(soundType_ == TYPE::SOUND_3D);

	baseHandleId_ = LoadSoundMem(path_.c_str());

	if (baseHandleId_ == -1)
		return;

	dupHandles_.clear();

	// BGMなら複製しない
	if (soundType_ == TYPE::BGM)
	{
		dupHandles_.push_back(baseHandleId_);
		return;
	}

	// SEなら複製する
	const int poolSize = 5;

	for (int i = 0; i < poolSize; i++)
	{
		int dup = DuplicateSoundMem(baseHandleId_);

		if (dup != -1)
		{
			dupHandles_.push_back(dup);
		}
	}
}

void Sound::Release(void)
{
	for (int handle : dupHandles_)
	{
		if (CheckSoundMem(handle) == 1)
		{
			StopSoundMem(handle);
		}
		DeleteSoundMem(handle);
	}
	dupHandles_.clear();

	if (baseHandleId_ != -1)
	{
		if (CheckSoundMem(baseHandleId_) == 1)
		{
			StopSoundMem(baseHandleId_);
		}
		DeleteSoundMem(baseHandleId_);
		baseHandleId_ = -1;
	}
}

bool Sound::Play(TIMES times, bool isForce)
{
	

	for (int handle : dupHandles_)
	{
		if (CheckSoundMem(handle) == 0)  // 再生中でないハンドルを探す
		{
			int ret = PlaySoundMem(handle, times == TIMES::ONCE ? DX_PLAYTYPE_BACK : DX_PLAYTYPE_LOOP, true);
			if (ret == 0)
			{
				ChengeVolume(1.0f);
				return true;
			}
		}
	}

	if (isForce && !dupHandles_.empty())
	{
		StopSoundMem(dupHandles_[0]);
		int ret = PlaySoundMem(dupHandles_[0], times == TIMES::ONCE ? DX_PLAYTYPE_BACK : DX_PLAYTYPE_LOOP, true);
		if (ret == 0)
		{
			ChengeVolume(1.0f);
			return true;
		}
	}

	return false;
}

bool Sound::Play(VECTOR pos, float radius, TIMES times)
{
	if (soundType_ != TYPE::SOUND_3D)
		return false;

	for (int handle : dupHandles_)
	{
		if (CheckSoundMem(handle) == 0)
		{
			Set3DPositionSoundMem(pos, handle);
			Set3DRadiusSoundMem(radius, handle);

			int ret = PlaySoundMem(handle, times == TIMES::ONCE ? DX_PLAYTYPE_BACK : DX_PLAYTYPE_LOOP, true);
			if (ret == 0)
			{
				pos_ = pos;
				radius_ = radius;
				ChengeVolume(1.0f);
				return true;
			}
		}
	}

	return false;
}

void Sound::Stop(void)
{
	for (int handle : dupHandles_)
	{
		if (CheckSoundMem(handle) == 1)
		{
			StopSoundMem(handle);
		}
	}
	pos_ = { 0.0f, 0.0f, 0.0f };
	radius_ = 0.0f;
}

bool Sound::CheckMove(void)
{
	for (int handle : dupHandles_)
	{
		if (CheckSoundMem(handle) == 1)
		{
			return true;
		}
	}
	return false;
}

bool Sound::CheckLoad(void)
{
	return baseHandleId_ != -1;
}

bool Sound::IsEnded()
{
	for (int handle : dupHandles_)
	{
		if (CheckSoundMem(handle) == 1)  // 再生中のものが1つでもあれば false
		{
			return false;
		}
	}
	return true;  // すべて再生終了していれば true
}

void Sound::ChengeVolume(float per)
{
	if (per > 1.0f)
		per = 1.0f;
	if (per < 0.0f)
		per = 0.0f;

	for (int handle : dupHandles_)
	{
		ChangeVolumeSoundMem(static_cast<int>(per * maxVolume_), handle);
	}
}

void Sound::ChengeMaxVolume(float per)
{
	maxVolume_ = static_cast<int>(255 * per);
}