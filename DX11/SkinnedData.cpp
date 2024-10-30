#include "pch.h"
#include "SkinnedData.h"
#include "MathHelper.h"

Keyframe::Keyframe()
	: TimePos(0.0f),
	Translation(0.0f, 0.0f, 0.0f),
	Scale(1.0f, 1.0f, 1.0f),
	RotationQuat(0.0f, 0.0f, 0.0f, 1.0f)
{
}

Keyframe::~Keyframe()
{
}

float BoneAnimation::GetStartTime()const
{
	// Keyframes are sorted by time, so first keyframe gives start time.
	return Keyframes.front().TimePos;
}

float BoneAnimation::GetEndTime()const
{
	// Keyframes are sorted by time, so last keyframe gives end time.
	float f = Keyframes.back().TimePos;

	return f;
}

void BoneAnimation::Interpolate(float t, XMFLOAT4X4& M)const
{
	if (t <= Keyframes.front().TimePos)
	{
		XMVECTOR S = XMLoadFloat3(&Keyframes.front().Scale);
		XMVECTOR P = XMLoadFloat3(&Keyframes.front().Translation);
		XMVECTOR Q = XMLoadFloat4(&Keyframes.front().RotationQuat);

		XMVECTOR zero = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
		XMStoreFloat4x4(&M, XMMatrixAffineTransformation(S, zero, Q, P));
	}
	else if (t >= Keyframes.back().TimePos)
	{
		XMVECTOR S = XMLoadFloat3(&Keyframes.back().Scale);
		XMVECTOR P = XMLoadFloat3(&Keyframes.back().Translation);
		XMVECTOR Q = XMLoadFloat4(&Keyframes.back().RotationQuat);

		XMVECTOR zero = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
		XMStoreFloat4x4(&M, XMMatrixAffineTransformation(S, zero, Q, P));
	}
	else
	{
		for (UINT i = 0; i < Keyframes.size() - 1; ++i)
		{
			if (t >= Keyframes[i].TimePos && t <= Keyframes[i + 1].TimePos)
			{
				float lerpPercent = (t - Keyframes[i].TimePos) / (Keyframes[i + 1].TimePos - Keyframes[i].TimePos);

				XMVECTOR s0 = XMLoadFloat3(&Keyframes[i].Scale);
				XMVECTOR s1 = XMLoadFloat3(&Keyframes[i + 1].Scale);

				XMVECTOR p0 = XMLoadFloat3(&Keyframes[i].Translation);
				XMVECTOR p1 = XMLoadFloat3(&Keyframes[i + 1].Translation);

				XMVECTOR q0 = XMLoadFloat4(&Keyframes[i].RotationQuat);
				XMVECTOR q1 = XMLoadFloat4(&Keyframes[i + 1].RotationQuat);

				XMVECTOR S = XMVectorLerp(s0, s1, lerpPercent);
				XMVECTOR P = XMVectorLerp(p0, p1, lerpPercent);
				XMVECTOR Q = XMQuaternionSlerp(q0, q1, lerpPercent);

				XMVECTOR zero = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
				XMStoreFloat4x4(&M, XMMatrixAffineTransformation(S, zero, Q, P));

				break;
			}
		}
	}
}



void BoneAnimation::to_byte(std::ofstream& outStream) const
{
	// Keyframes ũ�� ����
	uint32_t keyframeCount = Keyframes.size();
	outStream.write(reinterpret_cast<const char*>(&keyframeCount), sizeof(keyframeCount));

	// �� Keyframe ����
	for (const auto& keyframe : Keyframes)
	{
		// Keyframe�� �� ��� ���� ����ȭ
		outStream.write(reinterpret_cast<const char*>(&keyframe.TimePos), sizeof(keyframe.TimePos));
		outStream.write(reinterpret_cast<const char*>(&keyframe.Translation), sizeof(keyframe.Translation));
		outStream.write(reinterpret_cast<const char*>(&keyframe.Scale), sizeof(keyframe.Scale));
		outStream.write(reinterpret_cast<const char*>(&keyframe.RotationQuat), sizeof(keyframe.RotationQuat));
	}
}

void BoneAnimation::from_byte(std::ifstream& inStream)
{
	// Keyframes ũ�� �б�
	uint32_t keyframeCount;
	inStream.read(reinterpret_cast<char*>(&keyframeCount), sizeof(keyframeCount));
	Keyframes.resize(keyframeCount);

	// �� Keyframe �б�
	for (auto& keyframe : Keyframes)
	{
		// Keyframe�� �� ��� ���� ������ȭ
		inStream.read(reinterpret_cast<char*>(&keyframe.TimePos), sizeof(keyframe.TimePos));
		inStream.read(reinterpret_cast<char*>(&keyframe.Translation), sizeof(keyframe.Translation));
		inStream.read(reinterpret_cast<char*>(&keyframe.Scale), sizeof(keyframe.Scale));
		inStream.read(reinterpret_cast<char*>(&keyframe.RotationQuat), sizeof(keyframe.RotationQuat));
	}
}



float AnimationClip::GetClipStartTime()const
{
	// Find smallest start time over all bones in this clip.
	float t = MathHelper::Infinity;
	for (UINT i = 0; i < BoneAnimations.size(); ++i)
	{
		t = MathHelper::Min(t, BoneAnimations[i].GetStartTime());
	}

	return t;
}

float AnimationClip::GetClipEndTime()const
{
	// Find largest end time over all bones in this clip.
	float t = 0.0f;
	for (UINT i = 0; i < BoneAnimations.size(); ++i)
	{
		t = MathHelper::Max(t, BoneAnimations[i].GetEndTime());
	}

	return t;
}

void AnimationClip::Interpolate(float t, std::vector<XMFLOAT4X4>& boneTransforms)const
{
	for (UINT i = 0; i < BoneAnimations.size(); ++i)
	{
		BoneAnimations[i].Interpolate(t, boneTransforms[i]);
	}
}

void AnimationClip::to_byte(std::ofstream& outStream) const
{
	// �ִϸ��̼� �̸� ���̿� ������ ����
	uint32_t nameLength = Name.size();
	outStream.write(reinterpret_cast<const char*>(&nameLength), sizeof(nameLength));
	outStream.write(Name.c_str(), nameLength);

	// BoneAnimations ����
	uint32_t boneAnimCount = BoneAnimations.size();
	outStream.write(reinterpret_cast<const char*>(&boneAnimCount), sizeof(boneAnimCount));
	for (const auto& boneAnim : BoneAnimations)
	{
		boneAnim.to_byte(outStream); // BoneAnimation Ŭ������ to_byte ȣ�� 
	}
}

void AnimationClip::from_byte(std::ifstream& inStream)
{
	// �ִϸ��̼� �̸� �б�
	uint32_t nameLength;
	inStream.read(reinterpret_cast<char*>(&nameLength), sizeof(nameLength));
	Name.resize(nameLength);
	inStream.read(&Name[0], nameLength);

	// BoneAnimations �б�
	uint32_t boneAnimCount;
	inStream.read(reinterpret_cast<char*>(&boneAnimCount), sizeof(boneAnimCount));
	BoneAnimations.resize(boneAnimCount);
	for (auto& boneAnim : BoneAnimations)
	{
		boneAnim.from_byte(inStream); // BoneAnimation Ŭ������ from_byte ȣ��
	}
}






float SkinnedData::GetClipStartTime(const std::string& clipName)const
{
	auto clip = _animations.find(clipName);
	return clip->second.GetClipStartTime();
}

float SkinnedData::GetClipEndTime(const std::string& clipName)const
{
	auto clip = _animations.find(clipName);
	return clip->second.GetClipEndTime();
}

SkinnedData::SkinnedData()
{
}

SkinnedData::~SkinnedData()
{
}

uint32 SkinnedData::BoneCount()const
{
	return BoneHierarchy.size();
}

void SkinnedData::Set(std::vector<int>& boneHierarchy,
	std::vector<XMFLOAT4X4>& boneOffsets,
	std::map<std::string, AnimationClip>& animations)
{
	BoneHierarchy = boneHierarchy;
	BoneOffsets = boneOffsets;
	_animations = animations;
}

void SkinnedData::GetFinalTransforms(const std::string& clipName, float timePos, std::vector<XMFLOAT4X4>& finalTransforms)const
{
	uint32 numBones = BoneOffsets.size();

	std::vector<XMFLOAT4X4> toParentTransforms(numBones);

	// Interpolate all the bones of this clip at the given time instance.
	auto clip = _animations.find(clipName);
	clip->second.Interpolate(timePos, toParentTransforms);

	//
	// Traverse the hierarchy and transform all the bones to the root space.
	//

	std::vector<XMFLOAT4X4> toRootTransforms(numBones);

	// The root bone has index 0.  The root bone has no parent, so its toRootTransform
	// is just its local bone transform.
	toRootTransforms[0] = toParentTransforms[0];

	// Now find the toRootTransform of the children.
	for (uint32 i = 1; i < numBones; ++i)
	{
		XMMATRIX toParent = XMLoadFloat4x4(&toParentTransforms[i]);

		int parentIndex = BoneHierarchy[i];
		XMMATRIX parentToRoot = XMLoadFloat4x4(&toRootTransforms[parentIndex]);

		XMMATRIX toRoot = XMMatrixMultiply(toParent, parentToRoot);

		XMStoreFloat4x4(&toRootTransforms[i], toRoot);
	}

	// Premultiply by the bone offset transform to get the final transform.
	for (uint32 i = 0; i < numBones; ++i)
	{
		XMMATRIX offset = XMLoadFloat4x4(&BoneOffsets[i]);
		XMMATRIX toRoot = XMLoadFloat4x4(&toRootTransforms[i]);
		XMStoreFloat4x4(&finalTransforms[i], XMMatrixMultiply(offset, toRoot));
	}
}

void SkinnedData::from_byte(ifstream& inStream)
{
	// _boneHierarchy �б�
	uint32_t boneCount;
	inStream.read(reinterpret_cast<char*>(&boneCount), sizeof(boneCount));  
	BoneHierarchy.resize(boneCount); 
	inStream.read(reinterpret_cast<char*>(BoneHierarchy.data()), boneCount * sizeof(int)); 

	// _boneOffsets �б�
	uint32_t offsetCount;
	inStream.read(reinterpret_cast<char*>(&offsetCount), sizeof(offsetCount));
	BoneOffsets.resize(offsetCount);
	inStream.read(reinterpret_cast<char*>(BoneOffsets.data()), offsetCount * sizeof(XMFLOAT4X4));

	// _animations �б�
	uint32_t animCount;
	inStream.read(reinterpret_cast<char*>(&animCount), sizeof(animCount));
	for (uint32_t i = 0; i < animCount; ++i)
	{
		uint32_t nameLength;
		inStream.read(reinterpret_cast<char*>(&nameLength), sizeof(nameLength));

		std::string clipName(nameLength, ' ');
		inStream.read(clipName.data(), nameLength);

		AnimationClip animation;
		animation.from_byte(inStream); // AnimationClip Ŭ������ from_byte �Լ� �ʿ�

		AnimationClips.push_back(animation); 
	}
}

void SkinnedData::to_byte(ofstream& outStream)
{
	// _boneHierarchy ����
	uint32_t boneCount = BoneHierarchy.size();
	outStream.write(reinterpret_cast<const char*>(&boneCount), sizeof(boneCount));
	outStream.write(reinterpret_cast<const char*>(BoneHierarchy.data()), boneCount * sizeof(int));

	// _boneOffsets ����
	uint32_t offsetCount = BoneOffsets.size();
	outStream.write(reinterpret_cast<const char*>(&offsetCount), sizeof(offsetCount));
	outStream.write(reinterpret_cast<const char*>(BoneOffsets.data()), offsetCount * sizeof(XMFLOAT4X4));

	// _animations ����
	uint32_t animCount = AnimationClips.size();
	outStream.write(reinterpret_cast<const char*>(&animCount), sizeof(animCount));
	for (const auto& animation : AnimationClips) 
	{
		uint32_t nameLength = animation.Name.size();
		outStream.write(reinterpret_cast<const char*>(&nameLength), sizeof(nameLength));
		outStream.write(animation.Name.c_str(), nameLength);  

		// AnimationClip Ŭ������ to_byte�� �־�� ��
		animation.to_byte(outStream);
	}
}
