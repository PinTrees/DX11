#pragma once
#include "Component.h"

class SkinnedMesh;
class Model;
class Shader;
class Effect;
class UMaterial;
class InstancingBuffer;

class SkinnedMeshRenderer :
    public Component
{
	using Super = Component;

private:
	Shader* m_Shader;
	wstring				m_ShaderPath;

	shared_ptr<Effect>  m_Effect;

	shared_ptr<SkinnedMesh>	m_SkinnedMesh;
	wstring				m_MeshPath;
	int					m_MeshSubsetIndex;

	UMaterial* m_pMaterial;
	wstring		m_MaterialPath;

	//bool m_createShadowMap; // 그림자맵 생성 단계에서 오브젝트를 그릴지에 대한 여부(영향을 주는 오브젝트)
public:
	SkinnedMeshRenderer();
	virtual ~SkinnedMeshRenderer();

	void SetMaterial(UMaterial* mat) { m_pMaterial = mat; }
	void SetMesh(shared_ptr<SkinnedMesh> mesh) { m_SkinnedMesh = mesh; }
	void SetShader(Shader* shader) { m_Shader = shader; }
	void SetShader(shared_ptr<Effect> effect) { m_Effect = effect; }

	shared_ptr<SkinnedMesh> GetMesh() { return m_SkinnedMesh; }

	// 인스턴싱 ID, mesh and material and setting
	InstanceID GetInstanceID()
	{
		return make_tuple((uint64)m_SkinnedMesh.get(), (uint64)m_pMaterial, 0);
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

	GENERATE_COMPONENT_BODY(SkinnedMeshRenderer)
};

REGISTER_COMPONENT(SkinnedMeshRenderer)