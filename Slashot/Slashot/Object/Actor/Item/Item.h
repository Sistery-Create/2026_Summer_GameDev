#pragma once


#include<DxLib.h>
class Item
{
public:
	//定数定義
	//デフォルトスケール
	const VECTOR KEY_SCALE = { 0.01f, 0.01f, 0.01f };

	// 鍵の初期座標
	const VECTOR KEY1_INIT_POS = { -113.87f, 42.73f, -20.26f };
	const VECTOR KEY2_INIT_POS = { -42.69f, 42.0f, 18.55f };
	const VECTOR KEY3_INIT_POS = { -54.25f, 42.73f, -43.41f };
	const VECTOR KEY4_INIT_POS = { -80.00f, 41.10f, -10.0f };

	// 回転速度
	const float KEY_ROTATION_SPEED = 0.02f;

	// ライトの設定
	const COLOR_F LIGHT_COLOR = { 1.0f, 1.0f, 1.0f };
	const float LIGHT_RANGE = 2.5f;
	const float LIGHT_ATTEN0 = 0.0f;
	const float LIGHT_ATTEN1 = 0.5f;
	const float LIGHT_ATTEN2 = 0.0f;

	// UI描画設定
	const int FRAME_WIDTH = 120;        // 枠の横幅
	const int FRAME_HEIGHT = 120;       // 枠の縦幅
	const int FRAME_X_START = 20;       // 最初の枠の左端からの余白
	const int FRAME_Y_START = 20;       // 上端からの余白
	const int FRAME_SPACING = 10;       // 枠と枠の間隔
	const int ICON_PADDING = 10;        // 枠とアイコンの間の余白
	const int ICON_SIZE = FRAME_WIDTH - (ICON_PADDING * 2); // アイコンのサイズ (100)
	const unsigned int FRAME_COLOR = 0xFFFFFF; // 枠線の色（白: GetColor(255, 255, 255)）


	Item();
	~Item();
	void Load();
	void Init();
	void Update();
	void Draw();
	void Debag();
	void Release();

	bool GetItem();
	void ItemGet();
	
	bool IsGotten() const { return isGotten_1; }
	void SetGotten(bool flag) { isGotten_1 = flag; }
	int GetModelId() const { return modelId_; }

	bool IsGotten2() const { return isGotten_2; }
	void SetGotten2(bool flag) { isGotten_2 = flag; }
	int GetModelId2() const { return modelId2_; }

	bool IsGotten3() const { return isGotten_3; }
	void SetGotten3(bool flag) { isGotten_3 = flag; }
	int GetModelId3() const { return modelId3_; }

	bool IsGotten4() const { return isGotten_4; }
	void SetGotten4(bool flag) { isGotten_4 = flag; }
	int GetModelId4() const { return modelId4_; }

	bool AllGet();

	bool GetIsGet() const;

	void SetIsGet(bool get_);
	

private:
	int modelId_;
	int modelId2_;
	int modelId3_;
	int modelId4_;

	VECTOR pos_;
	VECTOR pos2_;
	VECTOR pos3_;
	VECTOR pos4_;
	VECTOR rot_;
	bool isGotten_1;
	bool isGotten_2;
	bool isGotten_3;
	bool isGotten_4;

	bool allGet_;

	int keyIcon1_;
	int keyIcon2_;
	int keyIcon3_;
	int keyIcon4_;

	VECTOR scale_;

	
	bool isSoundPlayed_= false; 
	bool isSoundPlayed_2 = false;
	bool isSoundPlayed_3 = false;
	bool isSoundPlayed_4 = false;

	int keyCount_;

	int lightHandle1_; // 鍵1用のライトハンドル
	int lightHandle2_; // 鍵2用のライトハンドル
	int lightHandle3_; // 鍵3用のライトハンドル
	int lightHandle4_; // 鍵4用のライトハンドル

	bool isGet = false;
};

