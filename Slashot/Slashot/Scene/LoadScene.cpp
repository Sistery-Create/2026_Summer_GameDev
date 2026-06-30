#include "LoadScene.h"
#include "../Manager/SceneManager.h"
#include"../Scene/GameScene.h"
//#include"../Scene/GameClearScene.h"
#include<cmath>
#include<thread>
#include<atomic>
LoadScene::LoadScene()
{
}

LoadScene::~LoadScene()
{
}

void LoadScene::Load(void)
{
    // ロード開始時の初期化
    loadingAngle_ = 0;
    timer_ = 0;
    Init();
    cleared_ = false;   
    loadComplete = false; 

    image_ = LoadGraph("Data/Image/UI/LoadingNow.png");

    // MV1モデルロードを別スレッドで実行
    loadThread_ = std::thread([this]() {
        this->gameScn_ = new GameScene();        // 非同期スレッド内でインスタンス生成
        // ロードが完了したらフラグを立てる
        this->isLoadCompleted_ = true;
        });
    loadThread_.detach(); // スレッドを切り離し、メイン処理とは独立させる
}

void LoadScene::Init(void)
{

}

void LoadScene::Update(void)
{
    // 角度更新
    loadingAngle_ += 10;
    if (loadingAngle_ >= 360) loadingAngle_ -= 360;

    // デルタタイムで経過時間を加算
    float deltaTimeMs = SceneManager::GetInstance().GetDeltaTime() * 1000.0f;
    elapsedTime_ += deltaTimeMs;

  
    //違和感がないようにロード完了しても一定時間ロード画面を流す
    if (isLoadCompleted_ && (elapsedTime_ >= MIN_LOAD_TIME_MS))
    {


        SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::GAME);

    }

}

void LoadScene::Draw(void)
{
	// 画面中央にローディング画像を表示
	DrawGraph(0,0,image_, TRUE);
    const int cx = 960;
    const int cy = 540;

    const int numCircles = 6;  // 円の数
    const int radius = 200;     // 回転の半径
    const int circleSize = 15; // 円の大きさ

    for (int i = 0; i < numCircles; ++i)
    {
        // 各円を少しずつずらす
        float angle = loadingAngle_ + (360.0f / numCircles) * i;
        float rad = angle * DX_PI / 180.0f;

        int x = cx + (int)(radius * cos(rad));
        int y = cy + (int)(radius * sin(rad));

       //カラーを設定
        int r = 255;
        int g = 255;
        int b = 255;
        DrawCircle(x, y, circleSize, GetColor(r, g, b), TRUE);
    }
	// 「Loading...」の文字を表示
	SetFontSize(50);
    DrawString(cx - 100, cy + radius+60, "Loading...", GetColor(255, 255, 255));
}

void LoadScene::Release(void)
{
    if (gameScn_ != nullptr)
    {
        delete gameScn_;
        gameScn_ = nullptr;
    }
}
