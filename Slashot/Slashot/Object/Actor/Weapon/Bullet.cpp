#include "Bullet.h"
#include "../../../Libs/Libs/ImGui/imgui.h"
#include "../Enemey/EnemyBase.h"
#include "../../../Scene/GameScene.h"
#include "../Collider/ColliderCapsule.h"

int Bullet::bulletModelId_ = -1;
bool Bullet::pause_ = false;
Bullet::Bullet(
    VECTOR pos,
    VECTOR dir,
    GameScene* scene)
{
    pos_ = pos;
    dir_ = VNorm(dir);

    speed_ = 2000.0f;
    radius_ = 30.0f;

    lifeTimer_ = 1.0f;

    isDead_ = false;

    gameScene_ = scene;


    modelId_ = MV1DuplicateModel(bulletModelId_);
    debugSpeed_ = speed_;
}

Bullet::~Bullet()
{
    MV1DeleteModel(modelId_);
}

void Bullet::LoadModel()
{
    if (bulletModelId_ == -1)
    {
        bulletModelId_ =
            MV1LoadModel(
                "Data/Model/Player/Weapon/Meshy_AI_Brass_cartridge_stand_0521011858_texture.mv1");
    }
}

void Bullet::ReleaseModel()
{
    if (bulletModelId_ != -1)
    {
        MV1DeleteModel(bulletModelId_);
        bulletModelId_ = -1;
    }
}

void Bullet::Update()
{
    if (isDead_ || pause_) return;

    float delta =
        1.0f / 60.0f;

    // à⁄ìÆ
    pos_ = VAdd(
        pos_,
        VScale(dir_, speed_ * delta)
    );

    // éıñΩ
    lifeTimer_ -= delta;

    if (lifeTimer_ <= 0.0f)
    {
        isDead_ = true;
        return;
    }

    // è∞Ç…ìñÇΩÇ¡ÇΩ
    if (pos_.y <= 0.0f)
    {
        isDead_ = true;
        return;
    }

    // ìGîªíË
    auto& enemies =
        gameScene_->GetEnemies();

    for (EnemyBase* enemy : enemies)
    {
        if (enemy == nullptr) continue;

        ColliderCapsule* col =
            enemy->GetCollider();

        if (col == nullptr) continue;
        VECTOR top =
            col->GetPosTop();

        VECTOR down =
            col->GetPosDown();

        // ÉJÉvÉZÉãê¸ï™
        VECTOR line =
            VSub(down, top);

        // íeÇ÷ÇÃÉxÉNÉgÉã
        VECTOR toBullet =
            VSub(pos_, top);

        // ç≈ãﬂê⁄åWêî
        float t =
            VDot(toBullet, line) /
            VDot(line, line);

        // 0Å`1Ç…êßå¿
        t = std::clamp(t, 0.0f, 1.0f);

        // ç≈ãﬂê⁄ì_
        VECTOR closest =
            VAdd(
                top,
                VScale(line, t)
            );

        // ç≈ãﬂê⁄ì_Ç∆íeãóó£
        float dist =
            VSize(
                VSub(pos_, closest)
            );

        if (dist <= radius_ + col->GetRadius())
        {
            enemy->TakeDamage(power_);

            isDead_ = true;

            return;
        }
    }
}

void Bullet::Draw()
{
    if(isDead_) return;

    MV1SetPosition(
        modelId_,
        pos_);

    float rotY =
        atan2f(
            dir_.x,
            dir_.z);

    MV1SetRotationXYZ(
        modelId_,
        VGet(
            DX_PI_F / 2.0f,
            rotY,
            0.0f));

    MV1SetScale(    
        modelId_,
        scale_);

    MV1DrawModel(modelId_);
}
void Bullet::Debug()
{
    ImGui::Begin("Bullet Debug");

    ImGui::SliderFloat(
        "Speed",
        &debugSpeed_,
        1.0f,
        500.0f);

    speed_ = debugSpeed_;

    ImGui::SliderFloat(
        "Radius",
        &radius_,
        0.1f,
        20.0f);

    ImGui::Text(
        "LifeTimer : %.2f",
        lifeTimer_);

    ImGui::InputFloat3(
        "Position",
        &pos_.x);

    ImGui::End();
}

void Bullet::SetPause(bool flag)
{
    pause_ = flag;
}

bool Bullet::IsDead() const
{
    return isDead_;
}