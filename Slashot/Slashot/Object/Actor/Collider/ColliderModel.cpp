#include "ColliderModel.h"
#include"../../Transform.h"
#include"../Stage/Stage.h"
ColliderModel::ColliderModel(TAG tag, const Transform* follow)
	:
	ColliderBase(SHAPE::MODEL, tag, follow)
{
}
ColliderModel::~ColliderModel(void)
{

}

void ColliderModel::AddExcludeFrameIds(const std::string& name)
{
	// フレーム数を取得
	int num = MV1GetFrameNum(follow_->modelId);
	for (int i = 0; i < num; i++)
	{
		// フレーム名称を取得
		std::string frameName = MV1GetFrameName(follow_->modelId, i);
		if (frameName.find(name) != std::string::npos)
		{
			// 除外フレームに追加
			excludeFrameIds_.emplace_back(i);
		}
	}
}

void ColliderModel::ClearExcludeFrame(void)
{
	excludeFrameIds_.clear();
}

bool ColliderModel::IsExcludeFrame(int frameIdx) const
{
	// 除外判定
	if (std::find(
		excludeFrameIds_.begin(),
		excludeFrameIds_.end(),
		frameIdx) != excludeFrameIds_.end())
	{
		// 除外に該当する
		return true;
	}
	return false;
}
