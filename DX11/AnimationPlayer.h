#pragma once
#include "Component.h"

class AnimationPlayer : public Component
{
	using Super = Component;

private:
	shared_ptr<AnimationClip>	m_AnimationClip;
	// Hidden
	string						m_AnimationFilePath;
	int							m_AnimationClipIndex;

	bool		m_PlayAutomatically = true; 

public:
	AnimationPlayer();
	virtual ~AnimationPlayer();

public:
	virtual void Update() override;
	virtual void LastUpdate() override;

public:
	// Editor
	virtual void OnInspectorGUI() override;

	GENERATE_COMPONENT_BODY(AnimationPlayer) 
};

REGISTER_COMPONENT(AnimationPlayer)
 