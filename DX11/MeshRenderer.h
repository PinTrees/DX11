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

	vector<shared_ptr<UMaterial>> m_pMaterials;
	vector<wstring> m_MaterialPaths;

	//bool m_createShadowMap; // �׸��ڸ� ���� �ܰ迡�� ������Ʈ�� �׸����� ���� ����(������ �ִ� ������Ʈ)
public:
	MeshRenderer();
	virtual ~MeshRenderer();

	void AddMaterial(shared_ptr<UMaterial> mat) { m_pMaterials.push_back(mat); }
	void SetMesh(shared_ptr<Mesh> mesh) { m_Mesh = mesh; }
	void SetShader(Shader* shader) { m_Shader = shader; }
	void SetShader(shared_ptr<Effect> effect) { m_Effect = effect; }

	shared_ptr<Mesh> GetMesh() { if (m_Mesh) return m_Mesh; else return nullptr; }

	// �ν��Ͻ� ID, mesh and material and setting
	InstanceID GetInstanceID()
	{
		return make_tuple((uint64)m_Mesh.get(), 0, 0);
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