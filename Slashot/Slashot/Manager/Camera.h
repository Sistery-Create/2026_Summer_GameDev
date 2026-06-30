#pragma once
#include"../Object/Actor/Player/Player.h"
#include"../Utility/AsoUtility.h"
#include<Dxlib.h>

class Camera :public ActorBase
{
public:
#define FLASHLIGHT_OUT_ANGLE (AsoUtility::Deg2RadF(20.0f)) 
#define FLASHLIGHT_IN_ANGLE  (AsoUtility::Deg2RadF(15.0f))
#define FLASHLIGHT_RANGE     30.0f

	//定数定義
	const float MOUSE_SENSITIVITY = 0.0005f;

	// カメラモード
	enum class MODE
	{
		NONE,
		FOLLOW, // 定点カメラ
		FREE, // フリーモード
	};
	// 衝突判定種別
	enum class COLLIDER_TYPE
	{
		SPHERE,
		MAX,
	};
	// カメラの初期位置
	static constexpr VECTOR DEFAULT_POS =
	{ -7.1f, 1.2f, -22.2f };
	// カメラの初期角度	
	static constexpr VECTOR DEFAULT_ANGLES =
	{ -20.0f * DX_PI_F / 180.0f, 0.0f, 0.0f };

	// 追従対象からカメラへの相対座標
	static constexpr VECTOR FOLLOW_CAMERA_LOCAL_POS = { 0.0f,100.0f, -150.0f };

	// 追従対象から注視点への相対座標
	static constexpr VECTOR FOLLOW_TARGET_LOCAL_POS = { 0.0f, 0.0f, 45.0f };

	// カメラクリップ：NEAR
	static constexpr float CAMERA_NEAR = 3.0f;	
	// カメラクリップ：FAR
	static constexpr float CAMERA_FAR = 5000.0f;
	//カメラの高さ
	static constexpr VECTOR HEAD_POS = { 2.0f,0.5f,0.3f };

	//カメラの進む速さ
	static constexpr float CAMERA_MOVE_SPEED = 1.1f;
	//カメラの回転速度
	static constexpr float CAMERA_ROT_SPEED = 1.0f * DX_PI_F / 180.0f;
	// コンストラクタ
	Camera(void);
	// デストラクタ
	~Camera(void);
	// 初期化
	void Init(void);
	// 更新
	void Update(void);
	// 描画前のカメラ設定
	void SetBeforeDraw(void);
	// 解放
	void Release(void);

	// カメラモードの変更
	void ChangeMode(MODE mode);

	//カメラのアングルを取得
	const VECTOR& GetCameraAngle(void)const { return transform_.rot; }
	//セッター
	void SetPlayer(Player* player);

	//追従対象のセット
	void SetFollow(CharactorBase* follow);

	void MoveMousePoint(void);

	void Collision(void);
protected:
	//初期化
	void InitLoad(void) override;
	//大きさ、回転、座標の初期化
	void InitTransform(void) override;
	//アニメーションの初期化
	void InitAnimation(void) override;
	//初期化後の個別処理
	void InitPost(void) override;
	//衝突判定初期化
	void InitCollider(void) override;


private:
	// 衝突時の押し戻し試行回数
	static constexpr int CNT_TRY_COLLISION_CAMERA = 30;
	// 衝突時の押し戻し量
	static constexpr float COLLISION_BACK_DIS = 2.0f;
	// 衝突判定用球体半径
	static constexpr float COL_CAPSULE_SPHERE = 30.0f;
	// カメラモード
	MODE mode_;
	// カメラの位置
	VECTOR pos_;
	// カメラの角度
	VECTOR angles_;
	//追従対象
	VECTOR targetPos_;
	// カメラに連動したライトハンドル
	int flashlightHandle_ = -1;
    //マウス感度
	float mouseSensitivity_; 

	// カメラモード別の制御処理
	void SetBeforeDrawFollow(void);
	void SetBeforeDrawFree(void);

	Player* player_=nullptr;

	//追従対象
	ActorBase* follow_;

	//追従
	const Transform* followTransform_;
};

