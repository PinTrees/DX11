#pragma once

class TimeManager
{
	SINGLE_HEADER(TimeManager)

private:
	LARGE_INTEGER m_CurCount;
	LARGE_INTEGER m_PrevCount;
	LARGE_INTEGER m_Frequency;

	double		m_dDT;			//������ ���� �ð��� ��ŸŸ��
	double		m_dAcc;			// 1�� ���� �ð� üũ
	UINT		m_iCallCount;	// �ʴ� ȣ�� Ƚ��
	UINT		m_iFPS;			//������

	bool		mbStoped;

public:
	void Init();
	void Update();

public:
	double GetDT() { return m_dDT; }
	float GetfDT() { return (float)m_dDT; }

	UINT GetFrame() { return m_iFPS; }
};

