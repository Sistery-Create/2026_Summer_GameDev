#include "Player.h"
#include <algorithm>
#include <cmath>
#include "../../../Libs/Libs/ImGui/imgui.h"
#include"../../../Manager/InputManager.h"
#include"../../../Manager/Camera.h"
#include"../../../Utility/AsoUtility.h"
#include"../../../Manager/SoundManager.h"
#include"../CharactorBase.h"
#include"../../../Manager/SceneManager.h"
#include"../../../Object/AnimationController.h"
#include"../../../Application.h"
#include"../../../Scene/GameScene.h"
#include"../../Actor/Enemey/EnemyBase.h"
#include"../../Actor/Weapon/Bullet.h"
#include"../Collider/ColliderBase.h"
#include"../Collider/ColliderLine.h"
#include"../Collider/ColliderCapsule.h"
#include<EffekseerForDXLib.h>
Player::Player(Camera* camera)
    :
    CharactorBase(),
	camera_(camera)
{
    camera_ = camera;
}

Player::~Player()
{
}

void Player::Load()
{
    // リソースロード
    InitLoad();
}

void Player::LoadEnd()
{
    Init();
}

void Player::Init()
{
    // Transform初期化
    InitTransform();
    // アニメーションの初期化
    InitAnimation();
    // 衝突判定初期化
    InitCollider();
    // 初期化後の個別処理
    InitPost();
}

void Player::Update()
{
    InputManager& ins = InputManager::GetInstance();

    animationController_->Update();

    //PlayChargeEffect();

    for (int i = 0; i < static_cast<int>(SKILL_TYPE::MAX); i++)
    {
        if (skillCoolTime_[i] > 0)
        {
            skillCoolTime_[i] -=
                SceneManager::GetInstance().GetDeltaTime();

            if (skillCoolTime_[i] < 0)
            {
                skillCoolTime_[i] = 0;
            }
        }
    }

    if (damageFlashTimer_ > 0.0f)
    {
        damageFlashTimer_ -=
            SceneManager::GetInstance().GetDeltaTime();

        if (damageFlashTimer_ < 0.0f)
        {
            damageFlashTimer_ = 0.0f;
        }
    }
    if (isPowerUp_)
    {
        powerUpTimer_ -= SceneManager::GetInstance().GetDeltaTime();

        if (powerUpTimer_ <= 0.0f)
        {
            powerUpTimer_ = 0.0f;

            playerAttackPower_ /= powerUpAmount_;

            isPowerUp_ = false;
        }
    }
    Death();

    if (isDead_)
    {
        return;
    }
    Move();

    //UpdateDebug();

    ProcessJump();

    CharactorBase::Update();

    transform_.quaRot = Quaternion::Euler(transform_.rot);

    UpdateWeaponOffset();


    PlayerStatement();
    PlayerAttackState();

    if (ins.IsNew(KEY_INPUT_P))
    {
        animationController_->TogglePause();
    }

    UpdateCollision();
  

    if (damegeInterval_ > 0.0f)
    {
        damegeInterval_ -= SceneManager::GetInstance().GetDeltaTime();
    }
    if (mpRecoveryInterval_ > 0.0f)
    {
        mpRecoveryInterval_ -= SceneManager::GetInstance().GetDeltaTime();
    }
    if (bulletDebugTimer_ > 0.0f)
    {
        bulletDebugTimer_ -= SceneManager::GetInstance().GetDeltaTime();
    }

   
	PlayerSkill();

    for (auto it = healPos_.begin();
        it != healPos_.end();)
    {
        it->timer -= SceneManager::GetInstance().GetDeltaTime();

        // 上へ浮かぶ
        it->offsetY += 40.0f *
            SceneManager::GetInstance().GetDeltaTime();

        if (it->timer <= 0.0f)
        {
            it = healPos_.erase(it);
        }
        else
        {
            ++it;
        }
    }

    if (animationController_->IsEnd(static_cast<int>(ANIM_TYPE::ATTACK_SPEL)))
    {
        isChargingAttack_ = false;
    }
}

void Player::Draw()
{

 /*  DrawFormatString(
        0,
        0,
        COLOR_WHITE,
        "State:%d",
        playerState_);
        
    DrawFormatString(
        0,
        20,
        COLOR_WHITE,
        "チャージ中?:%d",
        isCharging_);
    DrawFormatString(
        0,
        40,
        COLOR_WHITE,
        "攻撃力増加量:%.2f",
        chargePower_);*/
  

    DrawExtendGraph(
        1664, 824,
        1920, 1080,
        weaponImg_,
        false
    );
    if (damageFlashTimer_ > 0.0f)
    {
        MV1SetDifColorScale(
            modelId_,
            GetColorF(
                5.0f,
                0.0f,
                0.0f,
                1.0f
            )
        );
    }
    else
    {
        MV1SetDifColorScale(
            modelId_,
            GetColorF(
                1.0f,
                1.0f,
                1.0f,
                1.0f
            )
        );
    }
    if (!isDead_)
    {
        CharactorBase::Draw();
    }
    if (isDebug_)
    {
        for (const auto& capsule : hitCapsules_)
        {
            DrawCapsule3D(
                capsule.start,
                capsule.end,
                capsule.radius,
                16,
                GetColor(255, 0, 0),
                GetColor(255, 0, 0),
                FALSE
            );
        }
    }

    if (isDebug_ && bulletDebugTimer_ > 0.0f)
    {
        DrawLine3D(
            bulletStart_,
            bulletEnd_,
            GetColor(255, 0, 0)
        );
    }

    for (const auto& text : healPos_)
    {
        VECTOR screenPos;

        screenPos =
            ConvWorldPosToScreenPos(
                VGet(
                    text.pos.x,
                    text.pos.y + text.offsetY,
                    text.pos.z
                )
            );

        std::string healStr =
            std::to_string(text.heal);

        // 影
        DrawStringToHandle(
            static_cast<int>(screenPos.x) + 2,
            static_cast<int>(screenPos.y) + 2,
            healStr.c_str(),
            GetColor(0, 0, 0),
            healingHandle_
        );

        // 本体
        DrawStringToHandle(
            static_cast<int>(screenPos.x),
            static_cast<int>(screenPos.y),
            healStr.c_str(),
            COLOR_GREEN,
            healingHandle_
        );
    }
    if (isPowerUp_)
    {
        MV1SetDifColorScale(
            swordModelId_,
            GetColorF(
                3.0f,  // R
                0.0f,  // G
                0.0f,  // B
                1.0f
            )
        );
    }
    else
    {
        MV1SetDifColorScale(
            swordModelId_,
            GetColorF(
                1.0f,
                1.0f,
                1.0f,
                1.0f
            )
        );
    }
    MV1DrawModel(swordModelId_);
    MV1DrawModel(gunRModelId_);

    DrawHPandMP();
    DrawSkill();
    DrawChargeBox();
    if (isDebug_)
    {
        swordCollider_->DrawDebug(COLOR_BLUE);
    }
}

void Player::Release()
{

    MV1DeleteModel(modelId_);
    // アニメーション制御の解放
    if (animationController_ != nullptr)
    {
        animationController_->Release();
        delete animationController_;
        animationController_ = nullptr;
    }
}

void Player::OnGetItem()
{
    if (!hasItem_) {
        hasItem_ = true;
    }
}

void Player::PlayerStatement()
{
    int currentAnim = static_cast<int>(ANIM_TYPE::SWORD_SLASH) + (comboStep_ - 1);

    switch (playerState_) {
    case PlayerState::Idle:
        animationController_->Play(static_cast<int>(ANIM_TYPE::IDLE), true, AnimationController::PLAY_MODE::FULL_BODY);
        if (!AsoUtility::EqualsVZero(moveDir_)) ChangeState(PlayerState::Move);
        if (isJump_)  ChangeState(PlayerState::Jump);
        if (isDraw)   ChangeState(PlayerState::DrawWeapon);
        if (rolling_) ChangeState(PlayerState::Rolling);
        break;

    case PlayerState::Move:
        animationController_->Play(static_cast<int>(ANIM_TYPE::WALK), true, AnimationController::PLAY_MODE::FULL_BODY);
        if (AsoUtility::EqualsVZero(moveDir_)) ChangeState(PlayerState::Idle);
        if (isJump_) ChangeState(PlayerState::Jump);
        if (isDraw)  ChangeState(PlayerState::DrawWeapon);
        if (rolling_) ChangeState(PlayerState::Rolling);
        break;

    case PlayerState::Attack:
        // 攻撃アニメーションが終わったら
        if (attackState_ == PLAYER_ATTACK_STATE::ATTACK_GUN) {
            if (animationController_->IsEnd(static_cast<int>(ANIM_TYPE::GUN_SHOT))) {
                isAttacking_ = false;
                ChangeState(PlayerState::WeaponIdle); // 武器構え待機へ
            }
        }
        else {
            if (animationController_->IsEnd(currentAnim) || animationController_->IsEnd(static_cast<int>(ANIM_TYPE::ATTACK_SPEL))) {
                isAttacking_ = false;
                comboStep_ = 0;
                ChangeState(PlayerState::WeaponIdle);
            }
        }

        if (!isChargingAttack_)
        {
            chargePower_ = DEFAULT_CHARGE_POWER;
        }
        break;

    case PlayerState::Jump:
        animationController_->Play(static_cast<int>(ANIM_TYPE::JUMP), false, AnimationController::PLAY_MODE::FULL_BODY);
        if (isGrounded_) ChangeState(PlayerState::Idle);
        break;

    case PlayerState::DrawWeapon:
        if (attackState_ == PLAYER_ATTACK_STATE::ATTACK_GUN) {
            animationController_->Play(static_cast<int>(ANIM_TYPE::DRAW_GUN), false, AnimationController::PLAY_MODE::UPPER_BODY);
            // アニメーションがまだ終わっていないなら処理を抜ける
            if (!animationController_->IsEnd(static_cast<int>(ANIM_TYPE::DRAW_GUN))) return;
            if (animationController_->IsEnd(static_cast<int>(ANIM_TYPE::DRAW_GUN))) isDrawNow_ = true;
        }
        else if (attackState_ == PLAYER_ATTACK_STATE::ATTACK_SWORD) {
            animationController_->Play(static_cast<int>(ANIM_TYPE::DRAW_SWORD), false, AnimationController::PLAY_MODE::UPPER_BODY);
            // アニメーションがまだ終わっていないなら処理を抜ける
            if (!animationController_->IsEnd(static_cast<int>(ANIM_TYPE::DRAW_SWORD))) return;
        }


        offWeapon = false;
        if (isAttacking_) { ChangeState(PlayerState::Attack); break; }
        if (!AsoUtility::EqualsVZero(moveDir_)) { ChangeState(PlayerState::WeaponWalk); break; }
        if (AsoUtility::EqualsVZero(moveDir_)) { ChangeState(PlayerState::WeaponIdle); break; }
        if (isJump_) { ChangeState(PlayerState::WeaponJump); break; }
        break;

    case PlayerState::OffWeapon:
        if (attackState_ == PLAYER_ATTACK_STATE::ATTACK_GUN) {
            animationController_->Play(static_cast<int>(ANIM_TYPE::OFF_GUN), false, AnimationController::PLAY_MODE::UPPER_BODY);
            if (animationController_->IsEnd(static_cast<int>(ANIM_TYPE::OFF_GUN))) {
                isDraw = false;
                offWeapon = false; // フラグ初期化
                ChangeState(PlayerState::Idle);
            }
        }
        else if (attackState_ == PLAYER_ATTACK_STATE::ATTACK_SWORD) {
            animationController_->Play(static_cast<int>(ANIM_TYPE::OFF_SWORD), false, AnimationController::PLAY_MODE::UPPER_BODY);
            if (animationController_->IsEnd(static_cast<int>(ANIM_TYPE::OFF_SWORD))) {
                isDraw = false;
                offWeapon = false; // フラグ初期化
                ChangeState(PlayerState::Idle);
            }
        }
        break;

    case PlayerState::WeaponIdle:
        if (attackState_ == PLAYER_ATTACK_STATE::ATTACK_SWORD)
        {
            if (!isCharging_)
            {
                animationController_->Play(
                    static_cast<int>(ANIM_TYPE::SWORD_IDLE),
                    true,
                    AnimationController::PLAY_MODE::FULL_BODY,
                    false
                );
            }
        }
        else if (attackState_ == PLAYER_ATTACK_STATE::ATTACK_GUN)
        {
            animationController_->Play(static_cast<int>(ANIM_TYPE::GUN_IDLE), true, AnimationController::PLAY_MODE::FULL_BODY);
        }
        if (isAttacking_) ChangeState(PlayerState::Attack);
        if (!AsoUtility::EqualsVZero(moveDir_)) ChangeState(PlayerState::WeaponWalk);
        if (offWeapon) ChangeState(PlayerState::OffWeapon);
        if (isJump_) ChangeState(PlayerState::WeaponJump);
        break;

    case PlayerState::WeaponWalk:
        if (attackState_ == PLAYER_ATTACK_STATE::ATTACK_SWORD)
        {
            if (!isCharging_)
            {
                animationController_->Play(
                    static_cast<int>(ANIM_TYPE::SWORD_WALK),
                    true,
                    AnimationController::PLAY_MODE::FULL_BODY,
                    false
                );
            }
        }
        else if (attackState_ == PLAYER_ATTACK_STATE::ATTACK_GUN)
        {
            animationController_->Play(static_cast<int>(ANIM_TYPE::GUN_WALK), true, AnimationController::PLAY_MODE::FULL_BODY);
        }
        if (isAttacking_) ChangeState(PlayerState::Attack);
        if (AsoUtility::EqualsVZero(moveDir_)) ChangeState(PlayerState::WeaponIdle);
        if (offWeapon) ChangeState(PlayerState::OffWeapon);
        if (isJump_) ChangeState(PlayerState::WeaponJump);
        break;

    case PlayerState::WeaponJump:
        animationController_->Play(static_cast<int>(ANIM_TYPE::SWORD_JUMP), false, AnimationController::PLAY_MODE::FULL_BODY);
        if (isGrounded_) ChangeState(PlayerState::WeaponIdle);
        break;
    case PlayerState::Death:

        isDontMove_ = true;
        movePow_ = AsoUtility::VECTOR_ZERO;
        if (attackState_ == PLAYER_ATTACK_STATE::ATTACK_SWORD)
        {
            animationController_->Play(static_cast<int>(ANIM_TYPE::SWORD_DEATH), false, AnimationController::PLAY_MODE::FULL_BODY);

        }
        else
        {
            animationController_->Play(static_cast<int>(ANIM_TYPE::DEATH), false, AnimationController::PLAY_MODE::FULL_BODY);

        }
        if (animationController_->IsEnd(static_cast<int>(ANIM_TYPE::DEATH)) || animationController_->IsEnd(static_cast<int>(ANIM_TYPE::SWORD_DEATH)))
        {
            isDead_ = true;
        }
        break;
    case PlayerState::Skill:
        // 回復アニメ終了
        if (currentSkill_ == SKILL_TYPE::HEAL)
        {
            if (animationController_->IsEnd(
                static_cast<int>(ANIM_TYPE::HEAL)))
            {
                if (!healApplied_)
                {
                    playerHP_ += healAmount_;

                    playerHP_ =
                        std::clamp(
                            playerHP_,
                            0.0f,
                            maxPlayerHP_);

                    healApplied_ = true;
                }

                skillCoolTime_[static_cast<int>(currentSkill_)]
                    = skillCoolTimeMax_[static_cast<int>(currentSkill_)];

                isSkillUsing_ = false;

                ChangeState(PlayerState::Idle);
            }
        }
        else if (currentSkill_ == SKILL_TYPE::POWER_UP)
        {
            if (animationController_->IsEnd(
                static_cast<int>(ANIM_TYPE::POWER_UP)))
            {

                skillCoolTime_[static_cast<int>(currentSkill_)]
                    = skillCoolTimeMax_[static_cast<int>(currentSkill_)];

                isSkillUsing_ = false;

                ChangeState(PlayerState::Idle);
            }
        }
        else
        {
            if (animationController_->IsEnd(
                static_cast<int>(ANIM_TYPE::ATTACK_SPEL)))
            {
                skillCoolTime_[static_cast<int>(currentSkill_)]
                    = skillCoolTimeMax_[static_cast<int>(currentSkill_)];
                isSkillUsing_ = false;
                ChangeState(PlayerState::Idle);
            }
        }
        break;
    case PlayerState::Damage:
        if (!animationController_->IsPlaying(
            static_cast<int>(ANIM_TYPE::DAMAGE)))
        {
            animationController_->Play(
                static_cast<int>(ANIM_TYPE::DAMAGE),
                false,
                AnimationController::PLAY_MODE::FULL_BODY
            );
        }
        if (animationController_->IsEnd(static_cast<int>(ANIM_TYPE::DAMAGE)))
        {
            isDamageReaction_ = false;
            isAttacking_ = false;
            canNextCombo_ = false;
            isSkillUsing_ = false;
            comboStep_ = 0;

            if (AsoUtility::EqualsVZero(movePow_))
            {
                ChangeState(
                    isDraw ?
                    PlayerState::WeaponIdle :
                    PlayerState::Idle
                );
            }
            else
            {
                ChangeState(
                    isDraw ?
                    PlayerState::WeaponWalk :
                    PlayerState::Move
                );
            }
        }
        break;
    }
}

void Player::PlayerAttackState()
{
	InputManager& ins = InputManager::GetInstance();

    switch (attackState_) {
    case PLAYER_ATTACK_STATE::NONE:
        if (ins.IsNew(KEY_INPUT_Z))
        {
            if (attackState_ == PLAYER_ATTACK_STATE::ATTACK_SWORD)
            {
                ChangeAttackState(PLAYER_ATTACK_STATE::ATTACK_GUN);
            }
            else
            {
                ChangeAttackState(PLAYER_ATTACK_STATE::ATTACK_SWORD);
            }
        }
        if (ins.IsNew(KEY_INPUT_C))
        {
            if (attackState_ == PLAYER_ATTACK_STATE::ATTACK_GUN)
            {
                ChangeAttackState(PLAYER_ATTACK_STATE::ATTACK_SWORD);
            }
            else
            {
                ChangeAttackState(PLAYER_ATTACK_STATE::ATTACK_GUN);
            }
        }
        break;
    case PLAYER_ATTACK_STATE::ATTACK_SWORD:
        weaponImg_ = swordHandle_;
       
        if (ins.IsNew(KEY_INPUT_Z))
        {
            if (attackState_ == PLAYER_ATTACK_STATE::ATTACK_SWORD)
            {
                ChangeAttackState(PLAYER_ATTACK_STATE::ATTACK_GUN);
            }
            else
            {
                ChangeAttackState(PLAYER_ATTACK_STATE::ATTACK_SWORD);
            }
        }
        if (ins.IsNew(KEY_INPUT_C))
        {
            if (attackState_ == PLAYER_ATTACK_STATE::ATTACK_GUN)
            {
                ChangeAttackState(PLAYER_ATTACK_STATE::ATTACK_SWORD);
            }
            else
            {
                ChangeAttackState(PLAYER_ATTACK_STATE::ATTACK_GUN);
            }
        }
        if (!isCharging_)
        {
            ProcessAttack_Sword();
        }
        ProcessChargeAttack();
        break;
    case PLAYER_ATTACK_STATE::ATTACK_GUN:
        weaponImg_ = gunHandle_;
       
        if (ins.IsNew(KEY_INPUT_Z))
        {
            if (attackState_ == PLAYER_ATTACK_STATE::ATTACK_SWORD)
            {
                ChangeAttackState(PLAYER_ATTACK_STATE::ATTACK_GUN);
            }
            else
            {
                ChangeAttackState(PLAYER_ATTACK_STATE::ATTACK_SWORD);
            }
        }
        if (ins.IsNew(KEY_INPUT_C))
        {
            if (attackState_ == PLAYER_ATTACK_STATE::ATTACK_GUN)
            {
                ChangeAttackState(PLAYER_ATTACK_STATE::ATTACK_SWORD);
            }
            else
            {
                ChangeAttackState(PLAYER_ATTACK_STATE::ATTACK_GUN);
            }
        }
        ProcessAttack_Gun();
        break;
    }
}

void Player::PlayerSkill()
{
    InputManager& ins = InputManager::GetInstance();
    switch (currentSkill_)
    {
    case SKILL_TYPE::HEAL:
        skillImg_ = healHandle_;
        break;

    case SKILL_TYPE::POWER_UP:
        skillImg_ = powerUpHandle_;
        break;

    case SKILL_TYPE::ATTAKC_SPEL:
        skillImg_ = attackSpellHandle_;
        break;
    }
    if (ins.IsNew(KEY_INPUT_Q))
    {
        int skill =
            static_cast<int>(currentSkill_);

        skill++;

        if (skill >= static_cast<int>(SKILL_TYPE::MAX))
        {
            skill = 0;
        }

        currentSkill_ =
            static_cast<SKILL_TYPE>(skill);
    }

	//武器を構えていないときのみスキル使用可能
    if (ins.IsNew(KEY_INPUT_R))
    {
        if (currentSkill_ == SKILL_TYPE::ATTAKC_SPEL)
        {
            UseSkill();
        }
        else if (!isDraw)
        {
            UseSkill();
        }
    }
}

const char* Player::GetSkillName() const
{
    switch (currentSkill_)
    {
    case SKILL_TYPE::HEAL:
        return "HEAL:60";
		
    case SKILL_TYPE::POWER_UP:
        return "POWER_UP:100";
     
    case SKILL_TYPE::ATTAKC_SPEL:
        return "SPECIAL_MOVE:700";
    
    }

    return "NONE";
}

void Player::UseSkill()
{
    int skill =
        static_cast<int>(currentSkill_);

    if (skillCoolTime_[skill] > 0)
    {
        return;
    }
    if (isSkillUsing_)
    {
        return;
    }

    switch (currentSkill_)
    {
    case SKILL_TYPE::HEAL:
        Healing();
        break;
	case SKILL_TYPE::POWER_UP:
        PowerUp();
		break;
    case SKILL_TYPE::ATTAKC_SPEL:
        AttackSpell();
        break;
    }
}

int Player::GetWeapon()
{
    if (isAttacking_&&attackState_==PLAYER_ATTACK_STATE::ATTACK_SWORD)
    {
        return swordModelId_;
    }
    else if(isAttacking_ && attackState_ == PLAYER_ATTACK_STATE::ATTACK_GUN)
    {
        return gunRModelId_;
    }
    return -1;
}

void Player::CreateHitCapsule(int startFrame, int endFrame, float radius)
{
    HitCapsule capsule;

    capsule.start =
        MV1GetFramePosition(modelId_, startFrame);

    capsule.end =
        MV1GetFramePosition(modelId_, endFrame);

    capsule.radius = radius;

    hitCapsules_.push_back(capsule);
}

void Player::UpdateCollision()
{

    if (isSkillUsing_)
    {
		return;
    }
	int currentAnim = animationController_->GetPlayType();

    if (currentAnim ==
        static_cast<int>(ANIM_TYPE::SWORD_ATTACK2))
    {
        auto& enemies = gameScene_->GetEnemies();

        VECTOR kickStart =
            MV1GetFramePosition(
                modelId_,
                rightUpLegFrame_);

        VECTOR kickEnd =
            MV1GetFramePosition(
                modelId_,
                rightLegFrame_);

        float kickRadius = 8.0f;
        for (EnemyBase* enemy : enemies)
        {
            if (enemy == nullptr) continue;

            ColliderCapsule* enemyCol =
                enemy->GetCollider();

            if (enemyCol == nullptr) continue;

            VECTOR enemyTop =
                enemyCol->GetPosTop();

            VECTOR enemyDown =
                enemyCol->GetPosDown();

            VECTOR enemyCenter =
                VScale(
                    VAdd(enemyTop, enemyDown),
                    0.5f);

            VECTOR kickCenter =
                VScale(
                    VAdd(kickStart, kickEnd),
                    0.5f);

            float dist =
                VSize(
                    VSub(kickCenter, enemyCenter));

            float range =
                kickRadius +
                enemyCol->GetRadius();

            if (dist <= range)
            {
                enemy->TakeDamage(playerAttackPower_*2);
               
            }
        }
    }

    //剣と敵の当たり判定
    if (gameScene_ != nullptr && isAttacking_)
    {
        if (attackState_ == PLAYER_ATTACK_STATE::ATTACK_SWORD)
        {
            auto& enemies = gameScene_->GetEnemies();

            for (EnemyBase* enemy : enemies)
            {
                if (enemy == nullptr) continue;

                ColliderCapsule* enemyCol =
                    enemy->GetCollider();

                if (enemyCol == nullptr) continue;

                VECTOR swordTop =
                    swordCollider_->GetPosTop();

                VECTOR swordDown =
                    swordCollider_->GetPosDown();

                VECTOR enemyTop =
                    enemyCol->GetPosTop();

                VECTOR enemyDown =
                    enemyCol->GetPosDown();

                VECTOR swordCenter =
                    VScale(VAdd(swordTop, swordDown), 0.5f);

                VECTOR enemyCenter =
                    VScale(VAdd(enemyTop, enemyDown), 0.5f);

                float dist =
                    VSize(VSub(swordCenter, enemyCenter));

                float range =
                    swordCollider_->GetRadius() +
                    enemyCol->GetRadius();

                if (dist <= range)
                {
                    enemy->TakeDamage(playerAttackPower_* chargePower_);

                    VECTOR hitPos =
                    {
                        (swordCenter.x + enemyCenter.x) * 0.5f,
                        (swordCenter.y + enemyCenter.y) * 0.5f,
                        (swordCenter.z + enemyCenter.z) * 0.5f
                    };

					PlaySlashEffect();

                    // インターバル終了中だけ回復
                    if (mpRecoveryInterval_ <= 0.0f)
                    {
                        playerMP_ += swordHitRecoveryMP_;

                        playerMP_ =
                            std::clamp(
                                playerMP_,
                                0.0f,
                                maxPlayerMP_);

                        // インターバル開始
                        mpRecoveryInterval_ =
                            MP_RECOVERY_INTERVAL_MAX;
                    }
                }

                index++;
            }
        }
    }
    hitCapsules_.clear();

    CreateHitCapsule(
        spineFrame_,
        spine2Frame_,
        5.0f
    );

    CreateHitCapsule(
        rightArmFrame_,
        rightForeArmFrame_,
        3.0f
    );

    CreateHitCapsule(
        leftArmFrame_,
        leftForeArmFrame_,
        3.0f
    );

    CreateHitCapsule(
        rightUpLegFrame_,
        rightLegFrame_,
        4.0f
    );

    CreateHitCapsule(
        leftUpLegFrame_,
        leftLegFrame_,
        4.0f
    );
}

void Player::Death()
{
    if (!isDead_)
    {
		if(playerHP_ <= 0)
		{
			ChangeState(PlayerState::Death);
		}
	}
}

void Player::DrawHPandMP()
{
    const int BAR_X = 40;
    const int BAR_Y = 40;

    const int BAR_WIDTH = 400;
    const int BAR_HEIGHT = 30;

    // 最大HPに対する割合
    float hpRate = playerHP_ / maxPlayerHP_;

    // 0～1に制限
    hpRate = std::clamp(hpRate, 0.0f, 1.0f);

    // 色変更
    unsigned int hpColor;

    if (hpRate > 0.6f)
    {
        // 緑
        hpColor = GetColor(0, 255, 0);
    }
    else if (hpRate > 0.3f)
    {
        // 黄
        hpColor = GetColor(255, 255, 0);
    }
    else
    {
        // 赤
        hpColor = GetColor(255, 0, 0);
    }

    // 背景
    DrawBox(
        BAR_X,
        BAR_Y,
        BAR_X + BAR_WIDTH,
        BAR_Y + BAR_HEIGHT,
        GetColor(60, 60, 60),
        TRUE
    );

    // HP本体
    DrawBox(
        BAR_X,
        BAR_Y,
        BAR_X + static_cast<int>(BAR_WIDTH * hpRate),
        BAR_Y + BAR_HEIGHT,
        hpColor,
        TRUE
    );

    // 枠
    DrawBox(
        BAR_X,
        BAR_Y,
        BAR_X + BAR_WIDTH,
        BAR_Y + BAR_HEIGHT,
        GetColor(255, 255, 255),
        FALSE
    );

    // 数値表示
    DrawFormatString(
        BAR_X + 10,
        BAR_Y + 6,
        GetColor(255, 255, 255),
        "HP %.0f / %.0f",
        playerHP_,
        maxPlayerHP_
    );

    const int MP_BAR_Y = 80;

    float mpRate =
        playerMP_ / maxPlayerMP_;

    mpRate =
        std::clamp(
            mpRate,
            0.0f,
            1.0f);

    DrawBox(
        BAR_X,
        MP_BAR_Y,
        BAR_X + BAR_WIDTH,
        MP_BAR_Y + BAR_HEIGHT,
        GetColor(60, 60, 60),
        TRUE
    );

    DrawBox(
        BAR_X,
        MP_BAR_Y,
        BAR_X + static_cast<int>(BAR_WIDTH * mpRate),
        MP_BAR_Y + BAR_HEIGHT,
        GetColor(0, 150, 255),
        TRUE
    );

    DrawBox(
        BAR_X,
        MP_BAR_Y,
        BAR_X + BAR_WIDTH,
        MP_BAR_Y + BAR_HEIGHT,
        GetColor(255, 255, 255),
        FALSE
    );

    DrawFormatString(
        BAR_X + 10,
        MP_BAR_Y + 6,
        GetColor(255, 255, 255),
        "MP %.0f / %.0f",
        playerMP_,
        maxPlayerMP_
    );
}

void Player::DrawSkill()
{


    int skill =
        static_cast<int>(currentSkill_);

    if (skillCoolTime_[skill] > 0.0f)
    {
        SetDrawBright(
            100,
            100,
            100
        );
    }
    DrawString(
        SKILL_X - 80,
        SKILL_Y + 60,
        GetSkillName(),
        COLOR_WHITE
    );
    DrawExtendGraph(
        SKILL_X - SKILL_ICON_SIZE / 2,
        SKILL_Y - SKILL_ICON_SIZE / 2,
        SKILL_X + SKILL_ICON_SIZE / 2,
        SKILL_Y + SKILL_ICON_SIZE / 2,
        skillImg_,
        TRUE
    );
    float rate =
        skillCoolTime_[static_cast<int>(currentSkill_)]
        / skillCoolTimeMax_[static_cast<int>(currentSkill_)];
    int coverHeight =
        static_cast<int>(
            SKILL_ICON_SIZE * rate);

    DrawBox(
        SKILL_X - SKILL_ICON_SIZE / 2,
        SKILL_Y - SKILL_ICON_SIZE / 2,
        SKILL_X + SKILL_ICON_SIZE / 2,
        SKILL_Y - SKILL_ICON_SIZE / 2 + coverHeight,
        GetColor(0, 0, 0),
        TRUE
    );
    if (skillCoolTime_[skill] > 0.0f)
    {
        SetFontSize(40);

        DrawFormatString(
            SKILL_X - 12,
            SKILL_Y - 6,
            COLOR_WHITE,
            "%.0f",
            ceilf(
                skillCoolTime_[skill]
            )
        );
    }
    SetDrawBright(
        255,
        255,
        255
    );
    SetFontSize(25);
}

void Player::HealingPop()
{
    for (const auto& popup : healPos_)
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
            "+" + std::to_string(popup.heal);

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
}

void Player::PlayHealEffect()
{
    if (!isSkillUsing_)
    {
        return;
    }
    VECTOR start =
        MV1GetFramePosition(
            transform_.modelId,
            skillFrame_);

    VECTOR dir =
    {
        -sinf(transform_.rot.y),
        0.0f,
        -cosf(transform_.rot.y)
    };

    // ヒーリング
    int playHandle =
        PlayEffekseer3DEffect(
            healingEffect_);

    SetPosPlayingEffekseer3DEffect(
        playHandle,
        start.x,
        start.y,
        start.z
    );
    SetScalePlayingEffekseer3DEffect(
        playHandle,
        10.0f,
        10.0f,
        10.0f);

    if (animationController_->
        IsEnd(static_cast<int>(ANIM_TYPE::HEAL)))
    {
        StopEffekseer2DEffect(playHandle);
    }
}

void Player::PlayPowerUpEffect()
{
    if (!isPowerUp_)
    {
        return;
    }
    VECTOR start =
        MV1GetFramePosition(
            transform_.modelId,
            skillFrame_);
    VECTOR dir =
    {
        -sinf(transform_.rot.y),
        0.0f,
        -cosf(transform_.rot.y)
    };
    // パワーアップ
    int playHandle =
        PlayEffekseer3DEffect(
            powerUpEffect_);
    SetPosPlayingEffekseer3DEffect(
        playHandle,
        start.x,
        start.y,
        start.z
    );
    SetScalePlayingEffekseer3DEffect(
        playHandle,
        10.0f,
        10.0f,
		10.0f);
}

void Player::PlayAttackSpellEffect()
{

    VECTOR start =
        MV1GetFramePosition(
            transform_.modelId,
            skillFrame_);
    VECTOR dir =
    {
        -sinf(transform_.rot.y),
        0.0f,
        -cosf(transform_.rot.y)
    };
    // ヘビーアタック
    int playHandle =
        PlayEffekseer3DEffect(
            attackSpellEffect_);
    SetPosPlayingEffekseer3DEffect(
        playHandle,
        start.x,
        start.y,
        start.z
    );
    SetScalePlayingEffekseer3DEffect(
        playHandle,
        100.0f,
        100.0f,
        100.0f);
}



void Player::Healing()
{
    if (playerMP_ < healUseMP_)
    {
        return;
    }
    SoundManager::GetInstance().Play(SoundManager::SRC::HEALING, Sound::TIMES::ONCE);
    playerMP_ -= healUseMP_;

    isSkillUsing_ = true;

    healApplied_ = false;

    ChangeState(PlayerState::Skill);

    animationController_->Play(
        static_cast<int>(ANIM_TYPE::HEAL),
        false,
        AnimationController::PLAY_MODE::FULL_BODY
    );

    PlayHealEffect();

    // 回復文字生成
    Heal text;

    text.heal = healAmount_;

    text.pos = transform_.pos;

    text.pos.y += 35.0f;

    text.timer = 1.0f;

    text.offsetY = 0.0f;

    healPos_.push_back(text);

}

void Player::PowerUp()
{
    if (playerMP_ < powerUpUseMp_)
    {
        return;
    }

    if (isPowerUp_)
    {
        return;
    }

    playerMP_ -= powerUpUseMp_;

    isPowerUp_ = true;
    isSkillUsing_ = true;

    ChangeState(PlayerState::Skill);
    SoundManager::GetInstance().Play(SoundManager::SRC::POWER_UP, Sound::TIMES::ONCE);
    animationController_->Play(
        static_cast<int>(ANIM_TYPE::POWER_UP),
        false,
        AnimationController::PLAY_MODE::FULL_BODY
    );

    PlayPowerUpEffect();
    powerUpTimer_ = POWER_UP_TIME;

    playerAttackPower_ *= powerUpAmount_;

    skillCoolTime_[static_cast<int>(currentSkill_)]
        = skillCoolTimeMax_[static_cast<int>(currentSkill_)];

}

void Player::AttackSpell()
{
    if (playerMP_ < attackSpellUseMP_)
    {
        return;
    }

    playerMP_ -= attackSpellUseMP_;
    isSkillUsing_ = true;

    auto& enemies = gameScene_->GetEnemies();

    ChangeState(PlayerState::Skill);
    SoundManager::GetInstance().Play(SoundManager::SRC::SPEL_ATTACK, Sound::TIMES::ONCE);
    animationController_->Play(
        static_cast<int>(ANIM_TYPE::ATTACK_SPEL),
        false,
        AnimationController::PLAY_MODE::FULL_BODY
    );

    PlayAttackSpellEffect();

    for (auto enemy : enemies)
    {
        if (enemy == nullptr)
        {
            continue;
        }

        enemy->TakeDamage(
            playerAttackPower_ * 20.0f
        );
    }
}

void Player::ProcessChargeAttack()
{
    auto& ins = InputManager::GetInstance();

    // 攻撃中なら無視
    if (isAttacking_)
    {
        return;
    }

    // 武器を構えていないなら無視
    if (!isDraw)
    {
        return;
    }

    // 押した瞬間
    if (ins.IsTrgMouseRight())
    {
        isCharging_ = true;

        chargeTime_ = 0.0f;

        //printfDx("Charge Start\n");
        animationController_->Play(
            static_cast<int>(ANIM_TYPE::SWORD_CHARGE),
            true,
            AnimationController::PLAY_MODE::FULL_BODY,
            true
        );
       
    }

    // 押している間
    if (isCharging_ &&
        ins.IsClickMouseRight())
    {
        chargeTime_ +=
            SceneManager::GetInstance().GetDeltaTime();

        if (chargeTime_ > MAX_CHARGE_TIME)
        {
            chargeTime_ = MAX_CHARGE_TIME;
        }
    }

    // 離した瞬間
    if (isCharging_ &&
        ins.IsTrgUpMouseRight())
    {
        isCharging_ = false;

        chargePower_ =
            1.0f +
            chargeTime_ / MAX_CHARGE_TIME * 4.0f;

        ChargeSlash();

        chargeTime_ = 0.0f;
    }
}

void Player::ChargeSlash()
{
    isAttacking_ = true;
    isChargingAttack_ = true;

    ChangeState(PlayerState::Attack);

    animationController_->Play(
        static_cast<int>(ANIM_TYPE::ATTACK_SPEL),
        false,
        AnimationController::PLAY_MODE::FULL_BODY,
        true
    );

   

    SoundManager::GetInstance().Play(
        SoundManager::SRC::SWORD_ATTACK,
        Sound::TIMES::ONCE);

    isCharging_ = false;
}

void Player::DrawChargeBox()
{
    if (!isCharging_)
    {
        return;
    }

    const int x = 1750;
    const int y = 500;

    const int width = 40;
    const int height = 200;

    float chargeRate =
        chargeTime_ / MAX_CHARGE_TIME;

    if (chargeRate > 1.0f)
    {
        chargeRate = 1.0f;
    }

    int percent =
        static_cast<int>(chargeRate * 100.0f);

    // 外枠
    DrawBox(
        x - 2,
        y - 2,
        x + width + 2,
        y + height + 2,
        COLOR_WHITE,
        FALSE
    );

    // 背景
    DrawBox(
        x,
        y,
        x + width,
        y + height,
        GetColor(30, 30, 30),
        TRUE
    );

    // ゲージ（下→上）
    int fillHeight =
        static_cast<int>(
            height * chargeRate);

    int color = COLOR_YELLOW;

    if (chargeRate >= 1.0f)
    {
        SetDrawBright(255, 255, 100);

        color = COLOR_RED;
        DrawString(
            x,
            y - 30,

            "MAX",

            GetColor(255, 255, 0)
        );


        SetDrawBright(255, 255, 255);
    }
    else
    {
        DrawFormatString(
            x,
            y - 30,

            COLOR_WHITE,

            "%d%%",

            percent
        );
    }

    DrawBox(
        x,
        y + height - fillHeight,

        x + width,
        y + height,

       color,
        TRUE
    );

    DrawString(
        x - 20,
        y + height + 15,

        "CHARGE",

        COLOR_WHITE
    );

   

    
}

void Player::PlaySlashEffect()
{
    if (slashEffect_ == -1) return;
    if (tipFrame == -1) return;

    VECTOR tipPos;

    tipPos=MV1GetFramePosition(
        swordModelId_,
        tipFrame);

    int effectHandle =
        PlayEffekseer3DEffect(slashEffect_);

    SetPosPlayingEffekseer3DEffect(
        effectHandle,
        tipPos.x,
        tipPos.y,
        tipPos.z);

    // 剣の向きに合わせる
    MATRIX frameMat =
        MV1GetFrameLocalWorldMatrix(
            swordModelId_,
            tipFrame);

    VECTOR dir =
    {
        frameMat.m[2][0],
        frameMat.m[2][1],
        frameMat.m[2][2]
    };

    SetRotationPlayingEffekseer3DEffect(
        effectHandle,
        0.0f,
        atan2f(dir.x, dir.z),
        0.0f);
    SetScalePlayingEffekseer3DEffect(
        effectHandle,
        5.0f,
        5.0f,
		5.0f);
}

ColliderCapsule* Player::GetSwordCollider()
{
   return swordCollider_;
}

void Player::SetGameScene(GameScene* gameScene)
{
    gameScene_ = gameScene;
}

VECTOR Player::GetPos()
{
    return transform_.pos;
}

VECTOR Player::GetRot()
{
    return angle_;
}

bool Player::GetClear()
{
    return drawNow;
}

void Player::ChangeState(PlayerState newState)
{
    if (playerState_ == PlayerState::Damage &&
        newState != PlayerState::Damage)
    {
        isDamageReaction_ = false;
    }

	playerState_ = newState;
}

void Player::ChangeAttackState(PLAYER_ATTACK_STATE newState)
{
	attackState_ = newState;
}

void Player::SetPos(VECTOR pos)
{
    transform_.pos = pos;
}

const std::vector<Player::HitCapsule>& Player::GetHitCapsules() const
{
	return hitCapsules_;
}

void Player::TakeDamage(float damage)
{
    if (damegeInterval_ > 0.0f||isSkillUsing_)
    {
        return;
    }
    if (playerState_ == PlayerState::Damage)
    {
        playerHP_ -= damage;
        return;
    }

    playerHP_ -= damage;

    damegeInterval_ = DAMAGE_INTERVAL_MAX;

    // 光らせる
    damageFlashTimer_ = DAMAGE_FLASH_TIME;

    // 累計ダメージ加算
    accumulatedDamage_ += damage;
    if (!isChargingAttack_ || !isCharging_)
    {
        // 30ダメージ食らったらリアクション
        if (accumulatedDamage_ >= DAMAGE_REACTION_BORDER)
        {

            accumulatedDamage_ -= DAMAGE_REACTION_BORDER;

            isDamageReaction_ = true;

            isAttacking_ = false;
            canNextCombo_ = false;
            chargePower_ = DEFAULT_CHARGE_POWER;
            comboStep_ = 0;

            ChangeState(PlayerState::Damage);



        }
    }
    else//チャージ攻撃中のみ条件緩和
    {
        if (accumulatedDamage_ >= DAMAGE_REACTION_BORDER*3)
        {

            accumulatedDamage_ -= DAMAGE_REACTION_BORDER;

            isDamageReaction_ = true;

            isAttacking_ = false;
            canNextCombo_ = false;
            chargePower_ = DEFAULT_CHARGE_POWER;
            comboStep_ = 0;

            ChangeState(PlayerState::Damage);



        }
    }
}

void Player::ShotBullet()
{
    if (gameScene_ == nullptr)
    {
        return;
    }

    VECTOR start =
        MV1GetFramePosition(
            gunRModelId_,
            muzzleFrame_);

    VECTOR dir =
    {
        -sinf(transform_.rot.y),
        0.0f,
        -cosf(transform_.rot.y)
    };

    Bullet* bullet =
        new Bullet(
            start,
            dir,
            gameScene_);

    gameScene_->AddBullet(bullet);

    // マズルフラッシュ
    int playHandle =
        PlayEffekseer3DEffect(
            muzzleEffect_);

    SetPosPlayingEffekseer3DEffect(
        playHandle,
        start.x,
        start.y,
        start.z
    );
    SetScalePlayingEffekseer3DEffect(
        playHandle,
        3.0f,
        3.0f,
        3.0f);

}
 
bool Player::GetDeath()
{
    return isDead_;
}

void Player::InitLoad(void)
{
    modelId_ = MV1LoadModel("Data/Model/Player/Player.mv1");
    swordModelId_ = MV1LoadModel("Data/Model/Player/Weapon/BlenderTest.mv1");
    gunRModelId_ = MV1LoadModel("Data/Model/Player/Weapon/GunsR_.mv1");
   

    gunLModelId_ = MV1LoadModel("Data/Model/Player/Meshy_AI_Guns_L_0421033300_texture.mv1");

    handR_ = MV1SearchFrame(modelId_,"mixamorig:RightHandIndex1");
	handL_ = MV1SearchFrame(modelId_, "mixamorig:LeftHandIndex1");
	back_ = MV1SearchFrame(modelId_, "mixamorig:Spine2");
	hips_ = MV1SearchFrame(modelId_, "mixamorig:Hips");


	swordHandle_ = LoadGraph("Data/Image/Item/Sword_A.jpg");
	gunHandle_ = LoadGraph("Data/Image/Item/Guns_R.jpg");
	healHandle_ = LoadGraph("Data/Image/Skill/Heal.png");
    powerUpHandle_ = LoadGraph("Data/Image/Skill/PowerUp.png");
	attackSpellHandle_ = LoadGraph("Data/Image/Skill/FireWall.png");

	muzzleEffect_ = LoadEffekseerEffect("Data/Effect/Weapon/MuzzleFlash.efkefc");
	slashEffect_ = LoadEffekseerEffect("Data/Effect/Weapon/Slash.efkefc");
	healingEffect_ = LoadEffekseerEffect("Data/Effect/Skill/Healing.efkefc");
	powerUpEffect_ = LoadEffekseerEffect("Data/Effect/Skill/PowerUp.efkefc");
	attackSpellEffect_ = LoadEffekseerEffect("Data/Effect/Skill/AttackMagic.efkefc");
    LoadEnd();
}

void Player::InitTransform(void)
{
    transform_.SetModel(modelId_);
    transform_.pos = INITPOS;
    pos_ = transform_.pos;
    transform_.localPos = VGet(0, 3.0f, 0);

	transform_.rot = VGet(0.0f, AsoUtility::Deg2RadF(180.0f), 0.0f);
    angle_ = transform_.rot;

    InitWeapon();

    baseMoveSpeed_ = MOVE_SPEED;
    moveSpeed_ = baseMoveSpeed_;
    mouseSensitivity_ = MOUSE_SENSITIVITY;
    transform_.scl = VGet(0.6f, 0.6f, 0.6f);
	

    startCapsulePos_ = { 0.0f,5.0f,0.0f };
    endCapsulePos_ = { 0.0f,-3.0f,0.0f };
    capsuleRadius_ = 1.4f;

    isCollision_ = true;
    isGrounded_ = true;

    maxStamina_ = 150.0f;
    currentStamina_ = maxStamina_;
    staminaConsumptionRate_ = 45.0f; // 1秒あたり45消費
    staminaRecoveryRate_ = 10.0f;    // 1秒あたり10回復
    dashSpeedMultiplier_ = 1.75f;     // 速度1.75倍
    isDashing_ = false;


    staminaCoolDownTimer_ = 0.0f;
    announce = false;
    
	debugCounter_ = 0.0f;

    blendTime_ = 10.0f;
    stepBlend_ = 0.0f;

    // ダメージ間隔の初期化
    damegeInterval_ = 0.0f;
  
    healingHandle_ = CreateFontToHandle(
        "Arial",
        50,
        3
    );
}

void Player::InitAnimation(void)
{
    // アニメーション制御の初期化

    animationController_ = new AnimationController(modelId_);

    // アニメーションの追加
    animationController_->Add(static_cast<int>(ANIM_TYPE::IDLE), ANIMATION_SPEED, "Data/Model/Player/Idle.mv1");
    animationController_->Add(static_cast<int>(ANIM_TYPE::RUN), ANIMATION_SPEED, "Data/Model/Player/Standard Run.mv1");
    animationController_->Add(static_cast<int>(ANIM_TYPE::WALK), ANIMATION_SPEED, "Data/Model/Player/Walk.mv1");
    animationController_->Add(static_cast<int>(ANIM_TYPE::DAMAGE), ANIMATION_SPEED*3 , "Data/Model/Player/Damaged.mv1");
    animationController_->Add(static_cast<int>(ANIM_TYPE::JUMP), JUMP_ANIMATION_SPEED, "Data/Model/Player/Jump.mv1");
    animationController_->Add(static_cast<int>(ANIM_TYPE::SWORD_WALK), ANIMATION_SPEED, "Data/Model/Player/Sword Walk.mv1");
    animationController_->Add(static_cast<int>(ANIM_TYPE::SWORD_IDLE), ANIMATION_SPEED, "Data/Model/Player/Sword Idle.mv1");
    animationController_->Add(static_cast<int>(ANIM_TYPE::SWORD_JUMP), ANIMATION_SPEED, "Data/Model/Player/Sword Jump.mv1");
	animationController_->Add(static_cast<int>(ANIM_TYPE::DRAW_SWORD), ANIMATION_SPEED*1.5, "Data/Model/Player/Draw Sword.mv1");
    animationController_->Add(static_cast<int>(ANIM_TYPE::OFF_SWORD), ANIMATION_SPEED * 1.5, "Data/Model/Player/Off Sword.mv1");
    animationController_->Add(static_cast<int>(ANIM_TYPE::DRAW_GUN), ANIMATION_SPEED*2, "Data/Model/Player/Drawing Gun.mv1");
	animationController_->Add(static_cast<int>(ANIM_TYPE::GUN_IDLE), ANIMATION_SPEED, "Data/Model/Player/Gun Idle.mv1");
    animationController_->Add(static_cast<int>(ANIM_TYPE::GUN_SHOT), ANIMATION_SPEED*5, "Data/Model/Player/Shot.mv1");
	animationController_->Add(static_cast<int>(ANIM_TYPE::GUN_WALK), ANIMATION_SPEED, "Data/Model/Player/Gun Walk.mv1");
    animationController_->Add(static_cast<int>(ANIM_TYPE::OFF_GUN), ANIMATION_SPEED * 2, "Data/Model/Player/off Gun.mv1");
    animationController_->Add(static_cast<int>(ANIM_TYPE::SWORD_SLASH), ANIMATION_ATTACK_SPEED*2.0, "Data/Model/Player/Sword Attack1.mv1");
    animationController_->Add(static_cast<int>(ANIM_TYPE::SWORD_ATTACK), ANIMATION_ATTACK_SPEED*1.8, "Data/Model/Player/Sword Slash.mv1");
	animationController_->Add(static_cast<int>(ANIM_TYPE::SWORD_ATTACK2), ANIMATION_ATTACK_SPEED*1.5, "Data/Model/Player/Sword Kick.mv1");
    animationController_->Add(static_cast<int>(ANIM_TYPE::ROLLING), ANIMATION_SPEED*4, "Data/Model/Player/Rolling flont.mv1");
	animationController_->Add(static_cast<int>(ANIM_TYPE::ROLLING_INPLACE), ANIMATION_SPEED*4, "Data/Model/Player/Rolling InPlace.mv1");
    animationController_->Add(static_cast<int>(ANIM_TYPE::DEATH), ANIMATION_SPEED*2, "Data/Model/Player/Death.mv1");
    animationController_->Add(static_cast<int>(ANIM_TYPE::SWORD_DEATH), ANIMATION_SPEED*2, "Data/Model/Player/Sword Death.mv1");
    animationController_->Add(static_cast<int>(ANIM_TYPE::HEAL), ANIMATION_SPEED * 2, "Data/Model/Player/Skill/Magic Heal.mv1");
    animationController_->Add(static_cast<int>(ANIM_TYPE::POWER_UP), ANIMATION_SPEED * 2, "Data/Model/Player/Skill/PowerUp.mv1");
    animationController_->Add(static_cast<int>(ANIM_TYPE::ATTACK_SPEL), ANIMATION_SPEED * 4, "Data/Model/Player/Skill/AttackCasting.mv1");
    animationController_->Add(static_cast<int>(ANIM_TYPE::SWORD_CHARGE), ANIMATION_SPEED * 2, "Data/Model/Player/Sword Charge.mv1");
    animationController_->Play(static_cast<int>(ANIM_TYPE::IDLE), true, AnimationController::PLAY_MODE::FULL_BODY);
}

void Player::InitPost(void)
{

}

void Player::InitCollider(void)
{
    // 主に地面との衝突で仕様する線分コライダ
    ColliderLine* colLine = new ColliderLine(
        ColliderBase::TAG::PLAYER, &transform_,
        COL_LINE_START_LOCAL_POS, COL_LINE_END_LOCAL_POS);
    ownColliders_.emplace(static_cast<int>(COLLIDER_TYPE::LINE), colLine);

    // 主に壁や木などの衝突で仕様するカプセルコライダ
    ColliderCapsule* colCapsule = new ColliderCapsule(
        ColliderBase::TAG::PLAYER, &transform_,
        COL_CAPSULE_TOP_LOCAL_POS, COL_CAPSULE_DOWN_LOCAL_POS,
        COL_CAPSULE_RADIUS);
    ownColliders_.emplace(static_cast<int>(COLLIDER_TYPE::CAPSULE), colCapsule);

    //Sword to Enemy
    swordCollider_ = new ColliderCapsule(
        ColliderBase::TAG::PLAYER,
        &swordTransform_,
        VGet(0.0f, 15.0f, 0.0f),  
        VGet(0.0f, -15.0f, 0.0f), 
        18.0f
    );


}

void Player::Move()
{
    
    if (isAttacking_||isSkillUsing_||isDamageReaction_||isCharging_)
    {
        movePow_ = AsoUtility::VECTOR_ZERO;
        return;
    }

    if (rolling_ || isDontMove_)
    {
        return;
    }
    auto& ins = InputManager::GetInstance();
    // カメラ角度を取得
    VECTOR cameraAngles = camera_->GetCameraAngle();

    // 移動量
    const float MOVE_POW = 5.0f;
	const float SWORD_CAST_SPEED = 4.0;
     moveDir_ = AsoUtility::VECTOR_ZERO;

    // ゲームパッドが接続数で処理を分ける
    if (GetJoypadNum() == 0)
    {
        // WASDで移動する
        if (ins.IsPress(KEY_INPUT_W)) { moveDir_ = { 0.0f, 0.0f, 1.0f }; }
        if (ins.IsPress(KEY_INPUT_A)) { moveDir_ = { -1.0f, 0.0f, 0.0f }; }
        if (ins.IsPress(KEY_INPUT_S)) { moveDir_ = { 0.0f, 0.0f, -1.0f }; }
        if (ins.IsPress(KEY_INPUT_D)) { moveDir_ = { 1.0f, 0.0f, 0.0f }; }
    }
    else
    {
        // 接続されているゲームパッド１の情報を取得
        InputManager::JOYPAD_IN_STATE padState =
            ins.GetJPadInputState(InputManager::JOYPAD_NO::PAD1);

        // アナログキーの入力値から方向を取得
        moveDir_ = ins.GetDirectionXZAKey(padState.AKeyLX, padState.AKeyLY);

    }

    if (!AsoUtility::EqualsVZero(moveDir_))
    {
        // 入力値の補間
        const float SMOOTH = 0.25f;
        moveDir_.x = preInputDir_.x + (moveDir_.x - preInputDir_.x) * SMOOTH;
        moveDir_.z = preInputDir_.z + (moveDir_.z - preInputDir_.z) * SMOOTH;
        preInputDir_ = moveDir_;

        // 正規化
        moveDir_ = VNorm(moveDir_);

        // XYZの回転行列
        MATRIX mat = MGetIdent();
        mat = MMult(mat, MGetRotY(cameraAngles.y));

        // 回転行列を使用して、ベクトルを回転させる
        moveDir_ = VTransform(moveDir_, mat);

        transform_.rot.y = atan2f(moveDir_.x, moveDir_.z) + DX_PI_F;

        if (!isWalkingSE_)
        {
            SoundManager::GetInstance().Play(
                SoundManager::SRC::WALK,
                Sound::TIMES::LOOP);

            isWalkingSE_ = true;
        }

        if (isDraw)
        {
			movePow_ = VScale(moveDir_, SWORD_CAST_SPEED);
        }
        else
        {
            // 方向×スピードで移動量を作って、座標に足して移動
            movePow_ = VScale(moveDir_, MOVE_POW);
        }
    }
    else
    {
        if (isWalkingSE_)
        {
            SoundManager::GetInstance().Stop(
                SoundManager::SRC::WALK);

            isWalkingSE_ = false;
        }

        movePow_ = AsoUtility::VECTOR_ZERO;

    }
    

}

void Player::UpdateWeaponOffset()
{
    //武器の状態ごとのオフセット管理
    //剣
    VECTOR swordCenter = MV1GetFramePosition(swordModelId_, 0);

    if (isDraw && attackState_ == PLAYER_ATTACK_STATE::ATTACK_SWORD) {
        attachFrame = handR_;
        //攻撃中?
        if (!isAttacking_)
        {
            swordPos_ = MV1GetFramePosition(modelId_, attachFrame);
            MATRIX mat = MV1GetFrameLocalWorldMatrix(modelId_, attachFrame);

            MATRIX offsetPos = MGetTranslate(swordOffsetHandPos_);
            MATRIX rotX = MGetRotX(swordOffsetHandRot_.x);
            MATRIX rotY = MGetRotY(swordOffsetHandRot_.y);
            MATRIX rotZ = MGetRotZ(swordOffsetHandRot_.z);

            MATRIX offsetRot = MMult(rotX, rotY);
            offsetRot = MMult(offsetRot, rotZ);
            MATRIX scaleMat = MGetScale(swordScale_);

            mat = MMult(offsetPos, mat);
            mat = MMult(offsetRot, mat);
            mat = MMult(scaleMat, mat);

            MV1SetMatrix(swordModelId_, mat);

        }
        else
        {
            swordPos_ = MV1GetFramePosition(modelId_, attachFrame);
            MATRIX mat = MV1GetFrameLocalWorldMatrix(modelId_, attachFrame);

            MATRIX offsetPos = MGetTranslate(swordOffsetAttackPos_);
            MATRIX rotX = MGetRotX(swordOffsetAttackRot_.x);
            MATRIX rotY = MGetRotY(swordOffsetAttackRot_.y);
            MATRIX rotZ = MGetRotZ(swordOffsetAttackRot_.z);

            MATRIX offsetRot = MMult(rotX, rotY);
            offsetRot = MMult(offsetRot, rotZ);
            MATRIX scaleMat = MGetScale(swordScale_);

            mat = MMult(offsetPos, mat);
            mat = MMult(offsetRot, mat);
            mat = MMult(scaleMat, mat);

            MV1SetMatrix(swordModelId_, mat);

            swordTransform_.pos = swordPos_;

            swordTransform_.rot = transform_.rot;

            swordTransform_.quaRot =
                Quaternion::Euler(swordTransform_.rot);
        }
    }
    else {
        attachFrame = back_;
        swordPos_ = MV1GetFramePosition(modelId_, attachFrame);
        MATRIX mat = MV1GetFrameLocalWorldMatrix(modelId_, attachFrame);

        MATRIX offsetPos = MGetTranslate(swordOffsetBackPos_);
        MATRIX rotX = MGetRotX(swordOffsetBackRot_.x);
        MATRIX rotY = MGetRotY(swordOffsetBackRot_.y);
        MATRIX rotZ = MGetRotZ(swordOffsetBackRot_.z);

        MATRIX offsetRot = MMult(rotX, rotY);
        offsetRot = MMult(offsetRot, rotZ);

        MATRIX scaleMat = MGetScale(swordScale_);

        mat = MMult(offsetPos, mat);
        mat = MMult(offsetRot, mat);
        mat = MMult(scaleMat, mat);

        MV1SetMatrix(swordModelId_, mat);
    }

    //銃
    if (isDraw && attackState_ == PLAYER_ATTACK_STATE::ATTACK_GUN) {

        attachFrame = handR_;



        if (!isAttacking_)
        {
            if (AsoUtility::EqualsVZero(movePow_) && !isDrawNow_)
            {
                gunRPos_ = MV1GetFramePosition(modelId_, attachFrame);
                MATRIX mat = MV1GetFrameLocalWorldMatrix(modelId_, attachFrame);

                MATRIX offsetPos = MGetTranslate(gunROffsetHandPos_);
                MATRIX rotX = MGetRotX(gunROffsetHandRot_.x);
                MATRIX rotY = MGetRotY(gunROffsetHandRot_.y);
                MATRIX rotZ = MGetRotZ(gunROffsetHandRot_.z);

                MATRIX offsetRot = MMult(rotX, rotY);
                offsetRot = MMult(offsetRot, rotZ);

                MATRIX scaleMat = MGetScale(gunRScale_);

                mat = MMult(offsetPos, mat);
                mat = MMult(offsetRot, mat);
                mat = MMult(scaleMat, mat);

                MV1SetMatrix(gunRModelId_, mat);
            }
            else
            {
                gunRPos_ = MV1GetFramePosition(modelId_, attachFrame);
                MATRIX mat = MV1GetFrameLocalWorldMatrix(modelId_, attachFrame);

                MATRIX offsetPos = MGetTranslate(gunROffsetMovePos_);
                MATRIX rotX = MGetRotX(gunROffsetMoveRot_.x);
                MATRIX rotY = MGetRotY(gunROffsetMoveRot_.y);
                MATRIX rotZ = MGetRotZ(gunROffsetMoveRot_.z);

                MATRIX offsetRot = MMult(rotX, rotY);
                offsetRot = MMult(offsetRot, rotZ);

                MATRIX scaleMat = MGetScale(gunRScale_);

                mat = MMult(offsetPos, mat);
                mat = MMult(offsetRot, mat);
                mat = MMult(scaleMat, mat);

                MV1SetMatrix(gunRModelId_, mat);
            }
        }
        else
        {
            gunRPos_ = MV1GetFramePosition(modelId_, attachFrame);
            MATRIX mat = MV1GetFrameLocalWorldMatrix(modelId_, attachFrame);

            MATRIX offsetPos = MGetTranslate(gunROffsetAttackPos_);
            MATRIX rotX = MGetRotX(gunROffsetAttackRot_.x);
            MATRIX rotY = MGetRotY(gunROffsetAttackRot_.y);
            MATRIX rotZ = MGetRotZ(gunROffsetAttackRot_.z);

            MATRIX offsetRot = MMult(rotX, rotY);
            offsetRot = MMult(offsetRot, rotZ);

            MATRIX scaleMat = MGetScale(gunRScale_);

            mat = MMult(offsetPos, mat);
            mat = MMult(offsetRot, mat);
            mat = MMult(scaleMat, mat);

            MV1SetMatrix(gunRModelId_, mat);
        }
    }
    else {
        attachFrame = hips_;
        gunRPos_ = MV1GetFramePosition(modelId_, attachFrame);
        MATRIX mat = MV1GetFrameLocalWorldMatrix(modelId_, attachFrame);

        MATRIX offsetPos = MGetTranslate(gunROffsetBackPos_);
        MATRIX rotX = MGetRotX(gunROffsetBackRot_.x);
        MATRIX rotY = MGetRotY(gunROffsetBackRot_.y);
        MATRIX rotZ = MGetRotZ(gunROffsetBackRot_.z);

        MATRIX offsetRot = MMult(rotX, rotY);
        offsetRot = MMult(offsetRot, rotZ);
        MATRIX scaleMat = MGetScale(gunRScale_);

        mat = MMult(offsetPos, mat);
        mat = MMult(offsetRot, mat);
        mat = MMult(scaleMat, mat);

        MV1SetMatrix(gunRModelId_, mat);
    }



    VECTOR start =
        MV1GetFramePosition(swordModelId_, rootFrame);

    VECTOR end =
        MV1GetFramePosition(swordModelId_, tipFrame);

    swordCollider_->SetWorldPos(
        start,
        end
    );


}

void Player::DrawSword()
{
    auto& ins = InputManager::GetInstance();
    if (ins.IsNew(KEY_INPUT_F)) {
        if (!isDraw) {
            isDraw = true;
            offWeapon = false;
            SoundManager::GetInstance().Play(SoundManager::SRC::DRAW_SWORD, Sound::TIMES::ONCE, false);
        }
        else {
            offWeapon = true;
            SoundManager::GetInstance().Play(SoundManager::SRC::OFSWORD, Sound::TIMES::ONCE, false);
        }
    }
}

void Player::DrawGun()
{
    auto& ins = InputManager::GetInstance();
    if (ins.IsNew(KEY_INPUT_F)) {
        if (!isDraw) {
            isDraw = true;
            offWeapon = false;
            SoundManager::GetInstance().Play(SoundManager::SRC::DRAW_GUN, Sound::TIMES::ONCE, false);
        }
        else {
            offWeapon = true;
            SoundManager::GetInstance().Play(SoundManager::SRC::OFFGUN, Sound::TIMES::ONCE, false);
        }
    }
}

void Player::ProcessJump(void)
{
    if(isAttacking_)
    {
        return;
	}
    auto& ins = InputManager::GetInstance();
    bool isHitKey = ins.IsTrgDown(KEY_INPUT_SPACE)
        || ins.IsPadBtnTrgDown(
            InputManager::JOYPAD_NO::PAD1,
            InputManager::JOYPAD_BTN::DOWN);

    // 持続ジャンプ処理
    bool isHitKeyNew = ins.IsPress(KEY_INPUT_SPACE)
        || ins.IsPadBtnNew(
            InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::DOWN);
    if (isHitKeyNew)
    {
        // ジャンプの入力受付時間を減少
        stepJump_ += scnMng_.GetDeltaTime();
        if (stepJump_ < TIME_JUMP_INIT)
        {
            // ジャンプ量の計算
            float jumpSpeed = POW_JUMP_KEEP * scnMng_.GetDeltaTime();
            jumpPow_ = VAdd(jumpPow_, VScale(AsoUtility::DIR_U, jumpSpeed));
        }
    }
    else
    {
        // ボタンを離したらジャンプ力に加算しない
        stepJump_ = TIME_JUMP_INIT;
    }

    // ジャンプ
    if (isHitKey && !isJump_)
    {
        // ジャンプ量の計算
        float jumpSpeed = POW_JUMP_INIT * scnMng_.GetDeltaTime();
        jumpPow_ = VScale(AsoUtility::DIR_U, jumpSpeed);
        isJump_ = true;
        // アニメーション再生
        /*if (!isDrawSword &&!isAttacking_)
        {
            if (!drawNow && !offNow)
            {
                animationController_->Play(static_cast<int>(ANIM_TYPE::JUMP),false);
            }
        }
        else
        {
            if (!drawNow && !offNow)
            {
                animationController_->Play(static_cast<int>(ANIM_TYPE::SWORD_JUMP), false);
            }
        }*/
    }
    if (jumpPow_.y == AsoUtility::VECTOR_ZERO.y)
    {
        isGrounded_ = true;
    }
    else
    {
		isGrounded_ = false;
    }
}

void Player::ProcessAttack_Sword()
{
    auto& ins = InputManager::GetInstance();
    DrawSword();
    if (!isDraw) return;

	
    if (ins.IsTrgMouseLeft())
    {
        if (!isAttacking_)
        {
            isAttacking_ = true;
            comboStep_ = 1;
            ChangeState(PlayerState::Attack);
            movePow_ = AsoUtility::VECTOR_ZERO; // 攻撃中は移動しない
            int firstAnim = static_cast<int>(ANIM_TYPE::SWORD_SLASH);
            animationController_->Play(firstAnim, false, AnimationController::PLAY_MODE::FULL_BODY, true);
            SoundManager::GetInstance().Play(SoundManager::SRC::SWORD_ATTACK, Sound::TIMES::ONCE);
        }
        else if (canNextCombo_)
        {
            if (comboStep_ < 3)
            {
                comboStep_++;
                canNextCombo_ = false;

              
                int nextAnim = static_cast<int>(ANIM_TYPE::SWORD_SLASH) + (comboStep_ - 1);
                animationController_->Play(nextAnim, false, AnimationController::PLAY_MODE::FULL_BODY, true);
               
                isAttacking_ = true;
            }
            if (animationController_->IsPlaying(static_cast<int>(ANIM_TYPE::SWORD_ATTACK)))
            {
                SoundManager::GetInstance().Play(SoundManager::SRC::SWORD_ATTACK2, Sound::TIMES::ONCE);
            }
            else if(animationController_->IsPlaying(static_cast<int>(ANIM_TYPE::SWORD_ATTACK2)))
            {
                
                SoundManager::GetInstance().Play(SoundManager::SRC::KICK, Sound::TIMES::ONCE);
            }
        }
    }

   
    if (isAttacking_)
    {
        float progress = animationController_->GetProgress();
        canNextCombo_ = (progress > 0.3f && progress < 0.95f);
    }
}

void Player::ProcessAttack_Gun()
{
    auto& ins = InputManager::GetInstance();

    DrawGun();

    if (!isDraw) return;

    if (ins.IsTrgMouseLeft())
    {
        // MP不足
        if (playerMP_ < gunUseMP_)
        {
            return;
        }

        if (!isAttacking_)
        {
            // MP消費
            playerMP_ -= gunUseMP_;

            isAttacking_ = true;

            ChangeState(PlayerState::Attack);

            movePow_ = AsoUtility::VECTOR_ZERO;

            SoundManager::GetInstance().Play(SoundManager::SRC::GUN_SHOT, Sound::TIMES::ONCE);

            animationController_->Play(
                static_cast<int>(ANIM_TYPE::GUN_SHOT),
                false,
                AnimationController::PLAY_MODE::FULL_BODY,
                false
            );

            ShotBullet();
        }
    }
}

void Player::UpdateProcces(void)
{
	//Move();
}

void Player::UpdateProccesPost(void)
{
}

void Player::InitWeapon(void)
{
    // 武器の位置と回転のオフセット
    //剣
    swordOffsetBackPos_ = DEFAULT_SWORD_BACK_POS;
    swordOffsetBackRot_ = DEFAULT_SWORD_BACK_ROT;

    swordOffsetHandPos_ = DEFAULT_SWORD_HAND_POS;
    swordOffsetHandRot_ = DEFAULT_SWORD_HAND_ROT;

    swordOffsetAttackPos_ = DEFAULT_SWORD_ATTACK_POS;
    swordOffsetAttackRot_ = DEFAULT_SWORD_ATTACK_ROT;
    // 銃
    gunROffsetBackPos_ = DEFAULT_GUN_BACK_POS;
    gunROffsetBackRot_ = DEFAULT_GUN_BACK_ROT;

    gunROffsetHandPos_ = DEFAULT_GUN_HAND_POS;
    gunROffsetHandRot_ = DEFAULT_GUN_HAND_ROT;

    gunROffsetAttackPos_ = DEFAULT_GUN_ATTACK_POS;
    gunROffsetAttackRot_ = DEFAULT_GUN_ATTACK_ROT;

    gunROffsetMovePos_ = DEFAULT_GUN_MOVE_POS;
    gunROffsetMoveRot_ = DEFAULT_GUN_MOVE_ROT;

}

void Player::UpdateDebug()
{
    //// ウィンドウタイトル&開始処理

    ImGui::Begin("Player");
    // 大きさ
    ImGui::Text("scale");
    ImGui::InputFloat("SclX", &transform_.scl.x);
    ImGui::InputFloat("SclY", &transform_.scl.y);
    ImGui::InputFloat("SclZ", &transform_.scl.z);
    // 角度
    VECTOR rotDeg = VECTOR();
    rotDeg.x = AsoUtility::Rad2DegF(transform_.rot.x);
    rotDeg.y = AsoUtility::Rad2DegF(transform_.rot.y);
    rotDeg.z = AsoUtility::Rad2DegF(transform_.rot.z);
    ImGui::Text("angle(deg)");
    ImGui::SliderFloat("RotX", &rotDeg.x, 0.0f, 360.0f);
    ImGui::SliderFloat("RotY", &rotDeg.y, 0.0f, 360.0f);
    ImGui::SliderFloat("RotZ", &rotDeg.z, 0.0f, 360.0f);
    transform_.rot.x = AsoUtility::Deg2RadF(rotDeg.x);
    transform_.rot.y = AsoUtility::Deg2RadF(rotDeg.y);
    transform_.rot.z = AsoUtility::Deg2RadF(rotDeg.z);
    // 位置
    ImGui::Text("position");
    // 構造体の先頭ポインタを渡し、xyzと連続したメモリ配置へアクセス
    ImGui::InputFloat3("Pos", &transform_.pos.x);
    // 終了処理
    ImGui::Separator();
    ImGui::End();

    ////剣のオフセット
    //ImGui::Text("Sword Offset (Hand)");

    //// 位置
    //ImGui::InputFloat3("Hand Pos##Sword", &swordOffsetHandPos_.x);

    //// 回転（度で表示）
    //VECTOR handRotDeg;
    //handRotDeg.x = AsoUtility::Rad2DegF(swordOffsetHandRot_.x);
    //handRotDeg.y = AsoUtility::Rad2DegF(swordOffsetHandRot_.y);
    //handRotDeg.z = AsoUtility::Rad2DegF(swordOffsetHandRot_.z);

    //ImGui::SliderFloat("Hand RotX##Sword", &handRotDeg.x, -180.0f, 180.0f);
    //ImGui::SliderFloat("Hand RotY##Sword", &handRotDeg.y, -180.0f, 180.0f);
    //ImGui::SliderFloat("Hand RotZ##Sword", &handRotDeg.z, -180.0f, 180.0f);

    //// ラジアンに戻す
    //swordOffsetHandRot_.x = AsoUtility::Deg2RadF(handRotDeg.x);
    //swordOffsetHandRot_.y = AsoUtility::Deg2RadF(handRotDeg.y);
    //swordOffsetHandRot_.z = AsoUtility::Deg2RadF(handRotDeg.z);

    //ImGui::Separator();
    //ImGui::Text("Sword Offset (Back)");

    //// 背中用
    //ImGui::InputFloat3("Back Pos##Sword", &swordOffsetBackPos_.x);

    //VECTOR backRotDeg;
    //backRotDeg.x = AsoUtility::Rad2DegF(swordOffsetBackRot_.x);
    //backRotDeg.y = AsoUtility::Rad2DegF(swordOffsetBackRot_.y);
    //backRotDeg.z = AsoUtility::Rad2DegF(swordOffsetBackRot_.z);

    //ImGui::SliderFloat("Back RotX##Sword", &backRotDeg.x, -180.0f, 180.0f);
    //ImGui::SliderFloat("Back RotY##Sword", &backRotDeg.y, -180.0f, 180.0f);
    //ImGui::SliderFloat("Back RotZ##Sword", &backRotDeg.z, -180.0f, 180.0f);

    //swordOffsetBackRot_.x = AsoUtility::Deg2RadF(backRotDeg.x);
    //swordOffsetBackRot_.y = AsoUtility::Deg2RadF(backRotDeg.y);
    //swordOffsetBackRot_.z = AsoUtility::Deg2RadF(backRotDeg.z);

    //ImGui::Text("scale");
    //ImGui::InputFloat("SclX##Sword", &swordScale_.x);
    //ImGui::InputFloat("SclY##Sword", &swordScale_.y);
    //ImGui::InputFloat("SclZ##Sword", &swordScale_.z);

    //// 攻撃中
    //ImGui::Text("Attacking");
    //ImGui::InputFloat3("Attack Pos##Sword", &swordOffsetAttackPos_.x);

    //VECTOR attackRotDeg;
    //attackRotDeg.x = AsoUtility::Rad2DegF(swordOffsetAttackRot_.x);
    //attackRotDeg.y = AsoUtility::Rad2DegF(swordOffsetAttackRot_.y);
    //attackRotDeg.z = AsoUtility::Rad2DegF(swordOffsetAttackRot_.z);

    //ImGui::SliderFloat("Attack RotX##Sword", &attackRotDeg.x, -180.0f, 180.0f);
    //ImGui::SliderFloat("Attack RotY##Sword", &attackRotDeg.y, -180.0f, 180.0f);
    //ImGui::SliderFloat("Attack RotZ##Sword", &attackRotDeg.z, -180.0f, 180.0f);

    //swordOffsetAttackRot_.x = AsoUtility::Deg2RadF(attackRotDeg.x);
    //swordOffsetAttackRot_.y = AsoUtility::Deg2RadF(attackRotDeg.y);
    //swordOffsetAttackRot_.z = AsoUtility::Deg2RadF(attackRotDeg.z);

    //ImGui::Text("scale");
    //ImGui::InputFloat("SclX##Sword", &swordScale_.x);
    //ImGui::InputFloat("SclY##Sword", &swordScale_.y);
    //ImGui::InputFloat("SclZ##Sword", &swordScale_.z);

    ////銃のオフセット
    //ImGui::Text("Gun Offset (Hand)");

    //// 位置
    //ImGui::InputFloat3("Hand Pos##Gun", &gunROffsetHandPos_.x);

    //// 回転（度で表示）
    //VECTOR guns_hand_RotDeg;
    //guns_hand_RotDeg.x = AsoUtility::Rad2DegF(gunROffsetHandRot_.x);
    //guns_hand_RotDeg.y = AsoUtility::Rad2DegF(gunROffsetHandRot_.y);
    //guns_hand_RotDeg.z = AsoUtility::Rad2DegF(gunROffsetHandRot_.z);

    //ImGui::SliderFloat("Hand RotX##Gun", &guns_hand_RotDeg.x, -180.0f, 180.0f);
    //ImGui::SliderFloat("Hand RotY##Gun", &guns_hand_RotDeg.y, -180.0f, 180.0f);
    //ImGui::SliderFloat("Hand RotZ##Gun", &guns_hand_RotDeg.z, -180.0f, 180.0f);

    //// ラジアンに戻す
    //gunROffsetHandRot_.x = AsoUtility::Deg2RadF(guns_hand_RotDeg.x);
    //gunROffsetHandRot_.y = AsoUtility::Deg2RadF(guns_hand_RotDeg.y);
    //gunROffsetHandRot_.z = AsoUtility::Deg2RadF(guns_hand_RotDeg.z);

    //ImGui::Separator();
    //ImGui::Text("Gun Offset (hips_)");

    //// 背中用
    //ImGui::InputFloat3("Back Pos##Gun", &gunROffsetBackPos_.x);

    //VECTOR guns_backRotDeg;
    //guns_backRotDeg.x = AsoUtility::Rad2DegF(gunROffsetBackRot_.x);
    //guns_backRotDeg.y = AsoUtility::Rad2DegF(gunROffsetBackRot_.y);
    //guns_backRotDeg.z = AsoUtility::Rad2DegF(gunROffsetBackRot_.z);

    //ImGui::SliderFloat("Back RotX##Gun", &guns_backRotDeg.x, -180.0f, 180.0f);
    //ImGui::SliderFloat("Back RotY##Gun", &guns_backRotDeg.y, -180.0f, 180.0f);
    //ImGui::SliderFloat("Back RotZ##Gun", &guns_backRotDeg.z, -180.0f, 180.0f);

    //gunROffsetBackRot_.x = AsoUtility::Deg2RadF(guns_backRotDeg.x);
    //gunROffsetBackRot_.y = AsoUtility::Deg2RadF(guns_backRotDeg.y);
    //gunROffsetBackRot_.z = AsoUtility::Deg2RadF(guns_backRotDeg.z);

    //ImGui::Text("scale");
    //ImGui::InputFloat("SclX##Gun", &gunRScale_.x);
    //ImGui::InputFloat("SclY##Gun", &gunRScale_.y);
    //ImGui::InputFloat("SclZ##Gun", &gunRScale_.z);

    //// 背中用
    ImGui::Text("MoveNow");
    ImGui::InputFloat3("Move Pos##Gun", &gunROffsetMovePos_.x);

    VECTOR guns_moveRotDeg;
    guns_moveRotDeg.x = AsoUtility::Rad2DegF(gunROffsetMoveRot_.x);
    guns_moveRotDeg.y = AsoUtility::Rad2DegF(gunROffsetMoveRot_.y);
    guns_moveRotDeg.z = AsoUtility::Rad2DegF(gunROffsetMoveRot_.z);

    ImGui::SliderFloat("Move RotX##Gun", &guns_moveRotDeg.x, -180.0f, 180.0f);
    ImGui::SliderFloat("Move RotY##Gun", &guns_moveRotDeg.y, -180.0f, 180.0f);
    ImGui::SliderFloat("Move RotZ##Gun", &guns_moveRotDeg.z, -180.0f, 180.0f);
    gunROffsetMoveRot_.x = AsoUtility::Deg2RadF(guns_moveRotDeg.x);
    gunROffsetMoveRot_.y = AsoUtility::Deg2RadF(guns_moveRotDeg.y);
    gunROffsetMoveRot_.z = AsoUtility::Deg2RadF(guns_moveRotDeg.z);

;   // 背中用
    ImGui::Text("AttackNow");
    ImGui::InputFloat3("Attack Pos##Gun", &gunROffsetAttackPos_.x);

    VECTOR guns_attackRotDeg;
    guns_attackRotDeg.x = AsoUtility::Rad2DegF(gunROffsetAttackRot_.x);
    guns_attackRotDeg.y = AsoUtility::Rad2DegF(gunROffsetAttackRot_.y);
    guns_attackRotDeg.z = AsoUtility::Rad2DegF(gunROffsetAttackRot_.z);

    ImGui::SliderFloat("Attack RotX##Gun", &guns_attackRotDeg.x, -180.0f, 180.0f);
    ImGui::SliderFloat("Attack RotY##Gun", &guns_attackRotDeg.y, -180.0f, 180.0f);
    ImGui::SliderFloat("Attack RotZ##Gun", &guns_attackRotDeg.z, -180.0f, 180.0f);
    gunROffsetAttackRot_.x = AsoUtility::Deg2RadF(guns_attackRotDeg.x);
    gunROffsetAttackRot_.y = AsoUtility::Deg2RadF(guns_attackRotDeg.y);
    gunROffsetAttackRot_.z = AsoUtility::Deg2RadF(guns_attackRotDeg.z);

  

    // 上端
    VECTOR top = swordCollider_->GetLocalPosTop();

    ImGui::InputFloat3(
        "Top##SwordCollider",
        &top.x
    );

    swordCollider_->SetLocalPosTop(top);

    // 下端
    VECTOR down = swordCollider_->GetLocalPosDown();

    ImGui::InputFloat3(
        "Down##SwordCollider",
        &down.x
    );

    swordCollider_->SetLocalPosDown(down);

    // 半径
    float radius = swordCollider_->GetRadius();

    ImGui::SliderFloat(
        "Radius##SwordCollider",
        &radius,
        0.1f,
        30.0f
    );

    swordCollider_->SetRadius(radius);
}
