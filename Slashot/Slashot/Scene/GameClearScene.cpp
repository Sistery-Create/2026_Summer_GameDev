#include "GameClearScene.h"
#include"../Manager/SceneManager.h"
#include"../Manager/InputManager.h"
#include"../Manager/SoundManager.h"
#include"../Scene/GameScene.h"
#include"../Application.h"
#include <DxLib.h>

GameClearScene::GameClearScene()
{
}

GameClearScene::~GameClearScene()
{
}

void GameClearScene::Load(void)
{
	image_ = LoadGraph("Data/Image/UI/Black.png");
	kizu_ = LoadGraph("Data/Kizuato.png");

	waitSound_ = false;

    gameScore_ =
        SceneManager::GetInstance().GetClearScore();

	// èâä˙èÛë‘ÇÕì¸óÕë“Çø
	state_ = STATE::WAIT;
	timer_ = 0;
	menuSelection_ = 0;

	auto type =
		SceneManager::GetInstance().GetGameOverType();

	
	clearMessage_ =
			"  ÉQÅ[ÉÄÉNÉäÉAÅI";
	

	messageTimer_ = 0;
	messageSpeed_ = 4;

}

void GameClearScene::Init(void)
{

}

void GameClearScene::Update(void)
{

	auto& ins = InputManager::GetInstance();

    messageTimer_++;

 
    SoundManager::GetInstance().Play(SoundManager::SRC::CLEAR, Sound::TIMES::LOOP);

    if (messageTimer_ / messageSpeed_ >= clearMessage_.size())
    {
        if (ins.IsTrgDown(KEY_INPUT_LEFT))
        {
            menuSelection_--;

            if (menuSelection_ < 0)
            {
                menuSelection_ = 1;
            }
        }

        if (ins.IsTrgDown(KEY_INPUT_RIGHT))
        {
            menuSelection_++;

            if (menuSelection_ > 1)
            {
                menuSelection_ = 0;
            }
        }

        if (ins.IsTrgDown(KEY_INPUT_SPACE))
        {
            switch (menuSelection_)
            {
            case MENU_CONTINUE:

                SceneManager::GetInstance().ChangeScene(
                    SceneManager::SCENE_ID::LOAD
                );
                break;

            case MENU_TITLE:

                Application::EndGame();
                break;
            }

            SoundManager::GetInstance().AllStop();
        }
    }
}

void GameClearScene::Draw(void)
{
	DrawGraph(0, 0, image_, true);
    int windowX = 500;
    int windowY = 480;
    int windowW = 900;
    int windowH = 200;

    // ògê¸ÇÃêF
    unsigned int frameColor = GetColor(255, 255, 255);
    // îwåiÇÃêF
    unsigned int backgroundColor = GetColor(10, 10, 50);

    // ògê¸ÇÃï`âÊ
    DrawBox(windowX - 2, windowY - 2, windowX + windowW + 2, windowY + windowH + 2, frameColor, FALSE);

    // îºìßñæÇÃîwåiÇï`âÊ
    SetDrawBlendMode(DX_BLENDMODE_ALPHA, 200);
    DrawBox(windowX, windowY, windowX + windowW, windowY + windowH, backgroundColor, TRUE);
    SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

    SetFontSize(32);

    int charCount = messageTimer_ / messageSpeed_;
    if (charCount > clearMessage_.size()) {
        charCount = clearMessage_.size();
    }


    int x = windowX + 300;
    int y = windowY + 40;
    int lineHeight = 40;
    int menuY = windowY + 130;

    int continueX = windowX + 250;
    int titleX = windowX + 520;


    std::string currentLine;
    int currentY = y;
    int drawCount = 0;

    for (size_t i = 0; i < clearMessage_.length(); ++i)
    {
        if (drawCount >= charCount) {
            break;
        }

        if (clearMessage_[i] == '\n') {
            DrawString(x, currentY, currentLine.c_str(), 0xFFFFFF);

            currentLine.clear();
            currentY += lineHeight;
        }
        else {
            currentLine += clearMessage_[i];
        }

        drawCount++;
    }

    if (!currentLine.empty()) {
        DrawString(x, currentY, currentLine.c_str(), 0xFFFFFF);
    }


    if (charCount == clearMessage_.size()) {
        DrawFormatString(
            windowX + 350,
            windowY + 120,
            GetColor(255, 255, 0),
            "SCORE : %d",
            gameScore_
        );

        int menuY = 700;

        const char* menus[] =
        {
            "ë±ÇØÇÈ",
            "èIÇÌÇÈ"
        };
        DrawString(
            continueX,
            menuY,
            "ë±ÇØÇÈ",
            menuSelection_ == 0
            ? GetColor(255, 255, 0)
            : GetColor(255, 255, 255)
        );

        if (menuSelection_ == 0)
        {
            DrawString(
                continueX - 40,
                menuY,
                ">",
                GetColor(255, 255, 0)
            );
        }DrawString(
            titleX,
            menuY,
            "èIÇÌÇÈ",
            menuSelection_ == 1
            ? GetColor(255, 255, 0)
            : GetColor(255, 255, 255)
        );


        if (menuSelection_ == 1)
        {
            DrawString(
                titleX - 40,
                menuY,
                ">",
                GetColor(255, 255, 0)
            );
        }
        DrawString(
            windowX + 180,
            windowY + 255,
            "Å© Å® : ëIë   Space : åàíË",
            GetColor(180, 180, 180)
        );
    }
}

void GameClearScene::Release(void)
{

}
