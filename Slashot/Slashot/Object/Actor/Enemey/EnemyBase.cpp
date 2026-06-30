#include "EnemyBase.h"
#include "../../../Libs/Libs/ImGui/imgui.h"
#include"../../Actor/Player/Player.h"
#include"../../../Manager/InputManager.h"
#include"../../../Object/AnimationController.h"
#include"../../../Scene/GameScene.h"
#include"../../../Manager/SceneManager.h"
#include"../../../Manager/SoundManager.h"
#include"../../../Application.h"
#include"../Collider/ColliderBase.h"
#include"../Collider/ColliderLine.h"
#include"../Collider/ColliderCapsule.h"
#include<EffekseerForDXLib.h>
EnemyBase::EnemyBase()
{
}

EnemyBase::~EnemyBase()
{
}

void EnemyBase::Load()
{
	// リソースロード
	InitLoad();
}

void EnemyBase::LoadEnd()
{
	Init();
}

void EnemyBase::Init()
{
	
	// Transform初期化
	InitTransform();
	// アニメーションの初期化
	InitAnimation();
	// 初期化後の個別処理
	InitPost();
	// 衝突判定初期化
	InitCollider();
}

void EnemyBase::Update(void)
{
	if (isDead_)
	{
		return;
	}

	InputManager& ins = InputManager::GetInstance();

	if (gameScene_ != nullptr)
	{
		Player* player = gameScene_->GetPlayer();
		if (player != nullptr)
		{
			playerPos_ = player->GetPos();
		}
	}
	animationController_->Update();
	Death();
	SearchPlayer();
	EnemyStatement();
	//FollowPlayer();

	if (damageInterval_ > 0.0f)
	{
		damageInterval_ -= SceneManager::GetInstance().GetDeltaTime();
	}
	if (attackInterval_ > 0.0f)
	{
		attackInterval_ -= SceneManager::GetInstance().GetDeltaTime();
	}

	//UpdateDebug();

	CharactorBase::Update();

	transform_.quaRot = Quaternion::Euler(transform_.rot);

	ColliderUpdate();

	for (auto it = damageTexts_.begin();
		it != damageTexts_.end();)
	{
		it->timer -= SceneManager::GetInstance().GetDeltaTime();

		// 上へ浮かぶ
		it->offsetY += 40.0f *
			SceneManager::GetInstance().GetDeltaTime();

		if (it->timer <= 0.0f)
		{
			it = damageTexts_.erase(it);
		}
		else
		{
			++it;
		}
	}

	if (ins.IsTrgMouseRight())
	{
		isDebug_ = !isDebug_;
	}

}

void EnemyBase::Draw(void)
{
	if (isDead_)
	{
		return;
	}

	CharactorBase::Draw();
	for (const auto& text : damageTexts_)
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

		std::string damageStr =
			std::to_string(text.damage);

		// 影
		DrawStringToHandle(
			static_cast<int>(screenPos.x) + 2,
			static_cast<int>(screenPos.y) + 2,
			damageStr.c_str(),
			GetColor(0, 0, 0),
			damageFontHandle_
		);

		// 本体
		DrawStringToHandle(
			static_cast<int>(screenPos.x),
			static_cast<int>(screenPos.y),
			damageStr.c_str(),
			GetColor(255, 80, 80),
			damageFontHandle_
		);
	}
}

void EnemyBase::Release(void)
{
	delete animationController_;
	animationController_ = nullptr;

	MV1DeleteModel(modelId_);
}

void EnemyBase::SetGameScene(GameScene* gameScene)
{
	gameScene_ = gameScene;
}
void EnemyBase::ChangeState(EnemyState newState)
{
	if (enemyState_ == newState)
	{
		return;
	}

	enemyState_ = newState;

	switch (enemyState_)
	{
	case EnemyState::IDLE:

		animationController_->Play(
			static_cast<int>(ANIM_TYPE::IDLE),
			true
		);

		break;

	case EnemyState::WANDER:

		animationController_->Play(
			static_cast<int>(ANIM_TYPE::WALK),
			true
		);

		break;

	case EnemyState::CHASE:

		animationController_->Play(
			static_cast<int>(ANIM_TYPE::WALK),
			true
		);

		break;

	case EnemyState::ATTACK:

		animationController_->Play(
			static_cast<int>(ANIM_TYPE::ATTACK),
			false
		);

		break;
	case EnemyState::DAMAGE:

		animationController_->Play(
			static_cast<int>(ANIM_TYPE::DAMAGE),
			false
		);

		break;
	}
}
//決められた範囲内でランダムな位置にスポーンさせる関数
void EnemyBase::SetSpawnPos(const VECTOR& spawnPos)
{
	transform_.pos = spawnPos;
}
void EnemyBase::InitLoad(void)
{
	modelId_ = MV1LoadModel("Data/Model/Enemy/Spider.mv1");
	deathEffect_ =LoadEffekseerEffect("Data/Effect/Enemy/Death.efkefc");
	LoadEnd();
}

void EnemyBase::InitTransform(void)
{
	transform_.SetModel(modelId_);
	transform_.pos = DEFAULT_POS;
	pos_ = transform_.pos;


	transform_.rot = VGet(0.0f, AsoUtility::Deg2RadF(180.0f), 0.0f);
	angle_ = transform_.rot;
	
	transform_.scl = VGet(1.0f, 1.0f, 1.0f);

	wanderTarget_ = transform_.pos;
	wanderTimer_ = 3.0f;

	

	damageFontHandle_ = CreateFontToHandle(
		"Arial", 
		32,      
		3      
	);
}

void EnemyBase::InitAnimation(void)
{
	animationController_ = new AnimationController(transform_.modelId);

	animationController_->AddInFbx(static_cast<int>(ANIM_TYPE::IDLE), ANIMATION_SPEED, 5);
	animationController_->AddInFbx(static_cast<int>(ANIM_TYPE::WALK), ANIMATION_SPEED*2, 1);
	animationController_->AddInFbx(static_cast<int>(ANIM_TYPE::ATTACK), ANIMATION_SPEED,3);
	animationController_->AddInFbx(static_cast<int>(ANIM_TYPE::JUMP), ANIMATION_SPEED, 9);
	animationController_->AddInFbx(static_cast<int>(ANIM_TYPE::DEATH), ANIMATION_SPEED, 10);
	animationController_->AddInFbx(static_cast<int>(ANIM_TYPE::DAMAGE),ANIMATION_SPEED,7);
	ChangeState(EnemyState::IDLE);
}

void EnemyBase::InitPost(void)
{
}

void EnemyBase::InitCollider(void)
{
	ColliderLine* colLine = new ColliderLine(
		ColliderBase::TAG::ENEMY,
		&transform_,
		COL_LINE_START_LOCAL_POS,
		COL_LINE_END_LOCAL_POS
	);

	ownColliders_.emplace(
		static_cast<int>(COLLIDER_TYPE::LINE),
		colLine
	);

	
	colCapsule_ = new ColliderCapsule(
		ColliderBase::TAG::ENEMY,
		&transform_,
		COL_CAPSULE_TOP_LOCAL_POS,
		COL_CAPSULE_DOWN_LOCAL_POS,
		COL_CAPSULE_RADIUS
	);


	ownColliders_.emplace(
		static_cast<int>(COLLIDER_TYPE::CAPSULE),
		colCapsule_
	);


	colAttack_ = new ColliderCapsule(
		ColliderBase::TAG::ENEMY_ATTACK,
		&transform_,
		VGet(0.0f, 15.0f, 20.0f),
		VGet(0.0f, 15.0f, 60.0f),
		10.0f
	);

	
	ownColliders_.emplace(
		static_cast<int>(COLLIDER_TYPE::CAPSULE),
		colAttack_
	);
}

void EnemyBase::UpdateProcces(void)
{
}

void EnemyBase::UpdateProccesPost(void)
{
}

void EnemyBase::UpdateIdle()
{
	wanderTimer_ -= scnMng_.GetDeltaTime();

	// 一定時間経過したら巡回
	if (wanderTimer_ <= 0.0f)
	{
		ChangeState(EnemyState::WANDER);

		float randX =
			GetRand(1000) / 1000.0f * wanderRange_ - wanderRange_ * 0.5f;

		float randZ =
			GetRand(1000) / 1000.0f * wanderRange_ - wanderRange_ * 0.5f;

		wanderTarget_ =
		{
			transform_.pos.x + randX,
			transform_.pos.y,
			transform_.pos.z + randZ
		};

	}

	// Player発見
	if (isFoundPlayer_)
	{
		ChangeState(EnemyState::CHASE);
	}
}
void EnemyBase::UpdateWander()
{
	VECTOR dir = VSub(wanderTarget_, transform_.pos);

	dir.y = 0.0f;

	float dist = VSize(dir);

	// 到着
	if (dist < 2.0f)
	{
		ChangeState(EnemyState::IDLE);

		wanderTimer_ = 2.0f + GetRand(200) / 100.0f;

		animationController_->Play(
			static_cast<int>(ANIM_TYPE::IDLE),
			true
		);

		return;
	}

	dir = VNorm(dir);

	movePow_ = VScale(
		dir,
		wanderSpeed_
	);


	transform_.pos = VAdd(
		transform_.pos,
		movePow_
	);

	// 向き
	transform_.rot.y =
		atan2f(dir.x, dir.z) + DX_PI_F;

	// Player発見
	if (isFoundPlayer_)
	{
		ChangeState(EnemyState::CHASE);
	}
}
void EnemyBase::UpdateChase()
{
	if (gameScene_ == nullptr) return;

	movePow_ = VGet(0.0f, 0.0f, 0.0f);

	if (!isFoundPlayer_)
	{
		ChangeState(EnemyState::IDLE);

		wanderTimer_ = 2.0f;

		return;
	}

	Player* player = gameScene_->GetPlayer();

	if (player == nullptr) return;

	VECTOR playerPos = player->GetPos();

	VECTOR toPlayer = VSub(playerPos, transform_.pos);

	toPlayer.y = 0.0f;

	distance_ = VSize(toPlayer);

	// 攻撃判定
	if (distance_ <= attackRange_ &&
		attackInterval_ <= 0.0f)
	{
		isAttacking_ = true;

		ChangeState(EnemyState::ATTACK);
		isHitPlayer_ = false;


		return;
	}

	if (distance_ <= 0.01f)
	{
		return;
	}

	toPlayer = VNorm(toPlayer);

	float targetRotY = atan2f(toPlayer.x, toPlayer.z);

	targetRotY += DX_PI;

	float diff = targetRotY - transform_.rot.y;

	if (diff > DX_PI) diff -= DX_TWO_PI;
	if (diff < -DX_PI) diff += DX_TWO_PI;

	transform_.rot.y += diff * 0.1f;

	// 一定距離までは近づく
	stopDistance_ = attackRange_ * 0.8f;

	// stopDistanceより遠い時だけ移動
	if (distance_ > stopDistance_)
	{
		transform_.pos.x += toPlayer.x * moveSpeed_;
		transform_.pos.z += toPlayer.z * moveSpeed_;
	}

}
void EnemyBase::UpdateAttack()
{
	if (!isAttacking_)
	{
		return;
	}

	AsoUtility::EqualsVZero(movePow_);

	if (animationController_->IsEnd(
		static_cast<int>(ANIM_TYPE::ATTACK)))
	{
		isAttacking_ = false;

		attackInterval_ = ATTACK_COOL_MAX_;

		ChangeState(EnemyState::CHASE);
	}

}
void EnemyBase::ColliderUpdate()
{
	Player* player = gameScene_->GetPlayer();

	if (player == nullptr)
	{
		return;
	}

	if (isAttacking_)
	{
		hitCapsules_.clear();

		CreateHitCapsule(
			rightLegFrame_,
			rightLegUpFrame_,
			attackRadius_
		);

		CreateHitCapsule(
			leftLegFrame_,
			leftLegUpFrame_,
			attackRadius_
		);


		if (isHitPlayer_)
		{
			return;
		}

		const std::vector<Player::HitCapsule>& capsules =
			player->GetHitCapsules();

		if (capsules.empty())
		{
			return;
		}

		VECTOR playerPos = player->GetPos();

		for (const auto& enemyCapsule : hitCapsules_)
		{
			for (const auto& playerCapsule : capsules)
			{
				VECTOR enemyCenter =
					VScale(
						VAdd(enemyCapsule.start, enemyCapsule.end),
						0.5f
					);

				VECTOR playerCenter =
					VScale(
						VAdd(playerCapsule.start, playerCapsule.end),
						0.5f
					);

				float dist =
					VSize(
						VSub(playerCenter, enemyCenter)
					);

				float hitRange =
					playerCapsule.radius +
					enemyCapsule.radius;

				if (dist <= hitRange)
				{
					player->TakeDamage(10.0f);

					isHitPlayer_ = true;

					break;
				}
			}

			if (isHitPlayer_)
			{
				break;
			}
		}

	}
}
ColliderCapsule* EnemyBase::GetCollider()
{
	return colCapsule_;
}

void EnemyBase::TakeDamage(int damage)
{
	if (damageInterval_ > 0.0f||isInvincible)
	{
		return;
	}

	hp_ -= damage;

	damageInterval_ = DAMAGE_INTERVAL_MAX;

	// 攻撃中断
	isAttacking_ = false;

	ChangeState(EnemyState::DAMAGE);

	// ダメージ文字
	DamageText text;

	text.damage = damage;

	text.pos = transform_.pos;

	text.pos.y += 35.0f;

	text.timer = 1.0f;

	text.offsetY = 0.0f;

	damageTexts_.push_back(text);
}

void EnemyBase::Death()
{
	if (!isDead_)
	{
		if (hp_ <= 0)
		{
			ChangeState(EnemyState::DEATH);
			//PlayDeathEffect();
			SoundManager::GetInstance().Play(SoundManager::SRC::ENEMY_DEATH, Sound::TIMES::ONCE);
			if (!isCountedKill_)
			{
				gameScene_->AddKillEnemy();
				isCountedKill_ = true;
			}
		}
	}
}
void EnemyBase::UpdateDebug(void)
{
    // ウィンドウタイトル&開始処理
    ImGui::Begin("Enemy");
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
    ImGui::End();
    ImGui::Separator();
    ImGui::Text("Sword Offset (Hand)");
}

void EnemyBase::SearchPlayer(void)
{
	if (gameScene_ == nullptr) return;

	Player* player = gameScene_->GetPlayer();
	if (player == nullptr) return;

	VECTOR playerPos = player->GetPos();

	VECTOR toPlayer = VSub(playerPos, transform_.pos);

	distance_ = VSize(toPlayer);

	dictanceToPlayer_ = distance_;

	isFoundPlayer_ = (distance_ <= searchRange_);
}

void EnemyBase::EnemyStatement()
{
	switch (enemyState_)
	{
	case EnemyState::IDLE:
		UpdateIdle();
		
		break;

	case EnemyState::WANDER:
		UpdateWander();
	
		break;

	case EnemyState::CHASE:
		UpdateChase();
		
		break;

	case EnemyState::ATTACK:
		UpdateAttack();
		break;
	case EnemyState::DEATH:
	
		isInvincible = true;
	    animationController_->Play(static_cast<int>(ANIM_TYPE::DEATH), false, AnimationController::PLAY_MODE::FULL_BODY);
		if (!isCreateScorePopup_)
		{
			gameScene_->CreateScorePopup(
				scoreValue_,
				transform_.pos
			);

			isCreateScorePopup_ = true;
		}
		if (animationController_->IsEnd(static_cast<int>(ANIM_TYPE::DEATH)))
		{
			isDead_ = true;
			gameScene_->AddScore(scoreValue_);
		}
		break;
	case EnemyState::DAMAGE:
		if (isInvincible)
		{
			return;
		}
		movePow_ = AsoUtility::VECTOR_ZERO;

		if (animationController_->IsEnd(
			static_cast<int>(ANIM_TYPE::DAMAGE)))
		{
			if (hp_ <= 0)
			{
				ChangeState(EnemyState::DEATH);
			}
			else
			{
				ChangeState(EnemyState::CHASE);
			}
		}
		break;
	}
}

void EnemyBase::PlayDeathEffect()
{
	if (isDead_)
	{
		return;
	}
	VECTOR start =
		MV1GetFramePosition(
			transform_.modelId,
			effectFrame_);

	VECTOR dir =
	{
		-sinf(transform_.rot.y),
		0.0f,
		-cosf(transform_.rot.y)
	};

	// ヒーリング
	int playHandle =
		PlayEffekseer3DEffect(
		     deathEffect_);

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

void EnemyBase::CreateHitCapsule(int startFrame, int endFrame, float radius)
{
	HitCapsule capsule;

	capsule.start =
		MV1GetFramePosition(modelId_, startFrame);

	capsule.end =
		MV1GetFramePosition(modelId_, endFrame);

	capsule.radius = radius;

	hitCapsules_.push_back(capsule);
}

