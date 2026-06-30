#pragma once
#include<Dxlib.h>
class NightDoom
{
public:
	NightDoom();
	~NightDoom();

    void Load(void);
    void Init(void);
    void Update(void);
    void Draw(void);
    void Release(void);
private:
    int modelId_;
    VECTOR scl_;
    VECTOR pos_;
    VECTOR rot_;
};

