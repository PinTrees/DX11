#pragma once

class Transform;

class Gizmo
{
public:
	static bool isDragging; 
	static ImVec2 lastMousePos; 
	static int activeAxis;

public:
	static void DrawCube(const XMMATRIX& worldMatrix, const Vec3& size);
	static void DrawSphere(const XMMATRIX& worldMatrix, float radius);
	static void DrawTransformHandler(Transform* transform);

private:
	static bool IsMouseOverHandle(const ImVec2& handlePos);
	static void DrawAxisHandle(const ImVec2& pos, const ImVec2& dir, const ImVec4& color);
};

