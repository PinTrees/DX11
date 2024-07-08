#include "pch.h"
#include "Gizmo.h"
#include "Effects.h"
#include "MathHelper.h"
#include "SceneEditorWindow.h"
#include "EditorCamera.h"

bool Gizmo::isDragging = false;
ImVec2 Gizmo::lastMousePos = ImVec2(0, 0);
int Gizmo::activeAxis = 0;

void Gizmo::DrawCube(const XMMATRIX& worldMatrix, const Vec3& size)
{
    auto context = Application::GetI()->GetDeviceContext();

    // 박스의 8개의 꼭짓점 정의
    XMFLOAT3 halfSize = { size.x * 0.5f, size.y * 0.5f, size.z * 0.5f };
    XMFLOAT3 vertices[8] = {
        { -halfSize.x, -halfSize.y, -halfSize.z },
        { -halfSize.x,  halfSize.y, -halfSize.z },
        {  halfSize.x,  halfSize.y, -halfSize.z },
        {  halfSize.x, -halfSize.y, -halfSize.z },
        { -halfSize.x, -halfSize.y,  halfSize.z },
        { -halfSize.x,  halfSize.y,  halfSize.z },
        {  halfSize.x,  halfSize.y,  halfSize.z },
        {  halfSize.x, -halfSize.y,  halfSize.z }
    };

    // 카메라의 뷰 프로젝션 행렬 가져오기
    XMMATRIX viewProj = RenderManager::GetI()->cameraViewProjectionMatrix;
    XMMATRIX worldViewProj = worldMatrix * viewProj;

    // 뷰포트 크기 가져오기
    D3D11_VIEWPORT viewport;
    UINT numViewports = 1;
    context->RSGetViewports(&numViewports, &viewport);

    // 현재 창의 위치와 크기를 가져옴
    ImVec2 windowPos = ImGui::GetWindowPos();
    ImVec2 contentRegionMin = ImGui::GetWindowContentRegionMin();
    ImVec2 contentRegionMax = ImGui::GetWindowContentRegionMax();
    ImVec2 offset = ImVec2(contentRegionMin.x + windowPos.x, contentRegionMin.y + windowPos.y);

    // 꼭짓점을 스크린 좌표로 변환
    ImVec2 screenVertices[8];
    for (int i = 0; i < 8; ++i)
    {
        XMVECTOR pos = XMVector3TransformCoord(XMLoadFloat3(&vertices[i]), worldViewProj);

        // NDC to screen space conversion
        float x = (XMVectorGetX(pos) / XMVectorGetW(pos)) * 0.5f + 0.5f;
        float y = (XMVectorGetY(pos) / XMVectorGetW(pos)) * -0.5f + 0.5f;
        screenVertices[i] = ImVec2(x * (contentRegionMax.x - contentRegionMin.x) + offset.x,
            y * (contentRegionMax.y - contentRegionMin.y) + offset.y);
    }

    // ImGui를 사용하여 박스의 엣지를 그림
    const ImU32 color = IM_COL32(0, 255, 0, 255);
    const float thickness = 1.0f;

    // 박스의 12개의 엣지 그리기
    const int edges[12][2] = {
        {0, 1}, {1, 2}, {2, 3}, {3, 0},
        {4, 5}, {5, 6}, {6, 7}, {7, 4},
        {0, 4}, {1, 5}, {2, 6}, {3, 7}
    };

    for (int i = 0; i < 12; ++i)
    {
        ImGui::GetWindowDrawList()->AddLine(screenVertices[edges[i][0]], screenVertices[edges[i][1]], color, thickness);
    }
}


void Gizmo::DrawSphere(const XMMATRIX& worldMatrix, float radius)
{
    auto context = Application::GetI()->GetDeviceContext();

    const int circleSegments = 40;  // 원을 구성하는 세그먼트 수

    std::vector<XMFLOAT3> vertices;
    vertices.reserve(circleSegments + 1);

    // 카메라의 뷰 프로젝션 행렬 가져오기
    XMMATRIX viewProj = RenderManager::GetI()->cameraViewProjectionMatrix;
    XMMATRIX worldViewProj = worldMatrix * viewProj;

    // 뷰포트 크기 가져오기
    D3D11_VIEWPORT viewport;
    UINT numViewports = 1;
    context->RSGetViewports(&numViewports, &viewport);

    // 현재 창의 위치와 크기를 가져옴
    ImVec2 windowPos = ImGui::GetWindowPos();
    ImVec2 contentRegionMin = ImGui::GetWindowContentRegionMin();
    ImVec2 contentRegionMax = ImGui::GetWindowContentRegionMax();
    ImVec2 offset = ImVec2(contentRegionMin.x + windowPos.x, contentRegionMin.y + windowPos.y);

    auto WorldToScreen = [&](const XMFLOAT3& worldPos) -> ImVec2
        {
            XMVECTOR pos = XMVector3TransformCoord(XMLoadFloat3(&worldPos), worldViewProj);
            float x = (XMVectorGetX(pos) / XMVectorGetW(pos)) * 0.5f + 0.5f;
            float y = (XMVectorGetY(pos) / XMVectorGetW(pos)) * -0.5f + 0.5f;
            return ImVec2(x * viewport.Width + viewport.TopLeftX + offset.x, y * viewport.Height + viewport.TopLeftY + offset.y);
        };

    const ImU32 color = IM_COL32(0, 255, 0, 255);
    const float thickness = 1.0f;

    auto DrawCircle = [&](const XMFLOAT3& center, const XMFLOAT3& up, const XMFLOAT3& right)
        {
            vertices.clear();
            for (int i = 0; i <= circleSegments; ++i)
            {
                float theta = i * XM_2PI / circleSegments;
                XMFLOAT3 pos;
                pos.x = center.x + radius * (right.x * cosf(theta) + up.x * sinf(theta));
                pos.y = center.y + radius * (right.y * cosf(theta) + up.y * sinf(theta));
                pos.z = center.z + radius * (right.z * cosf(theta) + up.z * sinf(theta));
                vertices.push_back(pos);
            }

            for (int i = 0; i < circleSegments; ++i)
            {
                ImGui::GetWindowDrawList()->AddLine(WorldToScreen(vertices[i]), WorldToScreen(vertices[i + 1]), color, thickness);
            }
        };

    // 각 축에 대해 원을 그림
    XMFLOAT3 center(0.0f, 0.0f, 0.0f);

    XMVECTOR right = XMVector3TransformNormal(XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f), worldMatrix);
    XMVECTOR up = XMVector3TransformNormal(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), worldMatrix);
    XMVECTOR forward = XMVector3TransformNormal(XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), worldMatrix);

    XMFLOAT3 rightF, upF, forwardF;
    XMStoreFloat3(&rightF, right);
    XMStoreFloat3(&upF, up);
    XMStoreFloat3(&forwardF, forward);

    DrawCircle(center, upF, rightF);  // XY 평면 
    DrawCircle(center, forwardF, rightF);  // XZ 평면 
    DrawCircle(center, upF, forwardF);  // YZ 평면 
     
  

    //// 현재 오브젝트와 카메라 위치를 이용하여 평면을 정의하고 원을 그림
    //EditorCamera* sceneViewCamera = SceneViewManager::GetI()->m_LastActiveSceneEditorWindow->GetSceneCamera();
    //XMFLOAT3 objectPosition;
    //XMStoreFloat3(&objectPosition, worldMatrix.r[3]);  // 월드 매트릭스에서 오브젝트의 위치 추출

    //// 카메라의 방향 벡터를 구함
    //XMFLOAT3 cameraPosition = sceneViewCamera->GetPosition();
    //XMFLOAT3 direction;
    //direction.x = objectPosition.x - cameraPosition.x;
    //direction.y = objectPosition.y - cameraPosition.y;
    //direction.z = objectPosition.z - cameraPosition.z;

    //// 방향 벡터를 정규화
    //XMVECTOR directionVec = XMLoadFloat3(&direction);
    //directionVec = XMVector3Normalize(directionVec);
    //XMStoreFloat3(&direction, directionVec);

    //// up 벡터를 정의
    //XMFLOAT3 upC(0.0f, 1.0f, 0.0f);
    //XMVECTOR upVec = XMLoadFloat3(&upC);

    //// right 벡터를 구하기 위해 cross product를 사용
    //XMVECTOR rightVec = XMVector3Cross(directionVec, upVec);
    //XMFLOAT3 rightC;
    //XMStoreFloat3(&rightC, rightVec);

    EditorCamera* sceneViewCamera = SceneViewManager::GetI()->m_LastActiveSceneEditorWindow->GetSceneCamera();
    XMMATRIX camWorldMatrix = sceneViewCamera->GetWorldMatrix();  

    XMFLOAT3 camRight = sceneViewCamera->GetRight();
    XMFLOAT3 camUp = sceneViewCamera->GetUp();
    XMFLOAT3 camRightF, camUpF;

    XMVECTOR camRightV = XMVector3TransformNormal(XMVectorSet(camRight.x, camRight.y, camRight.z, 0.0f), worldMatrix); 
    XMVECTOR camUpV = XMVector3TransformNormal(XMVectorSet(camUp.x, camUp.y, camUp.z, 0.0f), worldMatrix);

    XMStoreFloat3(&camRightF, camRightV);
    XMStoreFloat3(&camUpF, camUpV);

    DrawCircle(center, camUpF, camRightF);  // XY 평면 
}


void Gizmo::DrawTransformHandler(Transform* transform)
{
    EditorCamera* sceneViewCamera = SceneViewManager::GetI()->m_LastActiveSceneEditorWindow->GetSceneCamera();

    XMMATRIX worldMatrix = transform->GetWorldMatrix();

    auto context = Application::GetI()->GetDeviceContext();
    XMMATRIX viewProj = RenderManager::GetI()->cameraViewProjectionMatrix;

    // 위치를 추출
    XMVECTOR position = transform->GetPosition();

    // 카메라의 위치를 가져오기
    XMFLOAT3 cameraPositionFloat3 = sceneViewCamera->GetPosition();
    XMVECTOR cameraPosition = XMLoadFloat3(&cameraPositionFloat3);

    // 카메라와 오브젝트 간의 거리 계산
    float distance = XMVectorGetX(XMVector3Length(position - cameraPosition));
    distance = max(distance, 0.1f); // 최소 거리 제한

    // 거리 기반 스케일링 팩터 계산
    float scale = distance / 7;

    // 각 축의 방향 벡터를 정의
    XMVECTOR xAxis = XMVector3Normalize(XMVector3TransformNormal(XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f), worldMatrix)) * scale;
    XMVECTOR yAxis = XMVector3Normalize(XMVector3TransformNormal(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), worldMatrix)) * scale;
    XMVECTOR zAxis = XMVector3Normalize(XMVector3TransformNormal(XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), worldMatrix)) * scale;

    // 3D에서 2D로 투영
    D3D11_VIEWPORT viewport;
    UINT numViewports = 1;
    context->RSGetViewports(&numViewports, &viewport);

    // 현재 창의 위치와 크기를 가져옴
    ImVec2 windowPos = ImGui::GetWindowPos();
    ImVec2 contentRegionMin = ImGui::GetWindowContentRegionMin();
    ImVec2 contentRegionMax = ImGui::GetWindowContentRegionMax();
    ImVec2 offset = ImVec2(contentRegionMin.x + windowPos.x, contentRegionMin.y + windowPos.y);

    auto WorldToScreen = [&](const XMVECTOR& worldPos) -> ImVec2
    {
            DirectX::XMFLOAT4 projectedPos;
            DirectX::XMStoreFloat4(&projectedPos, DirectX::XMVector3TransformCoord(worldPos, viewProj));

            // Z-분할이 0보다 작은 경우를 처리
            if (projectedPos.w <= 0.0f)
            {
                return ImVec2(-1000.0f, -1000.0f);
            }

            ImVec2 screenPos;
            screenPos.x = (projectedPos.x / projectedPos.w * 0.5f + 0.5f) * (contentRegionMax.x - contentRegionMin.x) + offset.x;
            screenPos.y = (1.0f - (projectedPos.y / projectedPos.w * 0.5f + 0.5f)) * (contentRegionMax.y - contentRegionMin.y) + offset.y;
            return screenPos;
    };

    ImVec2 handlePos = WorldToScreen(position);
    ImVec2 xEnd = WorldToScreen(position + xAxis);
    ImVec2 yEnd = WorldToScreen(position + yAxis);
    ImVec2 zEnd = WorldToScreen(position + zAxis);

    // 핸들 그리기
    ImVec4 xColor = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
    ImVec4 yColor = ImVec4(0.0f, 1.0f, 0.0f, 1.0f);
    ImVec4 zColor = ImVec4(0.0f, 0.0f, 1.0f, 1.0f);

    DrawAxisHandle(handlePos, xEnd, xColor);
    DrawAxisHandle(handlePos, yEnd, yColor);
    DrawAxisHandle(handlePos, zEnd, zColor);

    // 마우스 입력 처리 및 변환 적용
    if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
    {
        if (IsMouseOverHandle(handlePos))
        {
            isDragging = true;
            lastMousePos = ImGui::GetMousePos();
        }
    }

    if (ImGui::IsMouseReleased(ImGuiMouseButton_Left))
    {
        isDragging = false;
    }

    if (isDragging)
    {
        //ImVec2 mousePos = ImGui::GetMousePos();
        //ImVec2 delta = ImVec2(mousePos.x - lastMousePos.x, mousePos.y - lastMousePos.y);
        //
        //switch (activeAxis)
        //{
        //case 1: // X axis
        //    transformMatrix._41 += delta.x * 0.01f;
        //    break;
        //case 2: // Y axis
        //    transformMatrix._42 += delta.y * 0.01f;
        //    break;
        //case 3: // Z axis
        //    transformMatrix._43 += delta.x * 0.01f; // 단순 예제이므로 Z축도 X좌표에 대해 변경
        //    break;
        //}
        //
        //lastMousePos = mousePos;

        //transform = DirectX::XMLoadFloat4x4(&transformMatrix);
    }
}

bool Gizmo::IsMouseOverHandle(const ImVec2& handlePos)
{
    ImVec2 mousePos = ImGui::GetMousePos();
    return (mousePos.x >= handlePos.x && mousePos.x <= handlePos.x + 10 &&
        mousePos.y >= handlePos.y && mousePos.y <= handlePos.y + 10);
}

void Gizmo::DrawAxisHandle(const ImVec2& pos, const ImVec2& dir, const ImVec4& color)
{
    ImGui::GetWindowDrawList()->AddLine(pos, dir, ImGui::GetColorU32(color), 2.0f);
    ImGui::GetWindowDrawList()->AddCircleFilled(dir, 5.0f, ImGui::GetColorU32(color));
}