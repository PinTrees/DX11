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
	// 로컬 회전을 쿼터니언으로 변환
	Quaternion localRotation = Quaternion::CreateFromYawPitchRoll(m_LocalEulerAngles.y, m_LocalEulerAngles.x, m_LocalEulerAngles.z);

	// 로컬 변환 행렬 생성 
	Matrix S = Matrix::CreateScale(m_LocalScale);  
	Matrix R = Matrix::CreateFromQuaternion(localRotation);  
	Matrix T = Matrix::CreateTranslation(m_LocalPosition);

	m_LocalMatrix = S * R * T;

	if (HasParent())
	{
		m_WorldMatrix = m_LocalMatrix * _parent->GetWorldMatrix();
	}
	else
	{
		m_WorldMatrix = m_LocalMatrix;
	}

	m_WorldMatrix.Decompose(m_Scale, m_Rotation, m_Position); 
	m_EulerAngles = ToEulerAngles(m_Rotation); 

	// Children
	for (const shared_ptr<Transform>& child : _children) 
	{
		child->UpdateTransform(); 
	}
}

Vec3 Transform::GetLocalPosition()
{
	return m_LocalPosition;
}

void Transform::SetLocalPosition(const Vec3& localPosition)
{
	m_LocalPosition = localPosition;
	UpdateTransform();
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

void Transform::SetRotationQ(Quaternion q)
{
	m_Rotation = q;
	m_EulerAngles = ToEulerAngles(q);
	SetRotation(m_EulerAngles);
}

void Transform::SetLookRotation(Quaternion q)
{
	m_Rotation = q;  
	m_EulerAngles = ToEulerAngles(q);  
	SetRotation(m_EulerAngles); 
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

/* 월드 좌표계의 x, y, z 축 중 하나를 단위 벡터화하여 반환한다. */
Vec3 Transform::GetAxis(int index) const
{
	/* 입력값 검사 */
	if (index < 0 || index > 3)
	{
		return Vec3::Zero;
	}

	Vec3 result(
		m_WorldMatrix(index, 0),
		m_WorldMatrix(index, 1),
		m_WorldMatrix(index, 2)
	);
	result.Normalize();

	return result;
}

void Transform::OnInspectorGUI()
{
	ImGui::Text("Transform");

	bool positionChanged = false;
	bool rotationChanged = false;
	bool scaleChanged = false;

	ImGui::Text("Position"); 
	if (ImGui::DragFloat3("##WorldPosition", reinterpret_cast<float*>(&m_Position), 0.1f))
	{
		positionChanged = true;
	}
	if (ImGui::DragFloat3("##Position", reinterpret_cast<float*>(&m_LocalPosition), 0.1f))
	{
		positionChanged = true;
	}

	// Convert radians to degrees
	Vec3 angles;
	angles.x = XMConvertToDegrees(m_LocalEulerAngles.x); 
	angles.y = XMConvertToDegrees(m_LocalEulerAngles.y); 
	angles.z = XMConvertToDegrees(m_LocalEulerAngles.z); 

	ImGui::Text("Rotation");
	if (ImGui::DragFloat3("##Rotation", reinterpret_cast<float*>(&angles), 0.1f))
	{
		rotationChanged = true;
		Vec3 a;
		a.x = XMConvertToRadians(angles.x);
		a.y = XMConvertToRadians(angles.y);
		a.z = XMConvertToRadians(angles.z);
		SetLocalRotation(a);
	}

	ImGui::Text("Scale");
	scaleChanged = ImGui::DragFloat3("##Scale", reinterpret_cast<float*>(&m_LocalScale), 0.01f);

	if (positionChanged || rotationChanged || scaleChanged)
	{
		UpdateTransform();
	}
}

GENERATE_COMPONENT_FUNC_TOJSON(Transform)
{
	json j;
	j["type"] = "Transform";
	j["_localScale"] = { m_LocalScale.x, m_LocalScale.y, m_LocalScale.z };
	j["_localRotation"] = { m_LocalEulerAngles.x, m_LocalEulerAngles.y, m_LocalEulerAngles.z };
	j["_localPosition"] = { m_LocalPosition.x, m_LocalPosition.y, m_LocalPosition.z };
	j["_scale"] = { m_Scale.x, m_Scale.y, m_Scale.z };
	j["_rotation"] = { m_EulerAngles.x, m_EulerAngles.y, m_EulerAngles.z };
	j["_position"] = { m_Position.x, m_Position.y, m_Position.z };

	return j;
}

GENERATE_COMPONENT_FUNC_FROMJSON(Transform)
{
	m_LocalScale = { j.at("_localScale")[0].get<float>(), j.at("_localScale")[1].get<float>(), j.at("_localScale")[2].get<float>() };
	m_LocalEulerAngles = { j.at("_localRotation")[0].get<float>(), j.at("_localRotation")[1].get<float>(), j.at("_localRotation")[2].get<float>() };
	m_LocalPosition = { j.at("_localPosition")[0].get<float>(), j.at("_localPosition")[1].get<float>(), j.at("_localPosition")[2].get<float>() };
	m_Scale = { j.at("_scale")[0].get<float>(), j.at("_scale")[1].get<float>(), j.at("_scale")[2].get<float>() };
	m_EulerAngles = { j.at("_rotation")[0].get<float>(), j.at("_rotation")[1].get<float>(), j.at("_rotation")[2].get<float>() };
	m_Position = { j.at("_position")[0].get<float>(), j.at("_position")[1].get<float>(), j.at("_position")[2].get<float>() };

	UpdateTransform(); 
}
