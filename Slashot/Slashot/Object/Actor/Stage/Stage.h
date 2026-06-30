#pragma once
#include<Dxlib.h>
#include"../ActorBase.h"
#include<vector>
#include<string>
class Stage :public ActorBase
{
public:
	// Õ“Ë”»’èí•Ê
	enum class COLLIDER_TYPE
	{
		MODEL = 0,
		MAX,
	};
	Stage();
	~Stage();
	void Init();
	void Update()override;
	void Draw()override;
	int GetModelId(void);
private:
	void InitLoad()override;
	void InitTransform()override;
	void InitCollider(void)override;
	void InitAnimation()override;
	void InitPost()override;
private:

	int modelId;
	VECTOR pos_;
	VECTOR scl_;

	// œŠOƒtƒŒ[ƒ€–¼Ì
	const std::vector<std::string> EXCLUDE_FRAME_NAMES = {
	"Mush", "Grass",
	};

};

