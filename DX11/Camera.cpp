#include "pch.h"
#include "Camera.h"
#include "Application.h"
#include "App.h"
#include "GameObject.h"
#include "Mesh.h"
#include "MeshRenderer.h"
#include "SceneManager.h"
#include "Scene.h"
#include "Light.h"
#include "LightManager.h"
#include "EditorGUI.h"

Camera::Camera()
{
	m_InspectorTitleName = "Camera";
	SetLens(0.25f * XM_PI, 1.0f, 1.0f, 1000.0f);
}

Camera::~Camera()
{
}

XMVECTOR Camera::GetPositionXM()
{
	auto position = GetGameObject()->GetTransform()->GetPosition(); 
	return ::XMLoadFloat3(&position);
}

XMFLOAT3 Camera::GetPosition()const
{
	Transform* transform = m_pGameObject->GetComponent<Transform>();
	return transform->GetPosition(); 
}

XMVECTOR Camera::GetRightXM()const 
{
	return ::XMLoadFloat3(&_right);
} 

XMFLOAT3 Camera::GetRight()const
{
	return _right;
}

XMVECTOR Camera::GetUpXM()const
{
	return ::XMLoadFloat3(&_up);
}

XMFLOAT3 Camera::GetUp()const
{
	return _up;
}

XMVECTOR Camera::GetLookXM()const
{
	return ::XMLoadFloat3(&_look);
}

XMFLOAT3 Camera::GetLook()const
{
	return _look;
}

float Camera::GetNearZ()const
{
	return m_nearZ;
}

float Camera::GetFarZ()const
{
	return m_farZ;
}

float Camera::GetAspect()const
{
	return m_aspect;
}

float Camera::GetFovY()const
{
	return m_fovY;
}

float Camera::GetFovX()const
{
	float halfWidth = 0.5f * GetNearWindowWidth();
	return 2.0f * atan(halfWidth / m_nearZ);
}

float Camera::GetNearWindowWidth()const
{
	return m_aspect * m_nearWindowHeight;
}

float Camera::GetNearWindowHeight()const
{
	return m_nearWindowHeight;
}

float Camera::GetFarWindowWidth()const
{
	return m_aspect * m_farWindowHeight;
}

float Camera::GetFarWindowHeight()const
{
	return m_farWindowHeight;
}

void Camera::SetLens(float fovY, float aspect, float zn, float zf)
{
	// cache properties
	m_fovY = fovY;
	m_aspect = aspect;
	m_nearZ = zn;
	m_farZ = zf;

	ProjUpdate();
}


void Camera::LookAt(FXMVECTOR pos, FXMVECTOR target, FXMVECTOR worldUp)
{
	XMVECTOR L = ::XMVector3Normalize(::XMVectorSubtract(target, pos));
	XMVECTOR R = ::XMVector3Normalize(::XMVector3Cross(worldUp, L));
	XMVECTOR U = ::XMVector3Cross(L, R);

	auto position = GetGameObject()->GetTransform()->GetPosition();

	::XMStoreFloat3(&position, pos); 
	::XMStoreFloat3(&_look, L);
	::XMStoreFloat3(&_right, R);
	::XMStoreFloat3(&_up, U);
}

void Camera::LookAt(const XMFLOAT3& pos, const XMFLOAT3& target, const XMFLOAT3& up)
{
	XMVECTOR P = ::XMLoadFloat3(&pos);
	XMVECTOR T = ::XMLoadFloat3(&target);
	XMVECTOR U = ::XMLoadFloat3(&up);

	LookAt(P, T, U);
}

XMMATRIX Camera::View()const
{
	return ::XMLoadFloat4x4(&_view);
}

XMMATRIX Camera::Proj()const
{
	return ::XMLoadFloat4x4(&_proj);
}

XMMATRIX Camera::ViewProj()const
{
	return ::XMMatrixMultiply(View(), Proj());
}

void Camera::Strafe(float d)
{
	auto position = GetGameObject()->GetTransform()->GetPosition();

	XMVECTOR s = ::XMVectorReplicate(d);
	XMVECTOR r = ::XMLoadFloat3(&_right);
	XMVECTOR p = ::XMLoadFloat3(&position);
	::XMStoreFloat3(&position, XMVectorMultiplyAdd(s, r, p));
}

void Camera::Walk(float d)
{
	auto position = GetGameObject()->GetTransform()->GetPosition();

	XMVECTOR s = ::XMVectorReplicate(d);
	XMVECTOR l = ::XMLoadFloat3(&_look);
	XMVECTOR p = ::XMLoadFloat3(&position);
	::XMStoreFloat3(&position, XMVectorMultiplyAdd(s, l, p)); 
}

void Camera::Pitch(float angle)
{
	// Rotate up and look vector about the right vector.

	XMMATRIX R = ::XMMatrixRotationAxis(::XMLoadFloat3(&_right), angle);

	::XMStoreFloat3(&_up, ::XMVector3TransformNormal(XMLoadFloat3(&_up), R));
	::XMStoreFloat3(&_look, ::XMVector3TransformNormal(XMLoadFloat3(&_look), R));
}

void Camera::RotateY(float angle)
{
	// Rotate the basis vectors about the world y-axis.

	XMMATRIX R  = XMMatrixRotationY(angle);

	::XMStoreFloat3(&_right, ::XMVector3TransformNormal(::XMLoadFloat3(&_right), R));
	::XMStoreFloat3(&_up, ::XMVector3TransformNormal(::XMLoadFloat3(&_up), R));
	::XMStoreFloat3(&_look, ::XMVector3TransformNormal(::XMLoadFloat3(&_look), R));
}

void Camera::UpdateViewMatrix()
{
	auto position = GetGameObject()->GetTransform()->GetPosition();

	XMVECTOR R = ::XMLoadFloat3(&_right); 
	XMVECTOR U = ::XMLoadFloat3(&_up);
	XMVECTOR L = ::XMLoadFloat3(&_look);
	XMVECTOR P = ::XMLoadFloat3(&position); 

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

void Camera::Update()
{
}

void Camera::LateUpdate()
{
	XMVECTOR pos = m_pGameObject->GetTransform()->GetPosition();
	XMVECTOR dir = m_pGameObject->GetTransform()->GetLook();
	XMVECTOR target = pos + dir;
	XMVECTOR up = m_pGameObject->GetTransform()->GetUp();

	XMMATRIX V = ::XMMatrixLookAtLH(pos, target, up);
	::XMStoreFloat4x4(&_view, V);

	ProjUpdate();
	// OnPreCull 권장(Unity 생명주기)
	FrustumUpdate();
	//GetFrustumCulling();
}

void Camera::GetFrustumCulling()
{
	vector<GameObject*> allObject = SceneManager::GetI()->GetCurrentScene()->GetAllGameObjects();
	auto& culling = SceneManager::GetI()->GetCurrentScene()->GetCullingGameObjects();
	vector<PointLight>& pointLight = LightManager::GetI()->GetPointLights(true);
	vector<SpotLight>& spotLight = LightManager::GetI()->GetSpotLights(true);
	vector<PointLight> tempPointLight;
	vector<SpotLight> tempSpotLight;

	culling.clear();
	bool check;

	for (int i = 0; i < allObject.size(); i++)
	{
		auto meshRenderer = allObject[i]->GetComponent<MeshRenderer>();

		// meshRenderer 컴포넌트가 없는 객체이거나 mesh를 적용 안 시켰을 경우
		if (!meshRenderer)
			continue;

		if (!(meshRenderer->GetMesh()))
			continue;

		BouncingBall ball = meshRenderer->GetMesh()->Ball;
		check = true;
		for (int i = 0; i < 6; i++)
		{
			// 충돌체크
			//plane.normal.x * center.x + plane.normal.y * center.y + plane.normal.z * center.z + plane.d;
			float distance = m_frustum.planes[i].normal.x * ball.center.x + m_frustum.planes[i].normal.y * ball.center.y + m_frustum.planes[i].normal.z * ball.center.z + m_frustum.planes[i].d;
			if (distance < -ball.radius)
				check = false; // 구가 평면의 밖에 있음
		}

		if (check)
			culling.push_back(allObject[i]);
	}

	// PointLight
	for (int i = 0; i < pointLight.size(); i++)
	{
		check = true;
		for (int i = 0; i < 6; i++)
		{
			// 충돌체크
			//plane.normal.x * center.x + plane.normal.y * center.y + plane.normal.z * center.z + plane.d;
			float distance = m_frustum.planes[i].normal.x * pointLight[i].Position.x + 
							 m_frustum.planes[i].normal.y * pointLight[i].Position.y +
							 m_frustum.planes[i].normal.z * pointLight[i].Position.z + 
							 m_frustum.planes[i].d;
			if (distance < -pointLight[i].Range)
				check = false; // 구가 평면의 밖에 있음
		}

		if (check)
			tempPointLight.push_back(pointLight[i]);
	}

	// SpotLight를 PointLight처럼 구의 형태로 컬링을 하게되면 편하지만 정확도를 좀 떨어질 것이다.
	for (int i = 0; spotLight.size(); i++)
	{
		check = true;
		for (int i = 0; i < 6; i++)
		{
			// 충돌체크
			//plane.normal.x * center.x + plane.normal.y * center.y + plane.normal.z * center.z + plane.d;
			float distance = m_frustum.planes[i].normal.x * spotLight[i].Position.x +
				m_frustum.planes[i].normal.y * spotLight[i].Position.y +
				m_frustum.planes[i].normal.z * spotLight[i].Position.z +
				m_frustum.planes[i].d;
			if (distance < -spotLight[i].Range)
				check = false; // 구가 평면의 밖에 있음
		}

		if (check)
			tempSpotLight.push_back(spotLight[i]);
	}

	pointLight = tempPointLight;
	spotLight = tempSpotLight;
}

void Camera::FrustumUpdate()
{
	// 클립 행렬을 계산
	::XMMATRIX clipMatrix = DirectX::XMMatrixMultiply(::XMLoadFloat4x4(&_proj), ::XMLoadFloat4x4(&_view));
	::XMFLOAT4X4 clip;
	::XMStoreFloat4x4(&clip, clipMatrix);

	// 평면 계산 (각 면의 계수 계산)
	m_frustum.planes[0] = { {clip._41 + clip._11, clip._42 + clip._12, clip._43 + clip._13}, clip._44 + clip._14 }; // left
	m_frustum.planes[1] = { {clip._41 - clip._11, clip._42 - clip._12, clip._43 - clip._13}, clip._44 - clip._14 }; // right
	m_frustum.planes[2] = { {clip._41 + clip._21, clip._42 + clip._22, clip._43 + clip._23}, clip._44 + clip._24 }; // bottom
	m_frustum.planes[3] = { {clip._41 - clip._21, clip._42 - clip._22, clip._43 - clip._23}, clip._44 - clip._24 }; // top
	m_frustum.planes[4] = { {clip._41 + clip._31, clip._42 + clip._32, clip._43 + clip._33}, clip._44 + clip._34 }; // near
	m_frustum.planes[5] = { {clip._41 - clip._31, clip._42 - clip._32, clip._43 - clip._33}, clip._44 - clip._34 }; // far

	// 각 평면의 정규화
	for (int i = 0; i < 6; ++i) 
	{
		float length = sqrtf(m_frustum.planes[i].normal.x * m_frustum.planes[i].normal.x +
			m_frustum.planes[i].normal.y * m_frustum.planes[i].normal.y +
			m_frustum.planes[i].normal.z * m_frustum.planes[i].normal.z);

		if (length > 0.0f)
		{
			m_frustum.planes[i].normal.x /= length;
			m_frustum.planes[i].normal.y /= length;
			m_frustum.planes[i].normal.z /= length;
			m_frustum.planes[i].d /= length;
		}
	}
}

// private funtion
void Camera::ProjUpdate()
{
	m_nearWindowHeight = 2.0f * m_nearZ * tanf(0.5f * m_fovY);
	m_farWindowHeight = 2.0f * m_farZ * tanf(0.5f * m_fovY);

	XMMATRIX P;
	if (ProjectionType::Perspective == m_cameraType)
	{
		P = ::XMMatrixPerspectiveFovLH(m_fovY, m_aspect, m_nearZ, m_farZ);
	}
	else // Orthographic
	{
		Vec2 size = Application::GetI()->GetApp()->GetScreenSize();
		P = ::XMMatrixOrthographicLH(size.x, size.y, m_nearZ, m_farZ);
	}
	
	::XMStoreFloat4x4(&_proj, P);
}

string Camera::GetStringCameraType(ProjectionType type)
{
	switch (type)
	{
	case ProjectionType::Orthographic: return "Orthographic";
	case ProjectionType::Perspective: return "Perspective";
	default: return "Unknown";
	}
}

void Camera::OnInspectorGUI()
{
	// CameraType 선택GUI
	if (ImGui::BeginCombo("LightType", GetStringCameraType(m_cameraType).c_str())) // The second parameter is the previewed value
	{
		for (int n = 0; n < (int)ProjectionType::End; n++)
		{
			bool is_selected = ((int)m_cameraType == n); // You can store your selection somewhere
			if (ImGui::Selectable(GetStringCameraType((ProjectionType)n).c_str(), is_selected))
			{
				m_cameraType = (ProjectionType)n;
			}

			if (is_selected)
				ImGui::SetItemDefaultFocus(); // Set the initial focus when opening the combo (scrolling + for keyboard navigation support)
		}
		ImGui::EndCombo();
	}

	// Camera 값들 설정
	if (EditorGUI::FloatField("Near", m_nearZ)) {
		m_nearZ = max(m_nearZ, 0.001f);
	}
	if (EditorGUI::FloatField("Far", m_farZ)) {
		m_nearZ = min(m_nearZ, 9999);
	}

	if (ProjectionType::Perspective == m_cameraType)
	{
		float fov_degree = XMConvertToDegrees(m_fovY);
		if (EditorGUI::FloatField("Field Of View", fov_degree)) { 
			fov_degree = max(fov_degree, 1);  
			fov_degree = min(fov_degree, 360);
			m_fovY = XMConvertToRadians(fov_degree); 
		} 

		//ImGui::Text("Aspect");
		//ImGui::DragFloat("##Aspect", reinterpret_cast<float*>(&m_aspect), 0.1f);
	}
}

// 카메라 렌더 범위Draw
void Camera::OnDrawGizmos()
{
	Transform* transform = GetGameObject()->GetTransform();
	Gizmo::DrawFrustum(transform->GetWorldMatrix(), m_nearZ, m_farZ, XMConvertToDegrees(m_fovY));
}

GENERATE_COMPONENT_FUNC_TOJSON(Camera)
{
	json j = {};

	SERIALIZE_TYPE(j, Camera);
	SERIALIZE_ENUM(j, m_cameraType, "cameraType");
	 
	SERIALIZE_FLOAT(j, m_nearZ, "nearZ");
	SERIALIZE_FLOAT(j, m_farZ, "farZ"); 
	SERIALIZE_FLOAT(j, m_aspect, "aspect"); 
	SERIALIZE_FLOAT(j, m_fovY, "fovY");

	return j;
}

GENERATE_COMPONENT_FUNC_FROMJSON(Camera) 
{
	DE_SERIALIZE_ENUM(j, m_cameraType, "cameraType", ProjectionType);

	DE_SERIALIZE_FLOAT(j, m_nearZ, "nearZ");
	DE_SERIALIZE_FLOAT(j, m_farZ, "farZ");
	DE_SERIALIZE_FLOAT(j, m_aspect, "aspect");
	DE_SERIALIZE_FLOAT(j, m_fovY, "fovY");

	m_nearZ = max(m_nearZ, 0.001f);
	m_nearZ = min(m_nearZ, 9999); 
	float fov_degree = XMConvertToDegrees(m_fovY);
	fov_degree = max(fov_degree, 1); 
	fov_degree = min(fov_degree, 360); 
	m_fovY = XMConvertToRadians(fov_degree); 

	ProjUpdate();
}