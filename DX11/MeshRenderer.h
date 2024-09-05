#pragma once
#include "Component.h"

class Model;
class Shader;
class Effect;
class UMaterial;
class InstancingBuffer;

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
	int					m_MeshSubsetIndex;

	UMaterial*	m_pMaterial;
	wstring		m_MaterialPath;

	//ShaderSetting m_shaderSetting; // 구조체 변수의 UseShadowMap은 그 생성된 그림자맵에 영향을 미칠건지에 대한 여부(영향을 받는 오브젝트)
	//bool m_createShadowMap; // 그림자맵 생성 단계에서 오브젝트를 그릴지에 대한 여부(영향을 주는 오브젝트)

public:
	MeshRenderer();
	virtual ~MeshRenderer();

	void SetMaterial(UMaterial* mat) { m_pMaterial = mat; }
	void SetMesh(shared_ptr<Mesh> mesh) { m_Mesh = mesh; }
	void SetShader(Shader* shader) { m_Shader = shader; }
	void SetShader(shared_ptr<Effect> effect) { m_Effect = effect; }

	shared_ptr<Mesh> GetMesh() { return m_Mesh; }

	// 임시 함수, 인스턴싱 ID
	InstanceID GetInstanceID()
	{
		return make_pair((uint64)m_Mesh.get(), (uint64)m_pMaterial);
	}

public:
	virtual void OnInspectorGUI() override;

public:
	virtual void Render() override;
	void RenderShadow();
	void RenderShadowNormal();

	void RenderInstancing(shared_ptr<class InstancingBuffer>& buffer);
	void RenderShadowInstancing(shared_ptr<class InstancingBuffer>& buffer);
	void RenderShadowNormalInstancing(shared_ptr<class InstancingBuffer>& buffer);

	GENERATE_COMPONENT_BODY(MeshRenderer)
};

REGISTER_COMPONENT(MeshRenderer)