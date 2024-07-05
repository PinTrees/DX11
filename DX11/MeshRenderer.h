#pragma once
#include "Component.h"

class Model;
class Shader;
class Effect;
class UMaterial;

class MeshRenderer 
	: public Component
{
	using Super = Component;

private:
	Shader*				m_Shader;
	wstring				m_ShaderPath;

	shared_ptr<Effect>  m_Effect;

	shared_ptr<Mesh>	m_Mesh;
	wstring				m_MeshPath;

	UMaterial*	m_pMaterial;
	wstring		m_MaterialPath;

public:
	MeshRenderer();
	virtual ~MeshRenderer();

	void SetMaterial(UMaterial* mat) { m_pMaterial = mat; }
	void SetMesh(shared_ptr<Mesh> mesh) { m_Mesh = mesh; }
	void SetShader(Shader* shader) { m_Shader = shader; }
	void SetShader(shared_ptr<Effect> effect) { m_Effect = effect; }

public:
	virtual void OnInspectorGUI() override;

public:
	virtual void Render() override;
	void RenderShadow();
	void RenderShadowNormal();
};

