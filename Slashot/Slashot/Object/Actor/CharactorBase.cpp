#include "CharactorBase.h"
#include"../AnimationController.h"
#include"../../Utility/AsoUtility.h"
#include"../../Manager/SceneManager.h"
#include"../../Application.h"
#include"../Actor/Collider/ColliderLine.h"
#include"../Actor/Collider/ColliderModel.h"
#include"../Actor/Collider/ColliderCapsule.h"
CharactorBase::CharactorBase(void)
	:
	ActorBase(),
	stepJump_(0.0f),
	isJump_(false),
	jumpPow_(AsoUtility::VECTOR_ZERO),
	moveDir_(AsoUtility::VECTOR_ZERO),
	movePow_(AsoUtility::VECTOR_ZERO),
	animCon_(nullptr)

{

}

CharactorBase::~CharactorBase(void)
{
}

void CharactorBase::Update(void)
{
	

	prevPos_ = transform_.pos;

	UpdateProcces();

	//DelayRotate();

	CalcGravityPow();

	CollisionReserve();

	Collision();

	transform_.Update();

	if (animCon_ != nullptr)
	{
		animCon_->Update();
	}

	UpdateProccesPost();

	

}

void CharactorBase::Draw(void)
{
	// 基底クラスの描画処理
	ActorBase::Draw();
	// 丸影の描画
	DrawShadow();
}

void CharactorBase::Release(void)
{

}


void CharactorBase::InitLoad(void)
{
	// 丸影画像
	//imgShadow_ = resMng_.Load(ResourceManager::SRC::PLAYER_SHADOW).handleId_;
}

void CharactorBase::UpdateProcces(void)
{
}

void CharactorBase::UpdateProccesPost(void)
{
}

void CharactorBase::DelayRotate(void)
{
	//移動方向から回転
	Quaternion goalRot = Quaternion::LookRotation(moveDir_);

	//回転の補完
	transform_.quaRot = Quaternion::Slerp(transform_.quaRot, goalRot, 0.2f);

}

void CharactorBase::CalcGravityPow(void)
{
	//重力方向
	VECTOR dirGravity = AsoUtility::DIR_D;
	//重力の強さ
	float gravityPow = Application::GetInstance().GetGravityPow() * scnMng_.GetDeltaTime();
	//重力
	VECTOR gravity = VScale(dirGravity, gravityPow);
	jumpPow_ = VAdd(jumpPow_, gravity);

	// 重力速度の制限
	if (jumpPow_.y < MAX_FALL_SPEED)
	{
		jumpPow_.y = MAX_FALL_SPEED;
	}

}

void CharactorBase::Collision(void)
{
	//チェック
	CheckStepUp();
	// 移動処理
	transform_.pos = VAdd(transform_.pos, movePow_);
	// 衝突(カプセル)
	CollisionCapsule();
	// ジャンプ量を加算
	transform_.pos = VAdd(transform_.pos, jumpPow_);
	// 衝突(重力)
	CollisionGravity();
}

void CharactorBase::CollisionGravity(void)
{
	//落下中のみ判定する
	if (VDot(AsoUtility::DIR_D, VNorm(jumpPow_)) <= 0.9f)
	{
		return;
	}
	// 線分コライダ
	int lineType = static_cast<int>(COLLIDER_TYPE::LINE);
	// 線分コライダが無ければ処理を抜ける
	if (ownColliders_.count(lineType) == 0) return;
	// 線分コライダ情報

	ColliderLine* colliderLine_ = dynamic_cast<ColliderLine*>(ownColliders_.at(lineType));
	if (colliderLine_ == nullptr) return;
	// 線分の始点と終点を取得
	VECTOR s = colliderLine_->GetPosStart();
	VECTOR e = colliderLine_->GetPosEnd();
	// 登録されている衝突物を全てチェック
	for (const auto& hitCol : hitColliders_)
	{
		// ステージ以外は処理を飛ばす
		if (hitCol->GetTag() != ColliderBase::TAG::STAGE) continue;
		// 派生クラスへキャスト
		const ColliderModel* colliderModel =
			dynamic_cast<const ColliderModel*>(hitCol);
		if (colliderModel == nullptr) continue;
		// ステージモデル(地面)との衝突
		auto hit = MV1CollCheck_Line(
			colliderModel->GetFollow()->modelId, -1, s, e);
		//除外フレームは無視
		if (colliderModel->IsExcludeFrame(hit.FrameIndex))
		{
			continue;
		}
		if (hit.HitFlag > 0)
		{
			// 衝突地点から、少し上に移動
			transform_.pos =
				VAdd(hit.HitPosition, VScale(AsoUtility::DIR_U, 2.0f));
			// ジャンプリセット
			jumpPow_ = AsoUtility::VECTOR_ZERO;
			//ジャンプの入力受付時間リセット
			stepJump_ = 0.0f;
			//ジャンプ判定
			isJump_ = false;
		}
	}
}

void CharactorBase::CollisionCapsule(void)
{
	// カプセルコライダ
	int capsuleType = static_cast<int>(COLLIDER_TYPE::CAPSULE);
	// カプセルコライダが無ければ処理を抜ける
	if (ownColliders_.count(capsuleType) == 0) return;
	// カプセルコライダ情報
	ColliderCapsule* colliderCapsule =
		dynamic_cast<ColliderCapsule*>(ownColliders_.at(capsuleType));
	if (colliderCapsule == nullptr) return;
	// 登録されている衝突物を全てチェック
	for (const auto& hitCol : hitColliders_)
	{
		// モデル以外は処理を飛ばす
		if (hitCol->GetShape() != ColliderBase::SHAPE::MODEL) continue;
		// 派生クラスへキャスト
		const ColliderModel* colliderModel =
			dynamic_cast<const ColliderModel*>(hitCol);
		if (colliderModel == nullptr) continue;
		auto hits = MV1CollCheck_Capsule(
			colliderModel->GetFollow()->modelId, -1,
			colliderCapsule->GetPosTop(), colliderCapsule->GetPosDown(),
			colliderCapsule->GetRadius());
		// 衝突した複数のポリゴンと衝突回避するまで、
		// プレイヤーの位置を移動させる
		for (int i = 0; i < hits.HitNum; i++)
		{
			auto hit = hits.Dim[i];

			//除外フレームは無視
			if (colliderModel->IsExcludeFrame(hit.FrameIndex))
			{
				continue;
			}
			// 地面と異なり、衝突回避位置が不明なため、何度か移動させる
			// この時、移動させる方向は、移動前座標に向いた方向であったり、
			// 衝突したポリゴンの法線方向だったりする
			for (int tryCnt = 0; tryCnt < CNT_TRY_COLLISION; tryCnt++)
			{
				// 再度、モデル全体と衝突検出するには、効率が悪過ぎるので、
				// 最初の衝突判定で検出した衝突ポリゴン1枚と衝突判定を取る
				int pHit = HitCheck_Capsule_Triangle(
					colliderCapsule->GetPosTop(), colliderCapsule->GetPosDown(),
					colliderCapsule->GetRadius(),

					hit.Position[0], hit.Position[1], hit.Position[2]);
				if (pHit)
				{
					// 法線の方向にちょっとだけ移動させる
					transform_.pos =
						VAdd(transform_.pos,
							VScale(hit.Normal, COLLISION_BACK_DIS));

					continue;
				}
				break;
			}

			
		}
		// 検出した地面ポリゴン情報の後始末
		MV1CollResultPolyDimTerminate(hits);
	}
}

void CharactorBase::CheckStepUp(void)
{
	// 移動していないならチェック不要
	if (VSize(movePow_) < 0.01f) return;

	
	const float STEP_LIMIT = 10.0f;   // 登れる限界（これより高いと壁としてぶつかる）
	const float EXTRA_MARGINE = 5.0f; // 限界より少し高い位置からレイを飛ばす
	const float CHECK_HEIGHT = STEP_LIMIT + EXTRA_MARGINE;
	
	VECTOR nextPos = VAdd(transform_.pos, movePow_);

	
	VECTOR rayStart = VAdd(nextPos, VGet(0, CHECK_HEIGHT, 0));
	VECTOR rayEnd = VSub(nextPos, VGet(0, 5.0f, 0));

	
	for (const auto& hitCol : hitColliders_)
	{
		if (hitCol->GetTag() != ColliderBase::TAG::STAGE) continue;

		const ColliderModel* colModel = dynamic_cast<const ColliderModel*>(hitCol);
		if (!colModel) continue;

		
		auto hit = MV1CollCheck_Line(colModel->GetFollow()->modelId, -1, rayStart, rayEnd);

		if (hit.HitFlag > 0)
		{
			// ヒットした場所の高さ
			float hitY = hit.HitPosition.y;
			// 現在の高さとの差
			float diffY = hitY - transform_.pos.y;

			// 4. 段差が「0より高く」「限界以下」なら足元を乗せる
			if (diffY >0.1f && diffY <= STEP_LIMIT)
			{
				// 先にY座標だけ更新して段差の上に載せる
				transform_.pos.y = hitY;
				jumpPow_ = AsoUtility::VECTOR_ZERO; // ジャンプ力リセット
			}
		}
	}
}

void CharactorBase::DrawShadow(void)
{
	// 定数定義 (CharactorBase.h に移動を推奨)
	const float PLAYER_SHADOW_HEIGHT = 800.0f;
	const float PLAYER_SHADOW_SIZE = 30.0f;
	const int SHADOW_MAX_ALPHA = 128; // 影の最大不透明度

	int ModelHandle = -1;
	// hitColliders_ からステージモデルのハンドルを取得
	for (const auto& hitCol : hitColliders_)
	{
		if (hitCol->GetTag() == ColliderBase::TAG::STAGE)
		{
			// 派生クラスへキャスト
			const ColliderModel* colliderModel =
				dynamic_cast<const ColliderModel*>(hitCol);
			if (colliderModel != nullptr)
			{
				// ステージモデルのハンドルを取得
				ModelHandle = colliderModel->GetFollow()->modelId;
				break; // 最初のステージモデルが見つかればOKとする
			}
		}
	}

	// ステージモデルが見つからなければ処理を終了
	if (ModelHandle == -1) return;

	// プレイヤーの現在座標と影画像ハンドル
	VECTOR playerPos = transform_.pos;
	int shadowHandle = imgShadow_;

	//描画設定
	// ライティングを無効にする
	SetUseLighting(FALSE);
	// Ｚバッファを有効にする (地面とのちらつき防止)
	SetUseZBuffer3D(TRUE);
	// テクスチャアドレスモードを CLAMP にする
	SetTextureAddressMode(DX_TEXADDRESS_CLAMP);

	// --- 地面ポリゴン検出 ---
	MV1_COLL_RESULT_POLY_DIM HitResDim = MV1CollCheck_Capsule(
		ModelHandle,
		-1,
		playerPos,
		VAdd(playerPos, VGet(0.0f, -PLAYER_SHADOW_HEIGHT, 0.0f)),
		PLAYER_SHADOW_SIZE
	);

	// 頂点データ
	VERTEX3D Vertex[3];

	// ★ 最も近いポリゴンを探すための変数
	int nearestIndex = -1;
	float nearestDist = 999999.0f;

	// HitResDim.Dim は配列
	for (int i = 0; i < HitResDim.HitNum; i++)
	{
		MV1_COLL_RESULT_POLY* poly = &HitResDim.Dim[i];

		// ポリゴンの代表点：頂点 0 の高さを使う
		float dist = playerPos.y - poly->Position[0].y;

		// プレイヤーより上のポリゴンは無視（念のため）
		if (dist < 0.0f) continue;

		if (dist < nearestDist)
		{
			nearestDist = dist;
			nearestIndex = i;
		}
	}

	// ★ ヒットポリゴンが無い場合は描画しない
	if (nearestIndex == -1)
	{
		MV1CollResultPolyDimTerminate(HitResDim);
		SetUseLighting(TRUE);
		SetUseZBuffer3D(FALSE);
		return;
	}

	// ★ 最も近いポリゴンだけで影を描画
	MV1_COLL_RESULT_POLY* HitRes = &HitResDim.Dim[nearestIndex];

	// 頂点の基本設定
	for (int k = 0; k < 3; k++)
	{
		Vertex[k].dif = GetColorU8(255, 255, 255, 255);
		Vertex[k].spc = GetColorU8(0, 0, 0, 0);
	}

	// ポリゴンの座標をセット
	Vertex[0].pos = HitRes->Position[0];
	Vertex[1].pos = HitRes->Position[1];
	Vertex[2].pos = HitRes->Position[2];

	// 少し持ち上げてZファイティング防止
	VECTOR SlideVec = VScale(HitRes->Normal, 0.5f);
	Vertex[0].pos = VAdd(Vertex[0].pos, SlideVec);
	Vertex[1].pos = VAdd(Vertex[1].pos, SlideVec);
	Vertex[2].pos = VAdd(Vertex[2].pos, SlideVec);

	// アルファ設定（距離減衰）
	for (int k = 0; k < 3; k++)
	{
		float distFactor = 1.0f - fabsf(HitRes->Position[k].y - playerPos.y) / PLAYER_SHADOW_HEIGHT;
		if (distFactor < 0) distFactor = 0;
		Vertex[k].dif.a = (unsigned char)(SHADOW_MAX_ALPHA * distFactor);
	}

	// UV計算
	float size = PLAYER_SHADOW_SIZE * 2.0f;
	Vertex[0].u = (HitRes->Position[0].x - playerPos.x) / size + 0.5f;
	Vertex[0].v = (HitRes->Position[0].z - playerPos.z) / size + 0.5f;
	Vertex[1].u = (HitRes->Position[1].x - playerPos.x) / size + 0.5f;
	Vertex[1].v = (HitRes->Position[1].z - playerPos.z) / size + 0.5f;
	Vertex[2].u = (HitRes->Position[2].x - playerPos.x) / size + 0.5f;
	Vertex[2].v = (HitRes->Position[2].z - playerPos.z) / size + 0.5f;

	// ★ 最も近い1枚のポリゴンだけ描画
	DrawPolygon3D(Vertex, 1, shadowHandle, TRUE);

	// 後始末
	MV1CollResultPolyDimTerminate(HitResDim);

	// 描画設定を戻す
	SetUseLighting(TRUE);
	SetUseZBuffer3D(FALSE);
}