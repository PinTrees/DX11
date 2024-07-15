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
	// 로컬 변환 행렬 생성 
	Matrix S = Matrix::CreateScale(m_LocalScale);  
	Matrix R = Matrix::CreateRotationX(m_LocalEulerAngles.x)
			 * Matrix::CreateRotationY(m_LocalEulerAngles.y)
			 * Matrix::CreateRotationZ(m_LocalEulerAngles.z);
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
		Vec3 scale = worldScale / parentScale; 
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
		Vec3 rotation = Vec3::TransformNormal(worldRotation, inverseMatrix);
		 
		SetLocalRotation(rotation);
	}
	else
	{
		SetLocalRotation(worldRotation); 
	}
}

void Transform::SetRotationQ(Quaternion q)
{
	m_Rotation = q;
	m_EulerAngles = ToEulerAngles(q);
	SetRotation(m_EulerAngles);
}

void Transform::SetLookRotation(Quaternion targetRotation) 
{
	m_Rotation = targetRotation;
	m_EulerAngles = ToEulerAngles(targetRotation);
	SetRotation(m_EulerAngles);
}

void Transform::SetPosition(const Vec3& worldPosition)
{
	if (HasParent())
	{
		Matrix worldToParentLocalMatrix = _parent->GetWorldMatrix().Invert();
		Vec3 position = Vec3::Transform(worldPosition, worldToParentLocalMatrix);

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
	// 입력값 검사
	if (index < 0 || index > 2)  // 3x3 또는 4x4 행렬의 유효한 축 인덱스는 0, 1, 2
	{
		std::cout << "RigidBody::getAxis::Out of index" << std::endl;
		return Vector3();  
	}

	// 월드 매트릭스에서 축 벡터 추출
	Vector3 axis(
		m_WorldMatrix(index, 0),
		m_WorldMatrix(index, 1),
		m_WorldMatrix(index, 2)
	);

	axis.Normalize();  // 벡터를 정규화

	return axis;
}

void Transform::OnInspectorGUI()
{
	bool positionChanged = false;
	bool rotationChanged = false;
	bool scaleChanged = false;

	ImGui::Text("World Position"); 
	if (ImGui::DragFloat3("##WorldPosition", reinterpret_cast<float*>(&m_Position), 0.1f))
	{
		positionChanged = true;
	}
	ImGui::Text("Local Position");
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
	m_LocalScale = SafeGetVector3(j, "_localScale", { 1.0f, 1.0f, 1.0f }); 
	m_LocalEulerAngles = SafeGetVector3(j, "_localRotation", { 0.0f, 0.0f, 0.0f }); 
	m_LocalPosition = SafeGetVector3(j, "_localPosition", { 0.0f, 0.0f, 0.0f }); 
	m_Scale = SafeGetVector3(j, "_scale", { 1.0f, 1.0f, 1.0f }); 
	m_EulerAngles = SafeGetVector3(j, "_rotation", { 0.0f, 0.0f, 0.0f }); 
	m_Position = SafeGetVector3(j, "_position", { 0.0f, 0.0f, 0.0f }); 
	 
	UpdateTransform(); 
}
