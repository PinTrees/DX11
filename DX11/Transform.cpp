#include "pch.h"
#include "Transform.h"


Transform::Transform()
{
	m_InspectorTitleName = "Transform";
}

Transform::~Transform()
{

}

void Transform::Awake()
{
}

void Transform::Update()
{
}

Vec3 Transform::ToEulerAngles(Quaternion q)
{
	Vec3 angles;

	// roll (x-axis rotation)
	double sinr_cosp = 2 * (q.w * q.x + q.y * q.z);
	double cosr_cosp = 1 - 2 * (q.x * q.x + q.y * q.y);
	angles.x = std::atan2(sinr_cosp, cosr_cosp);

	// pitch (y-axis rotation)
	double sinp = std::sqrt(1 + 2 * (q.w * q.y - q.x * q.z));
	double cosp = std::sqrt(1 - 2 * (q.w * q.y - q.x * q.z));
	angles.y = 2 * std::atan2(sinp, cosp) - 3.14159f / 2;

	// yaw (z-axis rotation)
	double siny_cosp = 2 * (q.w * q.z + q.x * q.y);
	double cosy_cosp = 1 - 2 * (q.y * q.y + q.z * q.z);
	angles.z = std::atan2(siny_cosp, cosy_cosp);

	return angles;
}

void Transform::UpdateTransform()
{
	Matrix matScale = Matrix::CreateScale(_localScale);
	Matrix matRotation = Matrix::CreateRotationX(_localRotation.x);
	matRotation *= Matrix::CreateRotationY(_localRotation.y);
	matRotation *= Matrix::CreateRotationZ(_localRotation.z);
	Matrix matTranslation = Matrix::CreateTranslation(_localPosition);

	_matLocal = matScale * matRotation * matTranslation;

	if (HasParent())
	{
		_matWorld = _matLocal * _parent->GetWorldMatrix();
	}
	else
	{
		_matWorld = _matLocal;
	}

	Quaternion quat;
	_matWorld.Decompose(_scale, quat, _position);
	_rotation = ToEulerAngles(quat);

	// Children
	for (const shared_ptr<Transform>& child : _children)
		child->UpdateTransform();
}

void Transform::SetScale(const Vec3& worldScale)
{
	if (HasParent())
	{
		Vec3 parentScale = _parent->GetScale();
		Vec3 scale = worldScale;
		scale.x /= parentScale.x;
		scale.y /= parentScale.y;
		scale.z /= parentScale.z;
		SetLocalScale(scale);
	}
	else
	{
		SetLocalScale(worldScale);
	}
}

void Transform::SetRotation(const Vec3& worldRotation)
{
	if (HasParent())
	{
		Matrix inverseMatrix = _parent->GetWorldMatrix().Invert();

		Vec3 rotation;
		rotation.TransformNormal(worldRotation, inverseMatrix);

		SetLocalRotation(rotation);
	}
	else
		SetLocalRotation(worldRotation);
}

void Transform::SetPosition(const Vec3& worldPosition)
{
	if (HasParent())
	{
		Matrix worldToParentLocalMatrix = _parent->GetWorldMatrix().Invert();

		Vec3 position;
		position.Transform(worldPosition, worldToParentLocalMatrix);

		SetLocalPosition(position);
	}
	else
	{
		SetLocalPosition(worldPosition);
	}
}

void Transform::OnInspectorGUI()
{
	ImGui::Text("Transform");

	bool positionChanged = false;
	bool rotationChanged = false;
	bool scaleChanged = false;

	ImGui::Text("Position");
	positionChanged = ImGui::DragFloat3("##Position", reinterpret_cast<float*>(&_localPosition), 0.1f);

	ImGui::Text("Rotation");
	rotationChanged = ImGui::DragFloat3("##Rotation", reinterpret_cast<float*>(&_localRotation), 0.1f);

	ImGui::Text("Scale");
	scaleChanged = ImGui::DragFloat3("##Scale", reinterpret_cast<float*>(&_localScale), 0.1f);

	if (positionChanged || rotationChanged || scaleChanged)
	{
		UpdateTransform();
	}
}

GENERATE_COMPONENT_FUNC_TOJSON(Transform)
{
	json j;
	j["type"] = "Transform";
	j["_localScale"] = { _localScale.x, _localScale.y, _localScale.z };
	j["_localRotation"] = { _localRotation.x, _localRotation.y, _localRotation.z };
	j["_localPosition"] = { _localPosition.x, _localPosition.y, _localPosition.z };
	j["_scale"] = { _scale.x, _scale.y, _scale.z };
	j["_rotation"] = { _rotation.x, _rotation.y, _rotation.z };
	j["_position"] = { _position.x, _position.y, _position.z };

	return j;
}

GENERATE_COMPONENT_FUNC_FROMJSON(Transform)
{
	_localScale = { j.at("_localScale")[0].get<float>(), j.at("_localScale")[1].get<float>(), j.at("_localScale")[2].get<float>() };
	_localRotation = { j.at("_localRotation")[0].get<float>(), j.at("_localRotation")[1].get<float>(), j.at("_localRotation")[2].get<float>() };
	_localPosition = { j.at("_localPosition")[0].get<float>(), j.at("_localPosition")[1].get<float>(), j.at("_localPosition")[2].get<float>() };
	_scale = { j.at("_scale")[0].get<float>(), j.at("_scale")[1].get<float>(), j.at("_scale")[2].get<float>() };
	_rotation = { j.at("_rotation")[0].get<float>(), j.at("_rotation")[1].get<float>(), j.at("_rotation")[2].get<float>() };
	_position = { j.at("_position")[0].get<float>(), j.at("_position")[1].get<float>(), j.at("_position")[2].get<float>() };

	UpdateTransform(); 
}
