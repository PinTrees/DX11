#include "pch.h"
#include "EditorCamera.h"


EditorCamera::EditorCamera()
{
	SetLens(0.25f * XM_PI, 1.0f, 1.0f, 1000.0f);
}

EditorCamera::~EditorCamera()
{
}

XMVECTOR EditorCamera::GetPositionXM()const
{
	return ::XMLoadFloat3(&_position);
}

XMFLOAT3 EditorCamera::GetPosition()const
{
	return _position;
}

void EditorCamera::SetPosition(float x, float y, float z)
{
	_position = XMFLOAT3(x, y, z);
}

void EditorCamera::SetPosition(const XMFLOAT3& v)
{
	_position = v;
}

XMVECTOR EditorCamera::GetRightXM()const
{
	return ::XMLoadFloat3(&_right);
}

XMFLOAT3 EditorCamera::GetRight()const
{
	return _right;
}

XMVECTOR EditorCamera::GetUpXM()const
{
	return ::XMLoadFloat3(&_up);
}

XMFLOAT3 EditorCamera::GetUp()const
{
	return _up;
}

XMVECTOR EditorCamera::GetLookXM()const
{
	return ::XMLoadFloat3(&_look);
}

XMFLOAT3 EditorCamera::GetLook()const
{
	return _look;
}

float EditorCamera::GetNearZ()const
{
	return _nearZ;
}

float EditorCamera::GetFarZ()const
{
	return _farZ;
}

float EditorCamera::GetAspect()const
{
	return _aspect;
}

float EditorCamera::GetFovY()const
{
	return _fovY;
}

float EditorCamera::GetFovX()const
{
	float halfWidth = 0.5f * GetNearWindowWidth();
	return 2.0f * atan(halfWidth / _nearZ);
}

float EditorCamera::GetNearWindowWidth()const
{
	return _aspect * _nearWindowHeight;
}

float EditorCamera::GetNearWindowHeight()const
{
	return _nearWindowHeight;
}

float EditorCamera::GetFarWindowWidth()const
{
	return _aspect * _farWindowHeight;
}

float EditorCamera::GetFarWindowHeight()const
{
	return _farWindowHeight;
}

void EditorCamera::SetLens(float fovY, float aspect, float zn, float zf)
{
	// cache properties
	_fovY = fovY;
	_aspect = aspect;
	_nearZ = zn;
	_farZ = zf;

	_nearWindowHeight = 2.0f * _nearZ * tanf(0.5f * _fovY);
	_farWindowHeight = 2.0f * _farZ * tanf(0.5f * _fovY);

	XMMATRIX P = ::XMMatrixPerspectiveFovLH(_fovY, _aspect, _nearZ, _farZ);
	::XMStoreFloat4x4(&_proj, P);
}

void EditorCamera::LookAt(FXMVECTOR pos, FXMVECTOR target, FXMVECTOR worldUp)
{
	XMVECTOR L = ::XMVector3Normalize(::XMVectorSubtract(target, pos));
	XMVECTOR R = ::XMVector3Normalize(::XMVector3Cross(worldUp, L));
	XMVECTOR U = ::XMVector3Cross(L, R);

	::XMStoreFloat3(&_position, pos);
	::XMStoreFloat3(&_look, L);
	::XMStoreFloat3(&_right, R);
	::XMStoreFloat3(&_up, U);
}

void EditorCamera::LookAt(const XMFLOAT3& pos, const XMFLOAT3& target, const XMFLOAT3& up)
{
	XMVECTOR P = ::XMLoadFloat3(&pos);
	XMVECTOR T = ::XMLoadFloat3(&target);
	XMVECTOR U = ::XMLoadFloat3(&up);

	LookAt(P, T, U);
}

XMMATRIX EditorCamera::View()const
{
	return ::XMLoadFloat4x4(&_view);
}

XMMATRIX EditorCamera::Proj()const
{
	return ::XMLoadFloat4x4(&_proj);
}

XMMATRIX EditorCamera::ViewProj()const
{
	return ::XMMatrixMultiply(View(), Proj());
}

void EditorCamera::Strafe(float d)
{
	// mPosition += d*mRight
	XMVECTOR s = ::XMVectorReplicate(d);
	XMVECTOR r = ::XMLoadFloat3(&_right);
	XMVECTOR p = ::XMLoadFloat3(&_position);
	::XMStoreFloat3(&_position, XMVectorMultiplyAdd(s, r, p));
}

void EditorCamera::Walk(float d)
{
	// mPosition += d*mLook
	XMVECTOR s = ::XMVectorReplicate(d);
	XMVECTOR l = ::XMLoadFloat3(&_look);
	XMVECTOR p = ::XMLoadFloat3(&_position);
	::XMStoreFloat3(&_position, ::XMVectorMultiplyAdd(s, l, p));
}

void EditorCamera::Pitch(float angle)
{
	// Rotate up and look vector about the right vector.

	XMMATRIX R = ::XMMatrixRotationAxis(::XMLoadFloat3(&_right), angle);

	::XMStoreFloat3(&_up, ::XMVector3TransformNormal(XMLoadFloat3(&_up), R));
	::XMStoreFloat3(&_look, ::XMVector3TransformNormal(XMLoadFloat3(&_look), R));
}

void EditorCamera::RotateY(float angle)
{
	// Rotate the basis vectors about the world y-axis.

	XMMATRIX R = XMMatrixRotationY(angle);

	::XMStoreFloat3(&_right, ::XMVector3TransformNormal(::XMLoadFloat3(&_right), R));
	::XMStoreFloat3(&_up, ::XMVector3TransformNormal(::XMLoadFloat3(&_up), R));
	::XMStoreFloat3(&_look, ::XMVector3TransformNormal(::XMLoadFloat3(&_look), R));
}

void EditorCamera::UpdateViewMatrix()
{
	XMVECTOR R = ::XMLoadFloat3(&_right);
	XMVECTOR U = ::XMLoadFloat3(&_up);
	XMVECTOR L = ::XMLoadFloat3(&_look);
	XMVECTOR P = ::XMLoadFloat3(&_position);

	// Keep camera's axes orthogonal to each other and of unit length.
	L = ::XMVector3Normalize(L);
	U = ::XMVector3Normalize(::XMVector3Cross(L, R));

	// U, L already ortho-normal, so no need to normalize cross product.
	R = ::XMVector3Cross(U, L);

	// Fill in the view matrix entries.
	float x = -::XMVectorGetX(::XMVector3Dot(P, R));
	float y = -::XMVectorGetX(::XMVector3Dot(P, U));
	float z = -::XMVectorGetX(::XMVector3Dot(P, L));

	::XMStoreFloat3(&_right, R);
	::XMStoreFloat3(&_up, U);
	::XMStoreFloat3(&_look, L);

	_view(0, 0) = _right.x;
	_view(1, 0) = _right.y;
	_view(2, 0) = _right.z;
	_view(3, 0) = x;

	_view(0, 1) = _up.x;
	_view(1, 1) = _up.y;
	_view(2, 1) = _up.z;
	_view(3, 1) = y;

	_view(0, 2) = _look.x;
	_view(1, 2) = _look.y;
	_view(2, 2) = _look.z;
	_view(3, 2) = z;

	_view(0, 3) = 0.0f;
	_view(1, 3) = 0.0f;
	_view(2, 3) = 0.0f;
	_view(3, 3) = 1.0f;
}

