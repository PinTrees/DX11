#pragma once
#include "EditorGUIStyle.h"

#define FIELD_DEFAULT_HEIGHT	20.f

#define EDITOR_GUI_FIELD_WIDTH (ImGui::GetContentRegionAvail().x * 0.65f)
#define EDITOR_GUI_BUTTON_WIDTH  20

#define EDITOR_GUI_FIELD_PADDING    ImGui::Dummy(ImVec2(4, 0));	\
									ImGui::SameLine()			\

#define EDITOR_GUI_FIELD_SPACING(s)	ImGui::SameLine();													\
									ImGui::Dummy(ImVec2(ImGui::GetContentRegionAvail().x - s - 4, 0));	\
									ImGui::SameLine()													\

class Mesh;
class SkinnedMesh;

class EditorGUI
{
public:
	static EditorTextStyle defaultTextStyle;

public:
	static void RowSizedBox(float size);

	static void Image(wstring path, ImVec2 size);
	static void Checkbox();
	static bool InputField(string& inputText);
	static void Label(string text, EditorTextStyle guiStyle = defaultTextStyle);
	static void LabelHeader(string text);
	static bool Button(string text, Vec2 size = Vec2(0, 20), Color color = Color(0.31f, 0.31f, 0.31f, 1.0f));

	static bool ComponentHeader(string title);
	static void ComponentDivider();
	static void Divider(Color color, float height = 1.8f);
	static void Spacing(Vec2 size);


public:
	static bool FloatField(string title, float& v, ImVec2 size);
	static bool FloatField(string title, float& v);
	static bool Vector3Field(string title, Vec3& vec3);
	static bool MaterialField(string title, shared_ptr<UMaterial>& material, wstring& materialPath);
	static bool MeshField(string title, shared_ptr<Mesh>& mesh, int& subsetIndex);
	static bool SkinnedMeshField(string title, shared_ptr<SkinnedMesh>& mesh, int& subsetIndex);

public:
	static void DropFieldStylePush();
	static void DropFieldStylePop();

	static void FieldStylePush();
	static void FieldStylePop();

	static void ComponentBlockStylePush();
	static void ComponentBlockStylePop();

	static void EditorWindowStylePush();
	static void EditorWindowStylePop();
};

