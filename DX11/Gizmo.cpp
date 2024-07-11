#include "pch.h"
#include "Gizmo.h"
#include "Effects.h"
#include "MathHelper.h"
#include "SceneEditorWindow.h"
#include "EditorCamera.h"

bool Gizmo::isDragging = false;
ImVec2 Gizmo::lastMousePos = ImVec2(0, 0);
int Gizmo::activeAxis = 0;

void Gizmo::DrawVector(const Matrix worldMatrix, const Vec3 vector)
{
    auto context = Application::GetI()->GetDeviceContext();

    // ī�޶��� �� �������� ��� ��������
    Matrix viewProj = RenderManager::GetI()->cameraViewProjectionMatrix;
    Matrix worldViewProj = worldMatrix * viewProj;

    // ����Ʈ ũ�� ��������
    D3D11_VIEWPORT viewport;
    UINT numViewports = 1;
    context->RSGetViewports(&numViewports, &viewport);

    // ī�޶��� ��ġ�� ��������
    EditorCamera* sceneViewCamera = SceneViewManager::GetI()->m_LastActiveSceneEditorWindow->GetSceneCamera();
    Vec3 cameraPosition = sceneViewCamera->GetPosition();
    Vec3 position = Vec3(worldMatrix._41, worldMatrix._42, worldMatrix._43);

    // ī�޶�� ������Ʈ ���� �Ÿ� ���
    float distance = (position - cameraPosition).Length();
    distance = max(distance, 0.1f); // �ּ� �Ÿ� ����

    // �Ÿ� ��� �����ϸ� ���� ���
    float scale = distance / 7.0f;

    // ���� â�� ��ġ�� ũ�⸦ ������
    ImVec2 windowPos = ImGui::GetWindowPos();
    ImVec2 contentRegionMin = ImGui::GetWindowContentRegionMin();
    ImVec2 contentRegionMax = ImGui::GetWindowContentRegionMax();
    ImVec2 offset = ImVec2(contentRegionMin.x + windowPos.x, contentRegionMin.y + windowPos.y);

    auto WorldToScreen = [&](const Vec3& worldPos) -> ImVec2
        {
            Vec3 pos = Vector3::Transform(worldPos, worldViewProj);
            float x = (pos.x / pos.z) * 0.5f + 0.5f;
            float y = (pos.y / pos.z) * -0.5f + 0.5f;
            return ImVec2(x * viewport.Width + viewport.TopLeftX + offset.x, y * viewport.Height + viewport.TopLeftY + offset.y);
        };

    const ImU32 color = IM_COL32(255, 0, 0, 255);
    const float thickness = 2.0f;

    // ������ �� ���� ���
    Vec3 start = Vector3::Transform(Vec3::Zero, worldMatrix);
    Vec3 end = Vector3::Transform(vector * scale, worldMatrix);

    // ȭ�� ��ǥ�� ��ȯ
    ImVec2 startScreen = WorldToScreen(start);
    ImVec2 endScreen = WorldToScreen(end);

    // ���� �׸���
    ImGui::GetWindowDrawList()->AddLine(startScreen, endScreen, color, thickness);
}

void Gizmo::DrawCube(const XMMATRIX& worldMatrix, const Vec3& size)
{
    auto context = Application::GetI()->GetDeviceContext();

    // �ڽ��� 8���� ������ ����
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

    // ī�޶��� �� �������� ��� ��������
    XMMATRIX viewProj = RenderManager::GetI()->cameraViewProjectionMatrix;
    XMMATRIX worldViewProj = worldMatrix * viewProj;

    // ����Ʈ ũ�� ��������
    D3D11_VIEWPORT viewport;
    UINT numViewports = 1;
    context->RSGetViewports(&numViewports, &viewport);

    // ���� â�� ��ġ�� ũ�⸦ ������
    ImVec2 windowPos = ImGui::GetWindowPos();
    ImVec2 contentRegionMin = ImGui::GetWindowContentRegionMin();
    ImVec2 contentRegionMax = ImGui::GetWindowContentRegionMax();
    ImVec2 offset = ImVec2(contentRegionMin.x + windowPos.x, contentRegionMin.y + windowPos.y);

    // �������� ��ũ�� ��ǥ�� ��ȯ
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

    // ImGui�� ����Ͽ� �ڽ��� ������ �׸�
    const ImU32 color = IM_COL32(0, 255, 0, 255);
    const float thickness = 1.0f;

    // �ڽ��� 12���� ���� �׸���
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

    const int circleSegments = 25;  // ���� �����ϴ� ���׸�Ʈ ��

    std::vector<XMFLOAT3> vertices;
    vertices.reserve(circleSegments + 1);

    // ī�޶��� �� �������� ��� ��������
    XMMATRIX viewProj = RenderManager::GetI()->cameraViewProjectionMatrix;
    XMMATRIX worldViewProj = worldMatrix * viewProj;

    // ����Ʈ ũ�� ��������
    D3D11_VIEWPORT viewport;
    UINT numViewports = 1;
    context->RSGetViewports(&numViewports, &viewport);

    // ���� â�� ��ġ�� ũ�⸦ ������
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

    // �� �࿡ ���� ���� �׸�
    XMFLOAT3 center(0.0f, 0.0f, 0.0f);

    XMVECTOR right = XMVector3TransformNormal(XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f), worldMatrix); 
    XMVECTOR up = XMVector3TransformNormal(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), worldMatrix);
    XMVECTOR forward = XMVector3TransformNormal(XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), worldMatrix);

    XMFLOAT3 rightF, upF, forwardF;
    XMStoreFloat3(&rightF, right);
    XMStoreFloat3(&upF, up);
    XMStoreFloat3(&forwardF, forward);

    DrawCircle(center, upF, rightF);  // XY ��� 
    DrawCircle(center, forwardF, rightF);  // XZ ��� 
    DrawCircle(center, upF, forwardF);  // YZ ��� 
     
    EditorCamera* sceneViewCamera = SceneViewManager::GetI()->m_LastActiveSceneEditorWindow->GetSceneCamera();
    XMMATRIX camWorldMatrix = sceneViewCamera->GetWorldMatrix();  

    XMFLOAT3 camRight = sceneViewCamera->GetRight();
    XMFLOAT3 camUp = sceneViewCamera->GetUp();
    XMFLOAT3 camRightF, camUpF;

    XMVECTOR camRightV = XMVector3TransformNormal(XMVectorSet(camRight.x, camRight.y, camRight.z, 0.0f), worldMatrix); 
    XMVECTOR camUpV = XMVector3TransformNormal(XMVectorSet(camUp.x, camUp.y, camUp.z, 0.0f), worldMatrix);

    XMStoreFloat3(&camRightF, camRightV);
    XMStoreFloat3(&camUpF, camUpV);

    DrawCircle(center, camUpF, camRightF);  // XY ��� 
}

void Gizmo::DrawArrow(Vector3 position, Vec3 dir, ImVec4 color) 
{
    EditorCamera* sceneViewCamera = SceneViewManager::GetI()->m_LastActiveSceneEditorWindow->GetSceneCamera();

    // World Matrix ��������
    Matrix worldMatrix = Matrix::CreateTranslation(position);

    auto context = Application::GetI()->GetDeviceContext();
    Matrix viewProj = RenderManager::GetI()->cameraViewProjectionMatrix;

    // ��ġ�� ����
    Vector3 pos = position;

    // ī�޶��� ��ġ�� ��������
    Vector3 cameraPosition = sceneViewCamera->GetPosition();

    // ī�޶�� ������Ʈ ���� �Ÿ� ���
    float distance = Vector3::Distance(pos, cameraPosition);
    distance = max(distance, 0.1f); // �ּ� �Ÿ� ����

    // �Ÿ� ��� �����ϸ� ���� ���
    float scale = distance / 10;

    // 3D���� 2D�� ����
    D3D11_VIEWPORT viewport;
    UINT numViewports = 1;
    context->RSGetViewports(&numViewports, &viewport);

    // ���� â�� ��ġ�� ũ�⸦ ������
    ImVec2 windowPos = ImGui::GetWindowPos();
    ImVec2 contentRegionMin = ImGui::GetWindowContentRegionMin();
    ImVec2 contentRegionMax = ImGui::GetWindowContentRegionMax();
    ImVec2 offset = ImVec2(contentRegionMin.x + windowPos.x, contentRegionMin.y + windowPos.y);

    auto WorldToScreen = [&](const Vector3& worldPos) -> Vec2
        {
            DirectX::XMFLOAT4 projectedPos; 
            DirectX::XMStoreFloat4(&projectedPos, DirectX::XMVector3TransformCoord(worldPos, viewProj)); 

            if (projectedPos.w <= 0.0f)
            {
                return Vec2(-1000.0f, -1000.0f);
            }

            Vec2 screenPos;
            screenPos.x = (projectedPos.x / projectedPos.w * 0.5f + 0.5f) * (contentRegionMax.x - contentRegionMin.x) + offset.x;
            screenPos.y = (1.0f - (projectedPos.y / projectedPos.w * 0.5f + 0.5f)) * (contentRegionMax.y - contentRegionMin.y) + offset.y;
            return screenPos;
        };

    Vec2 handlePos = WorldToScreen(pos);
    Vec2 endPos = WorldToScreen(pos + dir * scale); 

    // �ڵ� �׸���
    DrawwPoinVector(handlePos, endPos, color);
}

void Gizmo::DrawTransformHandler(Transform* transform)
{
    EditorCamera* sceneViewCamera = SceneViewManager::GetI()->m_LastActiveSceneEditorWindow->GetSceneCamera();

    XMMATRIX worldMatrix = transform->GetWorldMatrix();

    auto context = Application::GetI()->GetDeviceContext();
    XMMATRIX viewProj = RenderManager::GetI()->cameraViewProjectionMatrix;

    // ��ġ�� ����
    XMVECTOR position = transform->GetPosition();

    // ī�޶��� ��ġ�� ��������
    XMFLOAT3 cameraPositionFloat3 = sceneViewCamera->GetPosition();
    XMVECTOR cameraPosition = XMLoadFloat3(&cameraPositionFloat3);

    // ī�޶�� ������Ʈ ���� �Ÿ� ���
    float distance = XMVectorGetX(XMVector3Length(position - cameraPosition));
    distance = max(distance, 0.1f); // �ּ� �Ÿ� ����

    // �Ÿ� ��� �����ϸ� ���� ���
    float scale = distance / 7;

    // �� ���� ���� ���͸� ����
    XMVECTOR xAxis = XMVector3Normalize(XMVector3TransformNormal(XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f), worldMatrix)) * scale;
    XMVECTOR yAxis = XMVector3Normalize(XMVector3TransformNormal(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), worldMatrix)) * scale;
    XMVECTOR zAxis = XMVector3Normalize(XMVector3TransformNormal(XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), worldMatrix)) * scale;

    // 3D���� 2D�� ����
    D3D11_VIEWPORT viewport;
    UINT numViewports = 1;
    context->RSGetViewports(&numViewports, &viewport);

    // ���� â�� ��ġ�� ũ�⸦ ������
    ImVec2 windowPos = ImGui::GetWindowPos();
    ImVec2 contentRegionMin = ImGui::GetWindowContentRegionMin();
    ImVec2 contentRegionMax = ImGui::GetWindowContentRegionMax();
    ImVec2 offset = ImVec2(contentRegionMin.x + windowPos.x, contentRegionMin.y + windowPos.y);

    auto WorldToScreen = [&](const XMVECTOR& worldPos) -> ImVec2
    {
            DirectX::XMFLOAT4 projectedPos;
            DirectX::XMStoreFloat4(&projectedPos, DirectX::XMVector3TransformCoord(worldPos, viewProj));

            // Z-������ 0���� ���� ��츦 ó��
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

    // �ڵ� �׸���
    ImVec4 xColor = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
    ImVec4 yColor = ImVec4(0.0f, 1.0f, 0.0f, 1.0f);
    ImVec4 zColor = ImVec4(0.0f, 0.0f, 1.0f, 1.0f);

    DrawAxisHandle(handlePos, xEnd, xColor);
    DrawAxisHandle(handlePos, yEnd, yColor);
    DrawAxisHandle(handlePos, zEnd, zColor);

    // ���콺 �Է� ó�� �� ��ȯ ����
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
        //    transformMatrix._43 += delta.x * 0.01f; // �ܼ� �����̹Ƿ� Z�൵ X��ǥ�� ���� ����
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

void Gizmo::DrawwPoinVector(const Vec2& pos, const Vec2& dir, const ImVec4& color)
{
    ImGui::GetWindowDrawList()->AddLine(ImVec2(pos.x, pos.y), ImVec2(dir.x, dir.y), ImGui::GetColorU32(color), 1.0f);
    ImGui::GetWindowDrawList()->AddCircleFilled(ImVec2(pos.x, pos.y), 3.0f, ImGui::GetColorU32(color));

    // ���� ���� ���
    Vec2 direction = dir - pos;
    float length = sqrtf(direction.x * direction.y + direction.y * direction.y); 

    // �ﰢ���� �ٸ� �� ���� ���
    Vec2 directionNorm = direction / length;
    Vec2 orthogonal(-directionNorm.y, directionNorm.x); // orthogonal ���� 

    float arrowSize = 10.0f; // �ﰢ���� ũ�⸦ �����ϱ� ���� �� 
    Vec2 p1 = dir;
    Vec2 p2 = dir - directionNorm * arrowSize + orthogonal * arrowSize * 0.5f;  
    Vec2 p3 = dir - directionNorm * arrowSize - orthogonal * arrowSize * 0.5f;  

    // �ﰢ�� �׸���
    ImGui::GetWindowDrawList()->AddTriangleFilled(
        ImVec2(p1.x, p1.y), 
        ImVec2(p2.x, p2.y), 
        ImVec2(p3.x, p3.y), 
        ImGui::GetColorU32(color));
}