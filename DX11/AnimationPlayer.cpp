#include "pch.h"
#include "AnimationPlayer.h"
#include "EditorGUI.h"

AnimationPlayer::AnimationPlayer()
{
	m_InspectorTitleName = "Animation"; 
}

AnimationPlayer::~AnimationPlayer()
{
}

void AnimationPlayer::Update()
{
}

void AnimationPlayer::LastUpdate()
{

}

void AnimationPlayer::OnInspectorGUI()
{
	EditorGUI::AnimationClipField("Animation Clip", m_AnimationClip, m_AnimationFilePath, m_AnimationClipIndex);
	EditorGUI::BoolField("Play Automatically", m_PlayAutomatically);
}



GENERATE_COMPONENT_FUNC_TOJSON(AnimationPlayer)
{
	json j;
	SERIALIZE_TYPE(j, AnimationPlayer);
	return j;
}

GENERATE_COMPONENT_FUNC_FROMJSON(AnimationPlayer)
{
}