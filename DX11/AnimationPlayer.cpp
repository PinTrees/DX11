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

void AnimationPlayer::Awake()
{
	m_pSkinnedMeshRenderer = m_pGameObject->GetComponent_SP<SkinnedMeshRenderer>();
	
	if (m_pSkinnedMeshRenderer == nullptr)
		return;
	if (m_AnimationClip == nullptr)
		return;

	m_FinalTransforms.resize(m_pSkinnedMeshRenderer->GetMesh()->SkinnedData.BoneCount()); 
}

void AnimationPlayer::Update()
{
	if (m_pSkinnedMeshRenderer == nullptr)
		return;

	m_TimePos += DT;  
	//m_pSkinnedMeshRenderer->GetMesh()->SkinnedData.GetFinalTransforms(ClipName, TimePos, FinalTransforms);
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
	SERIALIZE_STRING(j, m_AnimationFilePath, "m_AnimationFilePath");
	SERIALIZE_INT(j, m_AnimationClipIndex, "m_AnimationClipIndex"); 
	SERIALIZE_BOOL(j, m_PlayAutomatically, "m_PlayAutomatically");

	return j;
}

GENERATE_COMPONENT_FUNC_FROMJSON(AnimationPlayer)
{
	DE_SERIALIZE_STRING(j, m_AnimationFilePath, "m_AnimationFilePath"); 
	DE_SERIALIZE_INT(j, m_AnimationClipIndex, "m_AnimationClipIndex");
	DE_SERIALIZE_BOOL(j, m_PlayAutomatically, "m_PlayAutomatically");

	if (m_AnimationFilePath != "") 
	{
		m_AnimationClip = ResourceManager::GetI()->LoadAnimationClip(m_AnimationFilePath, m_AnimationClipIndex);
		int aa = 0;
	}
}