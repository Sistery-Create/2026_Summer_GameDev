#include "ColliderSphere.h"

ColliderSphere::ColliderSphere(TAG tag, const Transform* follow, const VECTOR& localPos, float radius)
	:
	ColliderBase(SHAPE::SPHERE, tag, follow),
	localPos_(localPos),
	radius_(radius)
{
}
ColliderSphere::~ColliderSphere(void)
{
}

const VECTOR& ColliderSphere::GetLocalPos(void) const
{
	return localPos_;
}

void ColliderSphere::SetLocalPos(const VECTOR& localPos)
{
	localPos_ = localPos;
}

VECTOR ColliderSphere::GetPos(void) const
{
	return GetRotPos(localPos_);
}

float ColliderSphere::GetRadius(void) const
{
	return radius_;
}

void ColliderSphere::SetRadius(float radius)
{
	radius_ = radius;
}

void ColliderSphere::DrawDebug(int color)
{
	DrawSphere3D(GetPos(), radius_, 16, color, color, false);
}
