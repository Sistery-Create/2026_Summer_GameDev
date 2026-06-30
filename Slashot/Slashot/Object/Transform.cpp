#include <DxLib.h>
#include "../Utility/AsoUtility.h"
#include "Transform.h"

Transform::Transform(void)
	:
	modelId(-1),
	scl(AsoUtility::VECTOR_ONE),
	rot(AsoUtility::VECTOR_ZERO),
	pos(AsoUtility::VECTOR_ZERO),
	localPos(AsoUtility::VECTOR_ZERO),
	matScl(MGetIdent()),
	matRot(MGetIdent()),
	matPos(MGetIdent()),
	quaRot(Quaternion()),
	quaRotLocal(Quaternion())

{
}

Transform::~Transform(void)
{
}

void Transform::Update(void)
{

    // スケール
    MATRIX matS = MGetScale(scl);

    // 回転
    Quaternion q = quaRot.Mult(quaRotLocal);
    MATRIX matR = q.ToMatrix();

    // 位置
    MATRIX matT = MGetTranslate(pos);

    // ローカル位置
    MATRIX matLocal = MGetTranslate(localPos);

    // 合成（順番超重要）
    MATRIX mat = MGetIdent();
    mat = MMult(mat, matLocal);
    mat = MMult(mat, matS);
    mat = MMult(mat, matR);
    mat = MMult(mat, matT);

    if (modelId != -1)
    {
        MV1SetMatrix(modelId, mat);
    }

}

void Transform::Release(void)
{
}

void Transform::SetModel(int model)
{
	modelId = model;
}

VECTOR Transform::GetForward(void) const
{
	return GetDir(AsoUtility::DIR_F);
}

VECTOR Transform::GetBack(void) const
{
	return GetDir(AsoUtility::DIR_B);
}

VECTOR Transform::GetRight(void) const
{
	return GetDir(AsoUtility::DIR_R);
}

VECTOR Transform::GetLeft(void) const
{
	return GetDir(AsoUtility::DIR_L);
}	

VECTOR Transform::GetUp(void) const
{
	return GetDir(AsoUtility::DIR_U);
}

VECTOR Transform::GetDown(void) const
{
	return GetDir(AsoUtility::DIR_D);
}

VECTOR Transform::GetDir(const VECTOR& dir) const
{
	return quaRot.PosAxis(dir);
}
