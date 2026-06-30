#include <DxLib.h>
#include "../../Transform.h"
#include "ColliderCapsule.h"
ColliderCapsule::ColliderCapsule(
	TAG tag, const Transform* follow,
	const VECTOR& localPosTop, const VECTOR& localPosDown, float radius)
	:
	ColliderBase(SHAPE::CAPSULE, tag, follow),
	localPosTop_(localPosTop),
	localPosDown_(localPosDown),
	radius_(radius)
{
}
ColliderCapsule::~ColliderCapsule(void)
{
}
const VECTOR& ColliderCapsule::GetLocalPosTop(void) const
{
	return localPosTop_;
}
const VECTOR& ColliderCapsule::GetLocalPosDown(void) const
{
	return localPosDown_;
}
void ColliderCapsule::SetLocalPosTop(const VECTOR& pos)
{
	localPosTop_ = pos;
}
void ColliderCapsule::SetLocalPosDown(const VECTOR& pos)
{
	localPosDown_ = pos;
}
VECTOR ColliderCapsule::GetPosTop(void) const
{
    if (useWorldPos_)
    {
        return worldTop_;
    }

    return GetRotPos(localPosTop_);
}
VECTOR ColliderCapsule::GetPosDown(void) const
{
    if (useWorldPos_)
    {
        return worldDown_;
    }

    return GetRotPos(localPosDown_);
}
VECTOR ColliderCapsule::GetWeaponPosTop(void) const
{
    return VAdd(follow_->pos, localPosTop_);
}
VECTOR ColliderCapsule::GetWeaponPosDown(void) const
{
    return VAdd(follow_->pos, localPosDown_);
}
float ColliderCapsule::GetRadius(void) const
{
	return radius_;
}
void ColliderCapsule::SetRadius(float radius)
{
	radius_ = radius;
}
float ColliderCapsule::GetHeight(void) const
{
	return localPosTop_.y;
}
VECTOR ColliderCapsule::GetCenter(void) const
{
	VECTOR top = GetPosTop();
	VECTOR down = GetPosDown();
	VECTOR diff = VSub(top, down);
	return VAdd(down, VScale(diff, 0.5f));
}
void ColliderCapsule::DrawDebug(int color)
{
    VECTOR top = GetPosTop();
    VECTOR down = GetPosDown();

    // óºí[ãÖ
    DrawSphere3D(
        top,
        radius_,
        8,
        color,
        color,
        false
    );

    DrawSphere3D(
        down,
        radius_,
        8,
        color,
        color,
        false
    );

    // íÜâõê¸
    DrawLine3D(
        top,
        down,
        color
    );

    // íÜêS
    VECTOR center =
        VScale(VAdd(top, down), 0.5f);

    DrawSphere3D(
        center,
        3.0f,
        8,
        color,
        color,
        true
    );
}