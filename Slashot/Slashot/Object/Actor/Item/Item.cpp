#include "Item.h"
#include"../../../Application.h"
#include"../../../Manager/SoundManager.h"
Item::Item()
{
}

Item::~Item()
{
}

void Item::Load()
{
	modelId_ = MV1LoadModel("Data/Model/Item/Key.mv1");
	modelId2_ = MV1LoadModel("Data/Model/Item/RedKey.mv1");
	modelId3_ = MV1LoadModel("Data/Model/Item/BlueKey.mv1");
	modelId4_ = MV1LoadModel("Data/Model/Item/GreenKey.mv1");
	pos_ = KEY1_INIT_POS;
	pos2_ = KEY2_INIT_POS;
	pos3_ = KEY3_INIT_POS;
	pos4_ = KEY4_INIT_POS;
	rot_ = VGet(0.0f, 0.0f, 0.0f);
	scale_ = KEY_SCALE;
	isGotten_1 = false;
	isGotten_2 = false;
	isGotten_3 = false;
	isGotten_4 = false;

	// ライト用設定
	COLOR_F lightColor = LIGHT_COLOR;
	float range = LIGHT_RANGE;
	float atten0 = LIGHT_ATTEN0;
	float atten1 = LIGHT_ATTEN1;
	float atten2 = LIGHT_ATTEN2;

	// 鍵1用のライトを作成
	lightHandle1_ = CreatePointLightHandle(pos_, range, atten0, atten1, atten2);
	SetLightDifColorHandle(lightHandle1_, lightColor);
	SetLightAmbColorHandle(lightHandle1_, lightColor);

	// 鍵2
	lightHandle2_ = CreatePointLightHandle(pos2_, range, atten0, atten1, atten2);
	SetLightDifColorHandle(lightHandle2_, lightColor);
	SetLightAmbColorHandle(lightHandle2_, lightColor);

	// 鍵3
	lightHandle3_ = CreatePointLightHandle(pos3_, range, atten0, atten1, atten2);
	SetLightDifColorHandle(lightHandle3_, lightColor);
	SetLightAmbColorHandle(lightHandle3_, lightColor);

	// 鍵4
	lightHandle4_ = CreatePointLightHandle(pos4_, range, atten0, atten1, atten2);
	SetLightDifColorHandle(lightHandle4_, lightColor);
	SetLightAmbColorHandle(lightHandle4_, lightColor);


	MV1SetPosition(modelId_, pos_);
	MV1SetPosition(modelId2_, pos2_);
	MV1SetPosition(modelId3_, pos3_);
	MV1SetPosition(modelId4_, pos4_);
	MV1SetRotationXYZ(modelId_, rot_);
	MV1SetRotationXYZ(modelId2_, rot_);
	MV1SetRotationXYZ(modelId3_, rot_);
	MV1SetRotationXYZ(modelId4_, rot_);
	MV1SetScale(modelId_,KEY_SCALE);
	MV1SetScale(modelId2_,KEY_SCALE);
	MV1SetScale(modelId3_,KEY_SCALE);
	MV1SetScale(modelId4_,KEY_SCALE);


	keyIcon1_ = LoadGraph("Data/Image/Item/Keyicon.png");
	keyIcon2_ = LoadGraph("Data/Image/Item/Keyicon2.png");
	keyIcon3_ = LoadGraph("Data/Image/Item/Keyicon3.png");
	keyIcon4_ = LoadGraph("Data/Image/Item/GreenKey.png");

	allGet_ = false;


}

void Item::Init()
{
	Load();
}

void Item::Update()
{

	rot_.y +=KEY_ROTATION_SPEED;
	// --- 鍵をすべて集めたかチェックするロジックの追加 ---
	if (isGotten_1 && isGotten_2 && isGotten_3 && isGotten_4)
	{
		allGet_ = true;
	}
	else
	{
		allGet_ = false;
	}

	// 鍵が取得されたら、ライトを無効にする (ライトを消す)
	SetLightEnableHandle(lightHandle1_, !isGotten_1);
	SetLightEnableHandle(lightHandle2_, !isGotten_2);
	SetLightEnableHandle(lightHandle3_, !isGotten_3);
	SetLightEnableHandle(lightHandle4_, !isGotten_4);


	auto& sm = SoundManager::GetInstance();




	if (isGotten_1 && !isSoundPlayed_)
	{
		

		isSoundPlayed_ = true;
	}
	if (isGotten_2 && !isSoundPlayed_2)
	{
		

		isSoundPlayed_2 = true; 
	}
	if (isGotten_3 && !isSoundPlayed_3)
	{
		

		isSoundPlayed_3 = true;
	}
	if (isGotten_4 && !isSoundPlayed_4)
	{

		isSoundPlayed_4 = true;
	}
	MV1SetRotationXYZ(modelId_, rot_);
	MV1SetRotationXYZ(modelId2_, rot_);
	MV1SetRotationXYZ(modelId3_, rot_);
	MV1SetRotationXYZ(modelId4_, rot_);
}

void Item::Draw()
{
	// アイコンと枠の基本設定に定数を使用
	int frameWidth = FRAME_WIDTH;
	int frameHeight = FRAME_HEIGHT;
	int frameXStart = FRAME_X_START;
	int frameY = FRAME_Y_START;
	int spacing = FRAME_SPACING;
	int iconPadding = ICON_PADDING;
	int iconSize = ICON_SIZE;
	unsigned int frameColor = FRAME_COLOR;


	//1つ目
	int frame1X = frameXStart;
	int icon1X = frame1X + 10; 
	int iconY = frameY + 10;   

	if (!isGotten_1)
	{
		MV1DrawModel(modelId_);
	}
	else
	{
		// 枠を描く
		DrawBox(frame1X, frameY, frame1X + frameWidth, frameY + frameHeight, frameColor, FALSE);
		// 枠の内側にアイコン画像を描画
		DrawExtendGraph(icon1X, iconY, icon1X + iconSize, iconY + iconSize, keyIcon3_, TRUE);
		
	}

	//2つ目
	int frame2X = frame1X + frameWidth + spacing;
	int icon2X = frame2X + 10;

	if (!isGotten_2)
	{
		MV1DrawModel(modelId2_);
	}
	else
	{
		// 枠を描く
		DrawBox(frame2X, frameY, frame2X + frameWidth, frameY + frameHeight, frameColor, FALSE);
		// 枠の内側にアイコン画像を描画
		DrawExtendGraph(icon2X, iconY, icon2X + iconSize, iconY + iconSize, keyIcon1_, TRUE);
		
	}

	// 3つ目
	int frame3X = frame2X + frameWidth + spacing;
	int icon3X = frame3X + 10;

	if (!isGotten_3)
	{
		MV1DrawModel(modelId3_);
	}
	else
	{
		// 枠を描く
		DrawBox(frame3X, frameY, frame3X + frameWidth, frameY + frameHeight, frameColor, FALSE);
		// 枠の内側にアイコン画像を描画
		DrawExtendGraph(icon3X, iconY, icon3X + iconSize, iconY + iconSize, keyIcon2_, TRUE);
		
	}

	//4つ目
	int frame4X = frame3X + frameWidth + spacing;
	int icon4X = frame4X + 10;

	if (!isGotten_4)
	{
		MV1DrawModel(modelId4_);
	}
	else
	{
		// 枠を描く
		DrawBox(frame4X, frameY, frame4X + frameWidth, frameY + frameHeight, frameColor, FALSE);
		// 枠の内側にアイコン画像を描画
		DrawExtendGraph(icon4X, iconY, icon4X + iconSize, iconY + iconSize, keyIcon4_, TRUE);
		
	}

	
	
}

void Item::Debag()
{
	
}

void Item::Release()
{
	MV1DeleteModel(modelId_);
	MV1DeleteModel(modelId2_);
	MV1DeleteModel(modelId3_);

	DeleteLightHandle(lightHandle1_);
	DeleteLightHandle(lightHandle2_);
	DeleteLightHandle(lightHandle3_);
	DeleteLightHandle(lightHandle4_);
}

bool Item::GetItem()
{
	return isGotten_1;
}

void Item::ItemGet()
{
	isGotten_1 = true;
}

bool Item::AllGet()
{
	
	return allGet_;
}

bool Item::GetIsGet() const
{
	return isGet;
}

void Item::SetIsGet(bool get_)
{
	isGet = get_;
}
