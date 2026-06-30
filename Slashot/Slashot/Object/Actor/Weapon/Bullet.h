#pragma once
#include <DxLib.h>

class EnemyBase;
class GameScene;

class Bullet
{
public:

    Bullet(
        VECTOR pos,
        VECTOR dir,
        GameScene* scene);
	~Bullet();

    static void LoadModel();
    static void ReleaseModel();


    void Update();
    void Draw();

    void Debug();

   static void SetPause(bool flag);

    bool IsDead() const;

private:

    VECTOR pos_;
    VECTOR dir_;
	VECTOR scale_ = VGet(0.1f, 0.1f, 0.1f);

    float speed_;
    float radius_;

    float lifeTimer_;

    bool isDead_;

	int modelId_;
   static bool pause_;
   static int bulletModelId_;
   float debugSpeed_;
   int power_ = 50;

    GameScene* gameScene_;
};