#include "TitleScene.h"
#include"../Manager/SceneManager.h"
#include"../Manager/InputManager.h"
#include"../Manager/SoundManager.h"
#include"../Application.h"
#include <DxLib.h>

TitleScene::TitleScene()
{
}

TitleScene::~TitleScene()
{
}

void TitleScene::Load(void)
{
	

	image_ = LoadGraph("Data/Image/UI/Black.png");
	kizu_ = LoadGraph("Data/Kizuato.png");
	logoFont_ = CreateFontToHandle(
		"Impact",
		240,
		5,
		DX_FONTTYPE_ANTIALIASING_EDGE
	);
	enterFont_ = CreateFontToHandle(
		"Impact",
		72,
		5,
		DX_FONTTYPE_ANTIALIASING_EDGE
	);
	waitSound_ = false;

	playBGM_ = true;

	// èâä˙èÛë‘ÇÕì¸óÕë“Çø
	state_ = STATE::WAIT;
	timer_ = 0;
	particles_.clear();

	for (int i = 0; i < 40; i++)
	{
		Particle p;

		p.pos = VGet(
			rand() % 1920,
			rand() % 1080,
			0
		);

		p.speed =
			20.0f +
			rand() % 40;

		p.alpha =
			100 +
			rand() % 156;

		particles_.push_back(p);
	}

	
	
}

void TitleScene::Init(void)
{
	
}

void TitleScene::Update(void)
{

	auto& ins = InputManager::GetInstance();

	if (playBGM_)
	{
		SoundManager::GetInstance().Play(SoundManager::SRC::TITLE, Sound::TIMES::LOOP);

		playBGM_ = false;
	}

	
	
	if (ins.IsTrgDown(KEY_INPUT_UP)||ins.IsTrgDown(KEY_INPUT_W))
	{
		SoundManager::GetInstance().Play(SoundManager::SRC::MENU_CARSOL, Sound::TIMES::ONCE);
		if (menu_ == MENU::START)
		{
			menu_ = MENU::EXIT;
		}
		else
		{
			menu_ = MENU::START;
		}
	}

	if (ins.IsTrgDown(KEY_INPUT_DOWN) || ins.IsTrgDown(KEY_INPUT_S))
	{
		SoundManager::GetInstance().Play(SoundManager::SRC::MENU_CARSOL, Sound::TIMES::ONCE);
		if (menu_ == MENU::START)
		{
			menu_ = MENU::EXIT;
		}
		else
		{
			menu_ = MENU::START;
		}
	}
	if (ins.IsTrgDown(KEY_INPUT_SPACE))
	{
		SoundManager::GetInstance().Play(SoundManager::SRC::MENU_CARSOL, Sound::TIMES::ONCE);
		switch (menu_)
		{
		case MENU::START:

			SceneManager::GetInstance().
				ChangeScene(
					SceneManager::SCENE_ID::LOAD);
			SoundManager::GetInstance().Stop(SoundManager::SRC::TITLE);

			break;

		case MENU::EXIT:

			Application::EndGame();

			break;
		}

		
	}
	
	for (auto& p : particles_)
	{
		p.pos.y -=
			p.speed *
			SceneManager::GetInstance().GetDeltaTime();

		if (p.pos.y < 0)
		{
			p.pos.y = 1080;

			p.pos.x =
				rand() % 1920;
		}
	}
}

void TitleScene::Draw(void)
{
	DrawGraph(0, 0, image_, true);

	const char* title = "SLASHOT";

	int width =
		GetDrawStringWidthToHandle(
			title,
			strlen(title),
			logoFont_
		);

	int x = (1920 - width) / 2;

	DrawStringToHandle(
		x + 10,
		305,
		title,
		GetColor(50, 100, 255),
		logoFont_
	);

	DrawStringToHandle(
		x,
		300,
		title,
		COLOR_WHITE,
		logoFont_
	);

	


	for (auto& p : particles_)
	{
		SetDrawBlendMode(
			DX_BLENDMODE_ALPHA,
			p.alpha
		);

		DrawCircle(
			static_cast<int>(p.pos.x),
			static_cast<int>(p.pos.y),
			6,
			COLOR_BLUE,
			TRUE
		);
	}

	SetDrawBlendMode(
		DX_BLENDMODE_NOBLEND,
		0
	);
	int startY = 650;
	int exitY = 730;

	
	DrawStringToHandle(
		800,
		startY,
		"Game Start",
		COLOR_YELLOW,
		enterFont_
	);

	DrawStringToHandle(
		800,
		exitY,
		"Exit",
		COLOR_YELLOW,
		enterFont_
	);

	if (menu_ == MENU::START)
	{
		int offset =
			(int)(
				sin(
					GetNowCount()
					* 0.01
				)
				* 10
				);

		DrawStringToHandle(
			700 + offset,
			startY-10,
			"Å®",
			COLOR_WHITE,
			enterFont_
		);

	}
	else
	{
		int offset =
			(int)(
				sin(
					GetNowCount()
					* 0.01
				)
				* 10
				);
		DrawStringToHandle(
			700 + offset,
			exitY-10,
			"Å®",
			COLOR_WHITE,
			enterFont_
		);
	}
}

void TitleScene::Release(void)
{
	
}
