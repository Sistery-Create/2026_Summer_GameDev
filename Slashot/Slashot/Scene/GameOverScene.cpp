#include "GameOverScene.h"
#include"../Manager/SceneManager.h"
#include"../Manager/SoundManager.h"
#include"../Manager/InputManager.h"
#include"../Application.h"
#include<string>
GameOverScene::GameOverScene()
{
	gameScene_ = nullptr;
}

GameOverScene::~GameOverScene()
{
}

void GameOverScene::Load(void)
{
    image_ = LoadGraph("Data/Image/UI/Black.png");

    menuSelection_ = 0;

    auto type =
        SceneManager::GetInstance().GetGameOverType();

    if (type == SceneManager::GAME_OVER_TYPE::DEATH)
    {
        gameOverMessage_ =
            "ÇµÇÒÇ≈ÇµÇ‹Ç§Ç∆ÇÕ\nÇ»Ç≥ÇØÇ»Ç¢...";
    }
    else
    {
        gameOverMessage_ =
            "ÉmÉãÉ}ñ¢íBê¨\nÉQÅ[ÉÄÉIÅ[ÉoÅ[ÇæÇÊ...";
    }

    messageTimer_ = 0;
    messageSpeed_ = 4;
}

void GameOverScene::Init(void)
{
}

void GameOverScene::Update(void)
{
	messageTimer_++;

	auto& ins = InputManager::GetInstance();

	SoundManager::GetInstance().Play(SoundManager::SRC::GAME_OVER, Sound::TIMES::LOOP);

    if (messageTimer_ / messageSpeed_ >= gameOverMessage_.size())
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

void GameOverScene::Draw(void)
{
    DrawGraph(0, 0, image_, false);

    int windowX = 500;
    int windowY = 480;
    int windowW = 900;
    int windowH = 200;

    // ògê¸ÇÃêFÅiîíÅj
    unsigned int frameColor = GetColor(255, 255, 255);
    // îwåiÇÃêFÅiîZÇ¢ê¬Åj
    unsigned int backgroundColor = GetColor(10, 10, 50);

    // ògê¸ÇÃï`âÊÅiíÜÇìhÇËÇ¬Ç‘Ç≥Ç»Ç¢ DrawBox ÇÃóòópÅj
    DrawBox(windowX - 2, windowY - 2, windowX + windowW + 2, windowY + windowH + 2, frameColor, FALSE);

    // îºìßñæÇÃîwåiÇï`âÊ
    SetDrawBlendMode(DX_BLENDMODE_ALPHA, 200); // îºìßñæÉÇÅ[Éh
    DrawBox(windowX, windowY, windowX + windowW, windowY + windowH, backgroundColor, TRUE);
    SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0); // å≥Ç…ñﬂÇ∑

    SetFontSize(32);

    int charCount = messageTimer_ / messageSpeed_;
    if (charCount > gameOverMessage_.size()) {
        charCount = gameOverMessage_.size();
    }


    int x = windowX + 300;
    int y = windowY + 40;
    int lineHeight = 40;
    int menuY = windowY + 130;

    int continueX = windowX + 220;
    int titleX = windowX + 520;


    std::string currentLine;
    int currentY = y;
    int drawCount = 0;

    for (size_t i = 0; i < gameOverMessage_.length(); ++i)
    {
        if (drawCount >= charCount) {
            break;
        }

        if (gameOverMessage_[i] == '\n') {
            DrawString(x, currentY, currentLine.c_str(), 0xFFFFFF);

            currentLine.clear();
            currentY += lineHeight;
        }
        else {
            currentLine += gameOverMessage_[i];
        }

        drawCount++;
    }

    if (!currentLine.empty()) {
        DrawString(x, currentY, currentLine.c_str(), 0xFFFFFF);
    }


    if (charCount == gameOverMessage_.size()) {
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

void GameOverScene::Release(void)
{
}
