#include<EffekseerForDXLib.h>
#include "../Utility/AsoUtility.h"
#include"../Manager/InputManager.h"
#include"../Object/Actor/ActorBase.h"
#include"../Object/Actor/CharactorBase.h"
#include"../Object/Actor/Collider/ColliderBase.h"
#include"../Object/Actor/Collider/ColliderSphere.h"
#include"../Object/Actor/Collider/ColliderModel.h"
#include <EffekseerForDXLib.h>
#include "Camera.h"
Camera::Camera(void)
{
	
}
Camera::~Camera()
{
}
void Camera::Init(void)
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
void Camera::Update(void)
{
	SetBeforeDraw();

}
void Camera::SetBeforeDraw(void)
{
	// クリップ距離を設定する(SetDrawScreenでリセットされる)
	SetCameraNearFar(CAMERA_NEAR, CAMERA_FAR);

	switch (mode_)
	{
	case Camera::MODE::FOLLOW:
		SetBeforeDrawFollow();
		break;
		return;
	case Camera::MODE::FREE:
		SetBeforeDrawFree();
		break;
	}

	// カメラの設定(位置と角度による制御)
	SetCameraPositionAndAngle(
		transform_.pos,
		transform_.rot.x,
		transform_.rot.y,
		transform_.rot.z
	);
	
	Effekseer_Sync3DSetting();

	//printfDx("CamPos: x=%.1f y=%.1f z=%.1f\n", pos_.x, pos_.y, pos_.z);

}
void Camera::Release(void)
{
}

void Camera::ChangeMode(MODE mode)
{
	// カメラモードの変更
	mode_ = mode;
	// 変更時の初期化処理
	switch (mode_)
	{
	case Camera::MODE::FOLLOW:
		break;
	case Camera::MODE::FREE:
		break;
	}

}

void Camera::SetPlayer(Player* player)
{
	player_ = player;
}

void Camera::SetFollow(CharactorBase* follow)
{
	follow_ = follow;
	followTransform_ = &follow->GetTransform();
}

void Camera::MoveMousePoint(void)
{
	//マウスによる視点回転
	int screenCenterX = 640;
	int screenCenterY = 360;
	int mouseX, mouseY;
	GetMousePoint(&mouseX, &mouseY);

	int dx = mouseX - screenCenterX;
	int dy = mouseY - screenCenterY;

	transform_.rot.y += dx * mouseSensitivity_;// 左右
	transform_.rot.x += dy * mouseSensitivity_;// 上下

	// マウス位置を中央に戻す
	SetMousePoint(screenCenterX, screenCenterY);

}

void Camera::InitLoad(void)
{
}

void Camera::InitTransform(void)
{
	transform_.pos = DEFAULT_POS;
	pos_ = transform_.pos;
	transform_.rot = DEFAULT_ANGLES;
	angles_ = transform_.rot;

	mouseSensitivity_ = MOUSE_SENSITIVITY;

	mode_ = MODE::FREE;

	transform_.Update();
}

void Camera::InitAnimation(void)
{
}

void Camera::InitPost(void)
{
}

void Camera::InitCollider(void)
{
	// 主に地面との衝突で使用する球体コライダ
	ColliderSphere* colliderSphere = new ColliderSphere(
		ColliderBase::TAG::CAMERA,
		&transform_,
		AsoUtility::VECTOR_ZERO,
		COL_CAPSULE_SPHERE
	);
	ownColliders_.emplace(
		static_cast<int>(COLLIDER_TYPE::SPHERE), colliderSphere);
}

void Camera::SetBeforeDrawFollow(void)
{
	if (follow_ == nullptr) return;

	if (GetJoypadNum() == 0)
	{
		// 方向回転によるXYZの移動
		MoveMousePoint();
	}

	// カメラの回転行列を作成
	MATRIX mat = MGetIdent();
	mat = MMult(mat, MGetRotX(transform_.rot.x));
	mat = MMult(mat, MGetRotY(transform_.rot.y));
	//mat = MMult(mat, MGetRotZ(transform_.rot.z));

	// カメラの回転行列(X抜き)を作成
	MATRIX matY = MGetIdent();
	//mat = MMult(mat, MGetRotX(transform_.rot.x));
	matY = MMult(matY, MGetRotY(transform_.rot.y));
	//mat = MMult(mat, MGetRotZ(transform_.rot.z));

	// 注視点の移動
	VECTOR followPos = follow_->GetPos();
	//VECTOR targetLocalRotPos = VTransform(FOLLOW_TARGET_LOCAL_POS, matY);
	VECTOR targetLocalRotPos = VTransform(FOLLOW_TARGET_LOCAL_POS, mat);
	targetPos_ = VAdd(followPos, targetLocalRotPos);

	// カメラの移動
	// 相対座標を回転させて、回転後の相対座標を取得する
	VECTOR cameraLocalRotPos = VTransform(FOLLOW_CAMERA_LOCAL_POS, mat);

	// 相対座標からワールド座標に直して、カメラ座標とする
	transform_.pos = VAdd(followPos, cameraLocalRotPos);

	// カメラの上方向を計算
	VECTOR up = VTransform(AsoUtility::DIR_U, mat);

	// カメラの設定(位置と注視点による制御)
	SetCameraPositionAndTargetAndUpVec(
		transform_.pos,
		targetPos_,
		up
	);

	//衝突判定
	Collision();
}

void Camera::SetBeforeDrawFree(void)
{
	auto& ins = InputManager::GetInstance();
	// WASDでカメラの位置を変える
	float movePow = CAMERA_MOVE_SPEED;
	if (ins.IsPress(KEY_INPUT_W)) { transform_.pos.z += movePow; }
	if (ins.IsPress(KEY_INPUT_A)) { transform_.pos.x -= movePow; }
	if (ins.IsPress(KEY_INPUT_S)) { transform_.pos.z -= movePow; }
	if (ins.IsPress(KEY_INPUT_D)) { transform_.pos.x += movePow; }
	if (ins.IsPress(KEY_INPUT_Q)) { transform_.pos.y += movePow; }
	if (ins.IsPress(KEY_INPUT_E)) { transform_.pos.y -= movePow; }
	// 矢印キーでカメラの角度を変える
	float rotPow = CAMERA_ROT_SPEED;
	if (ins.IsPress(KEY_INPUT_DOWN)) { transform_.rot.x += rotPow; }
	if (ins.IsPress(KEY_INPUT_UP)) { transform_.rot.x -= rotPow; }
	if (ins.IsPress(KEY_INPUT_RIGHT)) { transform_.rot.y += rotPow; }
	if (ins.IsPress(KEY_INPUT_LEFT)) { transform_.rot.y -= rotPow; }

	// カメラの設定(位置と角度による制御)
	SetCameraPositionAndAngle(
		transform_.pos	,
		transform_.rot.x,
		transform_.rot.y,
		transform_.rot.z
	);
}
void Camera::Collision(void)
{
	// プレイヤーのルートフレーム
	VECTOR start = MV1GetFramePosition(followTransform_->modelId, 1);

	for (const auto& hitCol : hitColliders_)
	{
		// モデル以外は処理を飛ばす
		if (hitCol->GetShape() != ColliderBase::SHAPE::MODEL) continue;
		// 派生クラスへキャスト
		const ColliderModel* colliderModel =
			dynamic_cast<const ColliderModel*>(hitCol);
		if (colliderModel == nullptr) continue;
		// 線分で衝突判定
		auto hits = MV1CollCheck_LineDim(
			colliderModel->GetFollow()->modelId,
			-1,
			transform_.pos,
			start
		);
		// 追従対象に一番近い衝突点を探す
		bool isCollision = false;
		MV1_COLL_RESULT_POLY hitPoly;
		double minDist = DBL_MAX;
		for (int i = 0; i < hits.HitNum; i++)
		{
			const auto& hit = hits.Dim[i];
			// 除外フレームは無視する
			if (colliderModel->IsExcludeFrame(hit.FrameIndex))
			{
				continue;
			}
			// 衝突判定
			isCollision = true;
			// 距離判定
			double dist = AsoUtility::Distance(start, hit.HitPosition);
			if (dist < minDist)
			{
				// 追従対象に一番近い衝突点を優先
				minDist = dist;
				hitPoly = hit;
			}
		}
		// 検出した地面ポリゴン情報の後始末
		MV1CollResultPolyDimTerminate(hits);
		if (!isCollision)
		{
			// 衝突していなければ次のコライダへ
			continue;
		}
		// カメラ位置から注視点への方向
		VECTOR dirToTarget = VNorm(VSub(targetPos_, transform_.pos));
		// 衝突点の少し手前にカメラを置く
		transform_.pos =
			VAdd(hitPoly.HitPosition,
				VScale(dirToTarget, COLLISION_BACK_DIS));

		// カメラ位置の球体コライダ
		int typeSphere = static_cast<int>(COLLIDER_TYPE::SPHERE);
		// 球体コライダが無ければ処理を抜ける
		if (ownColliders_.count(typeSphere) == 0) continue;
		// 球体コライダ情報
		ColliderSphere* colliderSphere =
			dynamic_cast<ColliderSphere*>(ownColliders_.at(typeSphere));
		if (colliderSphere == nullptr) return;
		// 衝突補正処理
		int sphereCnt = 0;
		while (sphereCnt < CNT_TRY_COLLISION_CAMERA)
		{
			// 球体と三角形の当たり判定
			bool isSphereHit = HitCheck_Sphere_Triangle(
				colliderSphere->GetPos(),
				colliderSphere->GetRadius(),
				hitPoly.Position[0],
				hitPoly.Position[1],
				hitPoly.Position[2]);

			if (!isSphereHit)
			{
				break;
			}
			// 衝突していたら法線方向に押し戻し
			transform_.pos =
				VAdd(transform_.pos,
					VScale(hitPoly.Normal, COLLISION_BACK_DIS));
			sphereCnt++;
		}


	}
}