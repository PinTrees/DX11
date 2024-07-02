#pragma once
#include "Component.h"

class Model;
class Shader;
class Effect;
class Material;

class MeshRenderer 
	: public Component
{
	using Super = Component;

private:
	Shader* m_Shader;
	shared_ptr<Effect>  m_Effect;
	uint8				m_Pass = 0;
	shared_ptr<Mesh>	m_Mesh;

public:
	MeshRenderer();
	virtual ~MeshRenderer();

	void SetMesh(shared_ptr<Mesh> mesh) { m_Mesh = mesh; }
	void SetShader(Shader* shader) { m_Shader = shader; }
	void SetShader(shared_ptr<Effect> effect) { m_Effect = effect; }
	void SetPass(uint8 pass) { m_Pass = pass; }

public:
	virtual void OnInspectorGUI() override;

public:
	virtual void Render() override;
	void RenderShadow();
	void RenderShadowNormal();
};

