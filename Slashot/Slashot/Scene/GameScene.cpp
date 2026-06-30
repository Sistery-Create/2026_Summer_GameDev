#include "GameScene.h"
#include<cmath>
#include"../Object/Actor/Player/Player.h"
#include"../Object/Actor/Stage/Stage.h"
#include"../Object/Actor/ActorBase.h"
#include"../Object/Actor/Item/Item.h"
#include"../Object/Actor/Stage/Door.h"
#include"../Object/Actor/Enemey/EnemyBase.h"
#include"../Object/Actor/Enemey/EnemyManager.h"
#include"../Object/Actor/Stage/NightDoom.h"
#include"../Object/Actor/Weapon/Bullet.h"
#include"../Manager/Camera.h"
#include"../Manager/InputManager.h"
#include"../Manager/SceneManager.h"
#include"../Manager/SoundManager.h"
#include"../Application.h"
#include "../Libs/Libs/ImGui/imgui.h"
#include <DxLib.h>

GameScene::GameScene()
{
}

GameScene::~GameScene(void)
{
}

void GameScene::Load(void)
{
    // ステージ生成
    stage_ = new Stage();
    stage_->Init();

    const ColliderBase* stageCollider =
        stage_->GetOwnCollider(static_cast<int>(Stage::COLLIDER_TYPE::MODEL));

    // カメラ生成
    camera_ = new Camera();
    camera_->Init();
    OkCollision_ = true;
    clear_ = false;
    camera_->ChangeMode(Camera::MODE::FOLLOW); // 変更

  
    //プレイヤー生成
	player_ = new Player(camera_);
	player_->SetGameScene(this);
	player_->Load();

    enemySpawnPoints_.push_back(VGet(50.0f, 0.0f, 1300.0f));
    enemySpawnPoints_.push_back(VGet(-300.0f, 0.0f, 1200.0f));
    enemySpawnPoints_.push_back(VGet(-550.0f, 0.0f, 600.0f));
    enemySpawnPoints_.push_back(VGet(450.0f, 0.0f, 500.0f));
    enemySpawnPoints_.push_back(VGet(0.0f, 0.0f, 600.0f));

    for (int i = 0; i < 5; i++)
    {
        EnemyBase* enemy = new EnemyBase();

        enemy->SetGameScene(this);
        enemy->Load();
        enemy->AddHitCollider(stageCollider);
    }


    //ナイトドーム生成
    nDoom = new NightDoom();
    nDoom->Init();

    //ステージモデルのコライダーをプレイヤーモデルに登録
   
    player_->AddHitCollider(stageCollider);
    
    camera_->SetFollow(player_);
	camera_->AddHitCollider(stageCollider);

    //アイテム生成
	item_ = new Item();
    //item_->Init();

	//ドア生成
	door_ = new Door();
	door_->Init();
    door_->SetItem(item_);

    Init();

    startTime_ = GetNowCount();

    controlsImage_ =
        LoadGraph(
            "Data/Image/UI/CONTROLS.png"
        );

    isShowControls_ = true;

    controlsStartTime_ = GetNowCount();

}

void GameScene::Init(void)
{
	
    scoreFontHandle_ = CreateFontToHandle(
        "Arial",
        64,
        3
    );
}

void GameScene::Update(void)
{
    SoundManager::GetInstance().Play(SoundManager::SRC::BATTLE, Sound::TIMES::ONCE);
    auto& ins = InputManager::GetInstance();

    if (isShowControls_)
    {
        int elapsed =
            GetNowCount()
            - controlsStartTime_;

        // Enter押下 or 7秒経過
        if (
            ins.IsTrgDown(KEY_INPUT_SPACE) ||
            elapsed >= CONTROLS_SHOW_TIME
            )
        {
            isShowControls_ = false;
        }

        // 操作説明中はゲームを止める
        return;

        
    }

   

    if (!isPaused_)
    {
        timeLimit_ -= SceneManager::GetInstance().GetDeltaTime();
    }
    if (ins.IsTrgDown(KEY_INPUT_TAB))
    {
        SoundManager::GetInstance().Play(SoundManager::SRC::MENU_OPEN, Sound::TIMES::ONCE);

        isPaused_ = !isPaused_;
        if (isPaused_)
        {
            menuSelection_ = 0;
            //SoundManager::GetInstance().PauseBGM();
        }
        else
        {
           
            // SoundManager::GetInstance().ResumeBGM();
        }
    }
    // --- ポーズ中の処理 ---
    if (GetJoypadNum() == 0)
    {
        if (isPaused_)
        {

            // メニュー操作
            // 上キーまたはWキーで選択肢を上に移動
            if (ins.IsTrgDown(KEY_INPUT_UP) || ins.IsTrgDown(KEY_INPUT_W))
            {
                menuSelection_--;
                if (menuSelection_ < 0) menuSelection_ = 1; // 選択肢は2つ (0と1)
				SoundManager::GetInstance().Play(SoundManager::SRC::MENU_CARSOL, Sound::TIMES::ONCE);
            }
            // 下キーまたはSキーで選択肢を下に移動
            else if (ins.IsTrgDown(KEY_INPUT_DOWN) || ins.IsTrgDown(KEY_INPUT_S))
            {
                menuSelection_++;
                if (menuSelection_ > 1) menuSelection_ = 0;
                SoundManager::GetInstance().Play(SoundManager::SRC::MENU_CARSOL, Sound::TIMES::ONCE);
            }

            // 決定 (Spaceキー)
            if (ins.IsPress(KEY_INPUT_SPACE))
            {
                SoundManager::GetInstance().Play(SoundManager::SRC::MENU_CARSOL, Sound::TIMES::ONCE);

                if (menuSelection_ == 0) //続ける
                {
                    isPaused_ = false; // ポーズ解除
                }
                else if (menuSelection_ == 1) //ゲームを終了する
                {
					Application::EndGame();
                    return; // シーン遷移後の処理をスキップ
                }
            }

            // ポーズ中はこれ以上のゲーム更新処理をスキップ
            return;
        }
    }
    else
    {
        if (isPaused_)
        {
            //接続されてるコントローラーを取得
            InputManager::JOYPAD_IN_STATE padstate = (ins.GetJPadInputState(InputManager::JOYPAD_NO::PAD1));

            // メニュー操作
            // 上キーまたはWキーで選択肢を上に移動
            if (ins.IsTrgDown(KEY_INPUT_UP) || ins.IsTrgDown(KEY_INPUT_W))
            {
                menuSelection_--;
                if (menuSelection_ < 0) menuSelection_ = 1; // 選択肢は2つ (0と1)
            }
            // 下キーまたはSキーで選択肢を下に移動
            else if (ins.IsTrgDown(KEY_INPUT_DOWN) || ins.IsTrgDown(KEY_INPUT_S))
            {
                menuSelection_++;
                if (menuSelection_ > 1) menuSelection_ = 0;
            }

            // 決定 (Enterキー)
            if (ins.IsPress(KEY_INPUT_SPACE))
            {
                if (menuSelection_ == 0) // 0: 続ける
                {
                    isPaused_ = false; // ポーズ解除
                }
                else if (menuSelection_ == 1) // 1: ゲームを終了する (タイトル画面へ戻る)
                {
                    SoundManager::GetInstance().AllStop();
                    SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::TITLE);
                    return; // シーン遷移後の処理をスキップ
                }
            }

            // ポーズ中はこれ以上のゲーム更新処理をスキップ
            return;
        }
    }

     stage_->Update();
    camera_->Update();
    nDoom->Update();
	player_->Update();

    for (Bullet* bullet : bullets_)
    {
        if (bullet == nullptr) continue;

        bullet->Update();
    }

    bullets_.erase(
        std::remove_if(
            bullets_.begin(),
            bullets_.end(),
            [](Bullet* bullet)
            {
                if (bullet->IsDead())
                {
                    delete bullet;
                    return true;
                }

                return false;
            }),
        bullets_.end());

    for(auto&enemy : enemys_)
    {
        enemy->Update();
	}
   

    // スポーンタイマー更新
    enemySpawnTimer_ += SceneManager::GetInstance().GetDeltaTime();

    // 一定時間経過
    if (enemySpawnTimer_ >= enemySpawnInterval_)
    {
        enemySpawnTimer_ = 0.0f;

        // 現在数が最大未満
        if (enemys_.size() < maxEnemyNum_)
        {
            // まだ生成可能
            if (spawnedEnemyCount_ < spawnEnemyNum_)
            {
                SpawnEnemy();
            }
        }
    }
    for (auto it = enemys_.begin(); it != enemys_.end();)
    {
        if ((*it)->IsDead())
        {
            (*it)->Release();
            delete (*it);

            it = enemys_.erase(it);
        }
        else
        {
            ++it;
        }
    }

    for (auto it = scorePopups_.begin();
        it != scorePopups_.end();)
    {
        it->timer -=
            SceneManager::GetInstance().GetDeltaTime();

        // 上に浮く
        it->offsetY +=
            80.0f *
            SceneManager::GetInstance().GetDeltaTime();

        // 拡大→縮小
        it->scale -=
            2.0f *
            SceneManager::GetInstance().GetDeltaTime();

        if (it->scale < 1.0f)
        {
            it->scale = 1.0f;
        }

        // フェードアウト
        it->alpha =
            static_cast<int>(
                255 * it->timer
                );

        if (it->timer <= 0.0f)
        {
            it = scorePopups_.erase(it);
        }
        else
        {
            ++it;
        }
    }

    if (drawScore_ < score_)
    {
        int diff = score_ - drawScore_;

        drawScore_ += diff / 10 + 1;
    }

   static bool bulletPause = false;

    if (ins.IsPress(KEY_INPUT_0))
    {
        bulletPause = !bulletPause;

        Bullet::SetPause(bulletPause);
    }

    // プレイヤー死亡
    if (player_->GetDeath())
    {
        SceneManager::GetInstance().SetGameOverType(
            SceneManager::GAME_OVER_TYPE::DEATH
        );
        SoundManager::GetInstance().AllStop();
        SceneManager::GetInstance().ChangeScene(
            SceneManager::SCENE_ID::OVER
        );
        
        return;
    }

    // タイムアップ
    if (timeLimit_ <= 0.0f)
    {
        SoundManager::GetInstance().AllStop();

        if (killEnemyCount_ >= targetKillCount_)
        {
			SceneManager::GetInstance().SetClearScore(score_);
            SceneManager::GetInstance().ChangeScene(
                SceneManager::SCENE_ID::CLEAR
            );
        }
        else
        {
            SceneManager::GetInstance().SetGameOverType(
                SceneManager::GAME_OVER_TYPE::MISSION_FAIL
            );
            SceneManager::GetInstance().ChangeScene(
                SceneManager::SCENE_ID::OVER
            );      
        }

        return;
    }

    if (killEnemyCount_ >= spawnEnemyNum_)
    {
        gameClearCountDown_-=
            SceneManager::GetInstance().GetDeltaTime();
    }

    if (gameClearCountDown_ <= 0.0f)
    {
        SceneManager::GetInstance().SetClearScore(score_);
        SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::CLEAR);
        SoundManager::GetInstance().AllStop();
    }
   
	//UpdateDebug();
}

void GameScene::UpdateDebug()
{
#ifdef _DEBUG

    ImGui::Begin("Enemy Spawn Points");

    // スポーンポイント編集
    for (int i = 0; i < enemySpawnPoints_.size(); i++)
    {
        ImGui::PushID(i);

        ImGui::Text("SpawnPoint %d", i);

        ImGui::InputFloat3(
            "Pos",
            &enemySpawnPoints_[i].x
        );

        // 削除ボタン
        if (ImGui::Button("Delete"))
        {
            enemySpawnPoints_.erase(
                enemySpawnPoints_.begin() + i
            );

            ImGui::PopID();
            break;
        }

        ImGui::Separator();

        ImGui::PopID();
    }

    // 追加ボタン
    if (ImGui::Button("Add SpawnPoint"))
    {
        enemySpawnPoints_.push_back(
            VGet(0.0f, 0.0f, 0.0f)
        );
    }

    ImGui::Separator();

    // スポーン設定
    ImGui::InputFloat(
        "Spawn Interval",
        &enemySpawnInterval_
    );

    ImGui::InputInt(
        "Max Enemy",
        &maxEnemyNum_
    );

    ImGui::InputInt(
        "Total Spawn Enemy",
        &spawnEnemyNum_
    );

    ImGui::End();

#endif
}

void GameScene::Draw(void)
{
    SetFontSize(25);
    // カメラの描画前設定
    camera_->SetBeforeDraw();
    // ステージとプレイヤーを描画
    stage_->Draw();

    player_->Draw();



    for (auto& enemy : enemys_)
    {
        enemy->Draw();
    }

    for (Bullet* bullet : bullets_)
    {
        if (bullet == nullptr) continue;

        bullet->Draw();
    }

    for (const auto& popup : scorePopups_)
    {
        VECTOR screenPos =
            ConvWorldPosToScreenPos(
                VGet(
                    popup.pos.x,
                    popup.pos.y + popup.offsetY,
                    popup.pos.z
                )
            );

        std::string text =
            "+" + std::to_string(popup.score);

        SetDrawBlendMode(
            DX_BLENDMODE_ALPHA,
            popup.alpha
        );

        // 影
        DrawStringToHandle(
            static_cast<int>(screenPos.x) + 2,
            static_cast<int>(screenPos.y) + 2,
            text.c_str(),
            GetColor(0, 0, 0),
            scoreFontHandle_
        );

        // 本体
        DrawStringToHandle(
            static_cast<int>(screenPos.x),
            static_cast<int>(screenPos.y),
            text.c_str(),
            GetColor(255, 220, 0),
            scoreFontHandle_
        );

        SetDrawBlendMode(
            DX_BLENDMODE_NOBLEND,
            0
        );
    }

    int scoreColor =
        (drawScore_ != score_) ?
        GetColor(255, 255, 0) :
        GetColor(255, 220, 255);
    DrawFormatString(
        1600,
        30,
		scoreColor,
        "SCORE : %08d",
        drawScore_
    );
    int minute =
        static_cast<int>(timeLimit_) / 60;

    int second =
        static_cast<int>(timeLimit_) % 60;

    int timeColor = GetColor(255, 255, 255);

    if (timeLimit_ <= 30.0f)
    {
        timeColor = GetColor(255, 50, 50);
    }
    constexpr int SCREEN_W = 1920;

    constexpr int TIME_Y = 30;

    // 文字列の横幅を取得
    int timeWidth = GetDrawFormatStringWidth(
        "TIME %02d:%02d",
        minute,
        second
    );

    // 中央揃え
    int timeX = (SCREEN_W - timeWidth) / 2;
    SetFontSize(50);
    DrawFormatString(
        timeX-50,
        TIME_Y,
        timeColor,
        "TIME %02d:%02d",
        minute,
        second
    );
    SetFontSize(25);
    DrawFormatString(
        INFO_X,
        INFO_Y + 30,
        GetColor(255, 255, 255),
        "MISSION %d / %d",
        killEnemyCount_,
        targetKillCount_
    );
   
    DrawString(INFO_X, INFO_Y - 25,
        "ノルマ：合計80体倒せ!",
        GetColor(255, 200, 0));
    float rate =
        static_cast<float>(killEnemyCount_) /
        static_cast<float>(targetKillCount_);

    rate = std::clamp(rate, 0.0f, 1.0f);
    DrawBox(
        INFO_X,
        INFO_Y + 60,
        INFO_X + 300,
        INFO_Y + 80,
        GetColor(60, 60, 60),
        TRUE
    );
    DrawBox(
        INFO_X,
        INFO_Y + 60,
        INFO_X + static_cast<int>(300 * rate),
        INFO_Y + 80,
        GetColor(255, 180, 0),
        TRUE
    );
    DrawBox(
        INFO_X,
        INFO_Y + 60,
        INFO_X + 300,
        INFO_Y + 80,
        GetColor(255, 255, 255),
        FALSE
    );
    // --- メニュー画面の描画 ---
    if (isPaused_)
    {
        DrawMenu();
    }
    if (isShowControls_)
    {
        // 少し暗くする
        SetDrawBlendMode(
            DX_BLENDMODE_ALPHA,
            180
        );

        DrawBox(
            0,
            0,
            1920,
            1080,
            GetColor(0, 0, 0),
            TRUE
        );

        SetDrawBlendMode(
            DX_BLENDMODE_NOBLEND,
            0
        );
        double scale = 0.8;

        // Enter待ち中に少し拡大縮小
        scale +=
            sin(GetNowCount() * 0.003)
            * 0.01;
        DrawRotaGraph(
            1920 / 2,
            1080 / 2,
            scale,      // 拡大率
            0.0,      // 回転角
            controlsImage_,
            TRUE
                    );
    }
}

void GameScene::Release(void)
{
    // 各オブジェクト解放
    stage_->Release();
    delete stage_;
	// アイテム解放
	item_->Release();
	delete item_;
	//ドア解放
	door_->Release();
	delete door_;

    camera_->Release();
    delete camera_;

    nDoom->Release();
    delete nDoom;

	player_->Release();
	delete player_;

    for (auto& enemy : enemys_)
    {
        enemy->Release();
        delete enemy;
    }
	enemys_.clear();
}


Item* GameScene::GetItem()
{
    return item_;
}

bool GameScene::CheckGameClear()
{
    return clear_;
}

Player* GameScene::GetPlayer()
{
    return player_; 
}

std::vector<EnemyBase*>& GameScene::GetEnemies()
{
    return enemys_;
}

void GameScene::SpawnEnemy()
{
    const ColliderBase* stageCollider =
        stage_->GetOwnCollider(
            static_cast<int>(Stage::COLLIDER_TYPE::MODEL)
        );

    EnemyBase* enemy = new EnemyBase();

    enemy->SetGameScene(this);
    enemy->Load();
    enemy->AddHitCollider(stageCollider);

    // スポーンポイントが無いなら生成しない
    if (enemySpawnPoints_.empty())
    {
        delete enemy;
        return;
    }

    // ランダム選択
    int index = rand() % enemySpawnPoints_.size();

    VECTOR spawnPos = enemySpawnPoints_[index];

    enemy->SetPos(spawnPos);

    enemys_.push_back(enemy);

    spawnedEnemyCount_++;
}

void GameScene::CreateScorePopup(int score, VECTOR pos)
{
    ScorePopup popup;

    popup.score = score;

    popup.pos = pos;

    popup.pos.y += 50.0f;

    popup.timer = 1.0f;

    popup.offsetY = 0.0f;

    popup.scale = 1.8f;

    popup.alpha = 255;

    scorePopups_.push_back(popup);
}

void GameScene::AddScore(int score)
{
	score_ += score;
}

void GameScene::AddBullet(Bullet* bullet)
{
    bullets_.push_back(bullet);
}

void GameScene::AddKillEnemy()
{
    killEnemyCount_++;
}

int GameScene::GetKillEnemyCount() const
{
    return killEnemyCount_;
}
int GameScene::GetScore()
{
    return score_;
}
Camera* GameScene::GetCamera()
{
    return camera_;
}    

void GameScene::EnemyCollision(ActorBase* actor)
{
    //// 座標を取得
    //VECTOR pos = GetEnemy()->GetPos();

    //// カプセルの座標
    //VECTOR capStartPos = VAdd(pos, GetEnemy()->GetStartCapsulePos());
    //VECTOR capEndPos = VAdd(pos, GetEnemy()->GetEndCapsulePos());

    //// カプセルとの当たり判定
    //auto hits = MV1CollCheck_Capsule(
    //    stage_->GetModelId(),   // ステージのモデルID
    //    -1,                     // ステージ全てのポリゴンを指定
    //    capStartPos,            // カプセルの上
    //    capEndPos,              // カプセルの下
    //    GetEnemy()->GetCapsuleRadius() // カプセルの半径
    //);

    //// 衝突したポリゴン全ての検索
    //for (int i = 0; i < hits.HitNum; i++)
    //{
    //    // ポリゴンを1枚に分割
    //    auto hit = hits.Dim[i];

    //    // ポリゴン検索を制限(全てを検索すると重いので)
    //    for (int tryCnt = 0; tryCnt < 10; tryCnt++)
    //    {
    //        // 最初の衝突判定で検出した衝突ポリゴン1枚と衝突判定を取る
    //        int pHit = HitCheck_Capsule_Triangle(
    //            capStartPos,               // カプセルの上
    //            capEndPos,                 // カプセルの下
    //            GetEnemy()->GetCapsuleRadius(), // カプセルの半径
    //            hit.Position[0],           // ポリゴン1
    //            hit.Position[1],           // ポリゴン2
    //            hit.Position[2]            // ポリゴン3
    //        );

    //        // カプセルとポリゴンが当たっていた
    //        if (pHit)
    //        {
    //            // 当たっていたので座標をポリゴンの法線方向に移動させる
    //            pos = VAdd(pos, VScale(hit.Normal, 0.1f));
    //            capStartPos = VAdd(capStartPos, VScale(hit.Normal, 0.1f));
    //            capEndPos = VAdd(capEndPos, VScale(hit.Normal, 0.1f));

    //            // 複数当たっている可能性があるので再検索
    //            continue;
    //        }
    //    }
    //}

    //// 検出したポリゴン情報の後始末
    //MV1CollResultPolyDimTerminate(hits);

    //// 計算した場所にアクターを戻す
    //GetEnemy()->CollisionStage(pos);
}

void GameScene::EnemyStack(ActorBase* actor)
{
    //// Enemy 以外は対象外
    //if (actor->GetType() != ActorBase::ACTOR_TYPE::ENEMY) return;

    //Enemy* enemy = static_cast<Enemy*>(actor);

    //// 座標を取得
    //VECTOR pos = enemy->GetPos();

    //// カプセルの座標
    //VECTOR capStartPos = VAdd(pos, enemy->GetStartCapsulePos());
    //VECTOR capEndPos = VAdd(pos, enemy->GetEndCapsulePos());

    //// カプセルとの当たり判定
    //auto hits = MV1CollCheck_Capsule(
    //    stage_->GetModelId(),   // ステージのモデルID
    //    -1,                     // ステージ全てのポリゴンを指定
    //    capStartPos,            // カプセルの上
    //    capEndPos,              // カプセルの下
    //    enemy->GetCapsuleRadius() // カプセルの半径
    //);

    //// 衝突したポリゴン全ての検索
    //if (hits.HitNum > 0)
    //{
    //    // **壁に当たっている！**
    //    // isEnemyStuck_をtrueにする
    //    isEnemyStuck_ = true;
    //}

    //// 検出したポリゴン情報の後始末
    //MV1CollResultPolyDimTerminate(hits);
}

void GameScene::DrawMenu()
{
    // 画面サイズを取得 (必要に応じて適切な値を使用)
    int screenW, screenH;

    GetDrawScreenSize(&screenW, &screenH);

    // メニュー背景の描画 (半透明の黒)
    // 画面全体を覆い、ゲーム画面が透けて見えるようにする
    SetDrawBlendMode(DX_BLENDMODE_ALPHA, 180); 
    DrawBox(0, 0, screenW, screenH, GetColor(0, 0, 0), TRUE);
    SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0); // ブレンドモードを元に戻す

    // --- タイトルと情報 ---
    int base_y = screenH / 2 - 100;
    int margin_y = 50;
    int color = GetColor(255, 255, 255); // 白
    int select_color = GetColor(255, 255, 0); // 黄色

    // メニュータイトル
    DrawString(screenW / 2 - 80, base_y, "PAUSE MENU", color);

    // --- メニュー選択肢 ---
    base_y += margin_y * 3;

    const char* options[] = {
        "ゲームに戻る",
        "ゲームをやめる",
    };

    for (int i = 0; i < 2; i++)
    {
        int draw_color = (menuSelection_ == i) ? select_color : color;
        DrawString(screenW / 2 - 120, base_y + margin_y * i, options[i], draw_color);
    }

    // 操作説明
    DrawString(screenW / 2 - 700, screenH - 50, "【↑/↓キー】選択 【Spaceキー】決定 【Tabキー】ポーズ解除", color);
}



