#include "pch.h"
#include "TimeManager.h"

SINGLE_BODY(TimeManager)

TimeManager::TimeManager()
	: m_CurCount{}
	, m_Frequency{}
	, m_PrevCount{}
	, m_dDT(0.0f)
	, m_iCallCount(0)
	, m_dAcc(0)
	, mbStoped(false)
{

}

TimeManager::~TimeManager()
{

}

void TimeManager::Init()
{
	QueryPerformanceCounter(&m_PrevCount);
	QueryPerformanceFrequency(&m_Frequency);
}

void TimeManager::Update()
{
	QueryPerformanceCounter(&m_CurCount);
	// ���� �������� ī���ð�, ���� ������ ī���� ���� ���̸� ���Ѵ�.
	m_dDT = (double)(m_CurCount.QuadPart - m_PrevCount.QuadPart) / (double)m_Frequency.QuadPart;

	// ����ī��Ʈ ���� ���簪���� ���� (�������� ����� ���ؼ�)
	m_PrevCount = m_CurCount;
	m_iFPS = 1.f / m_dDT;

	//#ifdef _DEBUG
	//	//if (m_dDT > (1. / 60.))
	//	//	m_dDT = (1. / 60.);
	//#endif // DEBUG
}
