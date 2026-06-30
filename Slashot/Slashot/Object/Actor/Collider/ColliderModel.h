#pragma once
#include "ColliderBase.h"
#include<vector>
#include<string>
class Stage;
class ColliderModel : public ColliderBase
{
public:
	// コンストラクタ
	ColliderModel(TAG tag, const Transform* follow);
	// デストラクタ
	~ColliderModel(void) override;
	// 指定された文字を含むフレームを衝突判定から除外
	void AddExcludeFrameIds(const std::string& name);
	// 衝突判定から除外するフレームをクリアする
	void ClearExcludeFrame(void);
	// 除外フレーム判定
	bool IsExcludeFrame(int frameIdx) const;
protected:
	// 衝突判定から除外するフレーム番号
	std::vector<int> excludeFrameIds_;
	// デバッグ用描画
	void DrawDebug(int color) override {};
	Stage* stage_;
};