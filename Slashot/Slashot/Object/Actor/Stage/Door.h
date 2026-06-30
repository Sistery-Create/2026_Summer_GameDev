#pragma once
#include<DxLib.h>
class Item;
class Door
{
public:
	Door();
	~Door();
	void Init();
	void Update(const VECTOR& playerPos);
	void Draw();
	void Release();
	void DownDoor();

	int GetModelId();

	bool GetDown();

	bool IsOperable() const { return isOperable_; }

	// GameSceneから本物のItemポインタを受け取る関数を追加
	void SetItem(Item* item) { item_ = item; }

private:
	Item* item_;
	int modelId_;
	float moveSpeed_;
	VECTOR pos_;
	VECTOR rot_;
	VECTOR scale_;
	bool isDown_;
	bool isOperable_;
	float openAngle_;
	bool isPlayerNear;
	bool hasAllItems;
};

