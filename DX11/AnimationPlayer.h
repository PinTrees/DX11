#pragma once
#include "Component.h"

class SkinnedMeshRenderer;

enum class UpdateMode
{
	Normal,
	Fixed,
	UnScale,
};

class AnimationPlayer : public Component
{
	using Super = Component;

private:
	shared_ptr<AnimationClip>	m_AnimationClip;
	// Hidden
	string						m_AnimationFilePath;
	int							m_AnimationClipIndex;

	bool		m_PlayAutomatically = true; 

	// Hidden
	float		m_TimePos;
	UpdateMode	m_UpdateMode; 

	// Runtime Value
	shared_ptr<SkinnedMeshRenderer> m_pSkinnedMeshRenderer; 
	vector<XMFLOAT4X4>				m_FinalTransforms;
public:
	AnimationPlayer();
	virtual ~AnimationPlayer();

public:
	virtual void Awake() override; 
	virtual void Update() override;
	virtual void LastUpdate() override;

public:
	// Editor
	virtual void OnInspectorGUI() override;

	GENERATE_COMPONENT_BODY(AnimationPlayer)  
};

REGISTER_COMPONENT(AnimationPlayer) 
 