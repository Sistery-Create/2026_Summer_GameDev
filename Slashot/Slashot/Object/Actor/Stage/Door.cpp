#include "Door.h"
#include"../../../Object/Actor/Item/Item.h"		
#include"../../../Utility/AsoUtility.h"
#include"../../../Manager/InputManager.h"
#include"../../../Application.h"
#include"../../../Manager/SoundManager.h"

Door::Door()
{
}
Door::~Door()
{
}

void Door::Init()
{
	//初期化
	modelId_ = MV1LoadModel("Data/Model/Stage/NewDoor.mv1");
	pos_ = VGet(27.95f, 39.80f, -20.08f);
	rot_ = VGet(0.0f, 0.0f, 0.0f);
	scale_ = VGet(0.03f, 0.03f, 0.03f);
	isDown_ = false;
	openAngle_ = 90.0f;
	moveSpeed_ = 0.1f;

	MV1SetPosition(modelId_, pos_);
	MV1SetRotationXYZ(modelId_, rot_);
	MV1SetScale(modelId_, scale_);
}

void Door::Update(const VECTOR& playerPos)
{
	auto& ins = InputManager::GetInstance();

	// 判定距離の二乗を設定
	const float OPEN_DISTANCE_SQ = 144.0f;
	
	VECTOR diff = VSub(pos_, playerPos);


	float distanceSq = VDot(diff, diff);

	//プレイヤーがドアの近くにいるか
	isPlayerNear = (distanceSq <= OPEN_DISTANCE_SQ);

	hasAllItems = (item_ != nullptr && item_->AllGet());

	isOperable_ = (isPlayerNear && hasAllItems);

	// Eキーが新しく押されたかをチェック
	if (GetJoypadNum() == 0)
	{
		if (ins.IsPress(KEY_INPUT_E)) {

			// 近くにいる かつ アイテムを全て持っているときのみ操作可能
			if (isOperable_)
			{
				// ドアが初期位置にある、または下降しきっている場合にのみ状態を切り替える
				if (pos_.y == 39.80f || pos_.y == 34.00f)
				{
					isDown_ = !isDown_; // 状態を反転
				}
			}
		}
	}
	else
	{
		// 接続されているゲームパッド１の情報を取得
		InputManager::JOYPAD_IN_STATE padState =
			ins.GetJPadInputState(InputManager::JOYPAD_NO::PAD1);
		// Bボタンが押されたらゲームシーンへ
		if (ins.IsPadBtnNew(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::RIGHT))
		{

			// 近くにいる かつ アイテムを全て持っているときのみ操作可能
			if (isOperable_)
			{
				// ドアが初期位置にある、または下降しきっている場合にのみ状態を切り替える
				if (pos_.y == 39.80f || pos_.y == 34.00f)
				{
					isDown_ = !isDown_; // 状態を反転
				}
			}
		}
	}

	DownDoor(); // DownDoor() の中で isDown_ とアイテムチェックに基づいてドアが動く

	if (isPlayerNear && !hasAllItems)
	{
		//SoundManager::GetInstance().Play(SoundManager::SRC::CLOSED, Sound::TIMES::ONCE);
	}
	else
	{
		//SoundManager::GetInstance().Stop(SoundManager::SRC::CLOSED);
	}

}

void Door::Draw()
{
	MV1SetPosition(modelId_, pos_);
	MV1SetRotationXYZ(modelId_, rot_);
	MV1SetScale(modelId_, scale_);
	MV1DrawModel(modelId_);

	//DrawFormatString(0, 0, 0xFFFFFF, "pos:(%.2f,%.2f,%.2f)", pos_.x, pos_.y, pos_.z);

	//DrawFormatString(0, 0, 0xFFFFFF, "IsDown_=%d", isDown_);

	if (isPlayerNear && !hasAllItems)
	{
        // 画面中央にテキストを表示
		DrawString(Application::SCREEN_SIZE_X / 2-25, Application::SCREEN_SIZE_Y / 2, "まだ帰さない...", GetColor(255, 0, 0)); 
	}
}

void Door::Release()
{
	MV1DeleteModel(modelId_);


	
}

void Door::DownDoor()
{
	//設定された本物のアイテムの状態を使う
	if (item_ == nullptr || item_->AllGet() == false) return;
	// isDown_がtrueの場合（下降処理）
	if (isDown_ == true)
	{
		// 下降スピード分だけY座標を減らす
		pos_.y -= moveSpeed_;

		// ドアが設定した下降位置(34.00f)以下に到達したかチェック
		if (pos_.y < 34.00f)
		{
			// 下降位置を超えないように、Y座標を正確に設定する
			pos_.y = 34.00f;
			// isDown_は次のEキー入力までtrueのまま保持
		}
	}
	// isDown_がfalseの場合（上昇処理）
	else if (isDown_ == false)
	{
		// 上昇スピード分だけY座標を増やす
		pos_.y += moveSpeed_;

		if (pos_.y > 39.80f)
		{
			// 初期位置を超えないように、Y座標を正確に設定する
			pos_.y = 39.80f;
		}
	}

	MV1SetPosition(modelId_, pos_);
}

int Door::GetModelId()
{
	return modelId_;
}

bool Door::GetDown()
{
	return isDown_;
}
