#pragma once

///<summary>
/// A Keyframe defines the bone transformation at an instant in time.
///</summary>
struct Keyframe
{
	Keyframe();
	~Keyframe();

public:
	float TimePos;				// �Ľ̵�
	XMFLOAT3 Translation;		// �Ľ̵�
	XMFLOAT3 Scale;				// �Ľ̵�
	XMFLOAT4 RotationQuat;		// �Ľ̵�
};




///<summary>
/// BoneAnimation�� Ű������ ������� ���ǵ˴ϴ�. �� ���� Ű������ ������ �ð� ���� ���ؼ���, 
/// �� �ð��� �����ϴ� �� ���� ����� Ű�������� �̿��� �����մϴ�. 
///
/// �ִϸ��̼��� �׻� �ּ� �� ���� Ű�������� �����ϴ�.
///</summary>
struct BoneAnimation
{
	float GetStartTime()const;
	float GetEndTime()const;

	void Interpolate(float t, XMFLOAT4X4& M)const;

public:
	std::vector<Keyframe> Keyframes;		// �Ľ̵�

public:
	// Editor
	void to_byte(std::ofstream& outStream) const;
	void from_byte(std::ifstream& inStream);
};




///<summary>
/// "Walk", "Run", "Attack", "Defend"�� ���� �ִϸ��̼� Ŭ���� ���� �� �� �ֽ��ϴ�.
/// AnimationClip�� �ִϸ��̼� Ŭ���� �����ϱ� ���� ��� ���� ���� BoneAnimation�� �ʿ�� �մϴ�.
///</summary>
struct AnimationClip
{
	float GetClipStartTime()const;
	float GetClipEndTime()const;

	void Interpolate(float t, vector<XMFLOAT4X4>& boneTransforms)const;

public:
	vector<BoneAnimation>	BoneAnimations;			// �Ľ̵�
	string					Name;					// �Ľ̵�


public:
	// Editor
	void to_byte(std::ofstream& outStream) const; 
	void from_byte(std::ifstream& inStream); 
};






class SkinnedData
{
public:
	SkinnedData(); 
	~SkinnedData(); 

	uint32 BoneCount() const;

	float GetClipStartTime(const std::string& clipName) const;
	float GetClipEndTime(const std::string& clipName) const;

	void Set(
		std::vector<int>& boneHierarchy,
		std::vector<XMFLOAT4X4>& boneOffsets,
		std::map<std::string, AnimationClip>& animations);

	// In a real project, you'd want to cache the result if there was a chance
	// that you were calling this several times with the same clipName at 
	// the same timePos.
	void GetFinalTransforms(const std::string& clipName, float timePos,
		std::vector<XMFLOAT4X4>& finalTransforms)const;

public:
	vector<shared_ptr<AnimationClip>>	AnimationClips;		// �Ľ̵�
	vector<int>							BoneHierarchy;		// �Ľ̵�
	vector<XMFLOAT4X4>					BoneOffsets;		// �Ľ̵�

private:
	// Gives parentIndex of ith bone.
	std::map<std::string, AnimationClip> _animations;

public:
	// Editor
	void from_byte(ifstream& inStream);
	void to_byte(ofstream& outStream);
};
