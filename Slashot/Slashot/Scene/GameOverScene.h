#pragma once
#include<Dxlib.h>
#include"../Scene/SceneBase.h"
#include"../Scene/GameScene.h"
#include<string>
class GameScene;
class GameOverScene :public SceneBase
{
public:
    static constexpr int MENU_CONTINUE = 0;
    static constexpr int MENU_TITLE = 1;
    GameOverScene();
    ~GameOverScene()override;
    void Load(void) override;
    void Init(void) override;
    void Update(void) override;
    void Draw(void) override;
    void Release(void) override;
private:
	GameScene* gameScene_;
    int image_;
    std::string gameOverMessage_;
    int messageTimer_;
    int messageSpeed_;
    int menuSelection_ = 0;

   
};

